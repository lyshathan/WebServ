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

int	CgiHandler::getStdinFd () const { return _stdinFd; }

int	CgiHandler::getStdoutFd () const { return _stdoutFd; }

bool CgiHandler::headersParsed () const { return _headersParsed; }

std::map<std::string, std::string> CgiHandler::getCgiHeaders() const { return _cgiHeaders; }

CgiState CgiHandler::getCgiStage () const { return _cgiStage; }

void CgiHandler::handleEvent(struct pollfd &pfd, std::vector<int> &removeFds) {
	if (pfd.revents & (POLLERR | POLLNVAL)) {
        if (pfd.fd == _stdoutFd) {
            IOStatus r = handleRead();
            if (r == IO_INCOMPLETE) {
                return;
            }
        }
        markError("Poll error");
        cleanUp(removeFds);
        return;
    }

	if ((pfd.revents & POLLHUP) && pfd.fd == _stdoutFd) {
		handleRead();
		handleCompletion();
		markDone();
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
			return;
		} else if (res == IO_ERROR) {
			markError("Read error");
			cleanUp(removeFds);
			return;
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

			if (_headersParsed && _headerPos != std::string::npos)
				_finalResponse = _outputBuffer.substr(_headerPos);
			else
				_finalResponse = _outputBuffer;

			_client->httpReq->setCGIResult(_finalResponse);
			_client->httpReq->setStatus(OK);
			_client->httpRes->parseResponse();
			markDone();
		} else {
			_client->httpReq->setStatus(INTERNAL_ERROR);
			markError("CGI exited abnormally");
		}
	} else if (result == -1) {
		_client->httpReq->setStatus(INTERNAL_ERROR);
		markError("waitpid() failed");
	}
}

IOStatus	CgiHandler::handleWrite() {
	size_t remaining = _inputBuffer.size() - _bytesWritten;

	if (remaining == 0)
		return IO_COMPLETE;

	_client->updateActivity();
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
	char buffer[4096];

	_client->updateActivity();
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
			size_t expected = atoi(it->second.c_str());
			if (_outputBuffer.size() >= expected) {
				return IO_COMPLETE;
			}
		}
	}
	return IO_INCOMPLETE;
}