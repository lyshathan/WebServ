#include "CgiHandler.hpp"
#include "../parsing/response/HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

CgiHandler::CgiHandler(Client *client) : _client(client) {}

CgiHandler::~CgiHandler() {}

/******************************************************************************/
/*						CGI I/O HANDLER										  */
/******************************************************************************/


void CgiHandler::handleEvent(struct pollfd &pfd, std::vector<int> &removeFds) {

	if (pfd.revents & (POLLERR | POLLNVAL)) {
		markError("Poll error");
		cleanUp(removeFds);
		return;
	}

	if ((pfd.revents & POLLHUP) && pfd.fd == _stdoutFd) {
		if (_cgiStage == CGI_READING)
			handleRead();
		handleCompletion();
	}

	if ((pfd.revents & POLLOUT) && pfd.fd == _stdinFd && _cgiStage == CGI_WRITING) {
		IOStatus res = handleWrite();
		if (res == IO_COMPLETE) {
			removeFds.push_back(_stdinFd);
			_cgiStage = CGI_READING;
		} else if (res == IO_ERROR) {
			markError("Write error");
			cleanUp(removeFds);
		}
	}

	if ((pfd.revents & POLLIN) && pfd.fd == _stdoutFd && _cgiStage == CGI_READING) {
		IOStatus res = handleRead();
		if (res == IO_COMPLETE) {
			handleCompletion();
		} else if (res == IO_ERROR) {
			markError("Read error");
			cleanUp(removeFds);
		}
	}
}

void CgiHandler::handleCompletion() {
	if (!_client || !_client->httpReq || !_client->httpRes)
		return;

	int status;
	pid_t result = waitpid(_pid, &status, WNOHANG);
	
	if (result == _pid) {
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			parseBodySendResponse(OK);
			markDone();
		} else {
			parseBodySendResponse(INTERNAL_ERROR);
			markError("CGI exited abnormally");
		}
	} else if (result == -1) {
		parseBodySendResponse(INTERNAL_ERROR);
		markError("waitpid() failed");
	}
}

void	CgiHandler::parseBodySendResponse(int result) {
	_client->httpReq->setStatus(result);
	if (_headersParsed && _headerPos != std::string::npos) {
		std::map<std::string, std::string>::iterator it = _cgiHeaders.find("content-length");
		if (it != _cgiHeaders.end()) {
			size_t expected = static_cast<size_t>(atoi(it->second.c_str()));
			if (_outputBuffer.size() > _headerPos) {
				// Truncate to expected length if body is longer
				if (_outputBuffer.size() - _headerPos > expected)
					_finalResponse = _outputBuffer.substr(_headerPos, expected);
				else
					_finalResponse = _outputBuffer.substr(_headerPos);
			} else {
				_finalResponse = "";
			}
		} else {
			_finalResponse = _outputBuffer.substr(_headerPos);
		}
	} else {
		_finalResponse = _outputBuffer;
	}
	_client->httpReq->setCGIResult(_finalResponse);
	_client->httpRes->parseResponse();
}

IOStatus	CgiHandler::handleWrite() {
	_client->updateActivity();
	size_t remaining = _inputBuffer.size() - _bytesWritten;

	if (remaining == 0)
		return IO_COMPLETE;

	ssize_t written = write(_stdinFd, _inputBuffer.c_str() + _bytesWritten, remaining);

	if (written <= 0) {
		_client->httpReq->setStatus(INTERNAL_ERROR);
		return IO_ERROR;
	}

	_bytesWritten += static_cast<size_t>(written);
	if (_bytesWritten >= _inputBuffer.size())
		return IO_COMPLETE;

	return IO_INCOMPLETE;
}

IOStatus	CgiHandler::handleRead() {
	_client->updateActivity();
	char buffer[4096];

	ssize_t bytesRead = read(_stdoutFd, buffer, sizeof(buffer));

	if (bytesRead == 0)
		return IO_COMPLETE;
	else if (bytesRead < 0) {
		_client->httpReq->setStatus(INTERNAL_ERROR);
		return IO_ERROR;
	}

	_outputBuffer.append(buffer, bytesRead);
	tryParseCGIHeaders();

	if (_headersParsed) {
		std::map<std::string, std::string>::iterator it = _cgiHeaders.find("content-length");
		if (it != _cgiHeaders.end()) {
			size_t expected = static_cast<size_t>(atoi(it->second.c_str()));
			size_t bodySize = 0;
			if (_headerPos != std::string::npos && _outputBuffer.size() > _headerPos)
				bodySize = _outputBuffer.size() - _headerPos;
			if (bodySize >= expected) {
				return IO_COMPLETE;
			}
		}
	}
	return IO_INCOMPLETE;
}