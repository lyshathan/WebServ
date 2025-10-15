#include "CgiHandler.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

CgiHandler::CgiHandler(Client *client) : _client(client) {}

CgiHandler::~CgiHandler() {
	// Kill the CGI process if it's still running
	if (_pid > 0) {
		kill(_pid, SIGTERM);
		usleep(100000);
		
		int status;
		pid_t result = waitpid(_pid, &status, WNOHANG);
		if (result == 0) {
			kill(_pid, SIGKILL);
			waitpid(_pid, &status, 0);
		}
	}
	
	// Close FDs if still open
	if (_stdinFd > 0) {
		close(_stdinFd);
	}
	if (_stdoutFd > 0) {
		close(_stdoutFd);
	}
}

/******************************************************************************/
/*						CGI I/O HANDLER										  */
/******************************************************************************/

int	CgiHandler::getStdinFd () const { return _stdinFd; }

int	CgiHandler::getStdoutFd () const { return _stdoutFd; }

CgiState CgiHandler::getCgiStage () const { return _cgiStage; }

void CgiHandler::handleEvent(struct pollfd &pfd, std::vector<int> &removeFds) {
	if (pfd.revents & (POLLERR | POLLNVAL)) {
        std::cerr << "[CGI] POLLERR/POLLNVAL on fd " << pfd.fd << ", attempting to drain\n";
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
		std::cout << "Calling handle competition\n";
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
			std::cout << "Calling handle competition 1\n";
			handleCompletion();
			return;
		} else if (res == IO_ERROR) {
			markError("Read error");
			cleanUp(removeFds);
			return;
		}
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
	// tryParseCGIHeaders(cgiState);

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

void		CgiHandler::handleCompletion() {

	if (!_client || !_client->httpReq || !_client->httpRes) {
    	std::cerr << "Null pointer detected!" << std::endl;
    	return;
	}
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


// void Client::parseCGIHeaders(CgiState *cgiState, size_t headerEnd) { std::string headers = cgiState->response_buffer.substr(0, headerEnd); while (!headers.empty()) { size_t headerLine = headers.find("\n"); if (headerLine == std::string::npos) break; std::string line = headers.substr(0, headerLine); if (!line.empty() && line[line.length() - 1] == '\r') line.erase(line.length() - 1); size_t colonPos = line.find(":"); if (colonPos != std::string::npos) { std::string key = line.substr(0, colonPos); std::string value = line.substr(colonPos + 1); while (!value.empty() && value[0] == ' ') value.erase(0, 1); while (!value.empty() && value[value.length() - 1] == ' ') value.erase(value.length() - 1); std::transform(key.begin(), key.end(), key.begin(), ::tolower); cgiState->_headers[key] = value; } headers = headers.substr(headerLine + 1); } } void Client::parseSimpleCGIHeaders(CgiState *cgiState, size_t headerEnd) { std::string headers = cgiState->response_buffer.substr(0, headerEnd); std::string line; size_t start = 0; while (start < headers.length()) { size_t lineEnd = headers.find('\n', start); if (lineEnd == std::string::npos) break; line = headers.substr(start, lineEnd - start); if (!line.empty() && line[line.length() - 1] == '\r') line.erase(line.length() - 1); size_t colonPos = line.find(':'); if (colonPos != std::string::npos) { std::string key = line.substr(0, colonPos); std::string value = line.substr(colonPos + 1); while (!value.empty() && value[0] == ' ') value.erase(0, 1); while (!value.empty() && value[value.length() - 1] == ' ') value.erase(value.length() - 1); std::transform(key.begin(), key.end(), key.begin(), ::tolower); cgiState->_headers[key] = value; } start = lineEnd + 1; } } void Client::tryParseCGIHeaders(CgiState *cgiState) { if (cgiState->headers_parsed) return; size_t headerEnd = cgiState->response_buffer.find("\r\n\r\n"); if (headerEnd == std::string::npos) { headerEnd = cgiState->response_buffer.find("\n\n"); if (headerEnd != std::string::npos) headerEnd += 2; } else headerEnd += 4; cgiState->headerPos = headerEnd; if (headerEnd != std::string::npos) { parseSimpleCGIHeaders(cgiState, headerEnd); cgiState->headers_parsed = true; } }

// void Webserv::addCGIToPoll(Client *client, struct pollfd &pfd, std::vector<struct pollfd> &newPollFds) {
// 	std::cerr << "\033[36m[DEBUG] Entering addCGIToPoll for client fd " << pfd.fd << "\033[0m" << std::endl;
// 	CgiState *cgi = client->httpReq->getCGIState();

// 	struct pollfd stdout_pollfd = {cgi->stdout_fd, POLLIN, 0};
// 	_cgiToClient[cgi->stdout_fd] = pfd.fd;
// 	newPollFds.push_back(stdout_pollfd);
// 	std::cerr << "\033[35m[DEBUG] Added CGI stdout_fd " << cgi->stdout_fd << " to poll (POLLIN)\033[0m" << std::endl;

// 	if (cgi->state == CgiState::WRITING && cgi->stdin_fd != -1) {
// 		struct pollfd stdin_pollfd = {cgi->stdin_fd, POLLOUT, 0};
// 		_cgiToClient[cgi->stdin_fd] = pfd.fd;
// 		newPollFds.push_back(stdin_pollfd);
// 		std::cerr << "\033[35m[DEBUG] Added CGI stdin_fd " << cgi->stdin_fd << " to poll (POLLOUT)\033[0m" << std::endl;
// 	}
// 	std::cerr << "\033[36m[DEBUG] Exiting addCGIToPoll\033[0m" << std::endl;
// }