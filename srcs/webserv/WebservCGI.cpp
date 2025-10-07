#include "Webserv.hpp"
#include "../parsing/Client.hpp"

void Webserv::handleCGIWrite(int clientFd, CgiState *cgiState) {
	size_t remaining = cgiState->request_body.size() - cgiState->bytes_written;

	if (remaining <= 0) {
		closeCGIStdin(cgiState);
		return;
	}

	ssize_t written = write(cgiState->stdin_fd,
						cgiState->request_body.c_str() + cgiState->bytes_written,
						remaining);

	if (written > 0) {
		cgiState->bytes_written += written;
		if (cgiState->bytes_written >= cgiState->request_body.size()) {
			closeCGIStdin(cgiState);
		}
	} else {
		_clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
		cleanupCGI(clientFd, cgiState);
		processAndSendResponse(clientFd);
	}
}

void Webserv::parseCGIHeaders(CgiState *cgiState, size_t headerEnd) {
	std::string headers = cgiState->response_buffer.substr(0, headerEnd);

	while (!headers.empty()) {
		size_t headerLine = headers.find("\n");
		if (headerLine == std::string::npos) break;

		std::string line = headers.substr(0, headerLine);
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t colonPos = line.find(":");
		if (colonPos != std::string::npos) {
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			while (!value.empty() && value[0] == ' ') value.erase(0, 1);
			while (!value.empty() && value[value.length() - 1] == ' ') value.erase(value.length() - 1);

			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			cgiState->_headers[key] = value;
		}
		headers = headers.substr(headerLine + 1);
	}
}

void Webserv::parseSimpleCGIHeaders(CgiState *cgiState, size_t headerEnd) {
	std::string headers = cgiState->response_buffer.substr(0, headerEnd);
	std::string line;
	size_t start = 0;

	while (start < headers.length()) {
		size_t lineEnd = headers.find('\n', start);
		if (lineEnd == std::string::npos) break;

		line = headers.substr(start, lineEnd - start);
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			while (!value.empty() && value[0] == ' ') value.erase(0, 1);
			while (!value.empty() && value[value.length() - 1] == ' ') value.erase(value.length() - 1);

			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			cgiState->_headers[key] = value;
		}
		start = lineEnd + 1;
	}
}

void Webserv::tryParseCGIHeaders(CgiState *cgiState) {
	if (cgiState->headers_parsed)
		return;

	size_t headerEnd = cgiState->response_buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos) {
		headerEnd = cgiState->response_buffer.find("\n\n");
		if (headerEnd != std::string::npos)
			headerEnd += 2;
	} else
		headerEnd += 4;

	cgiState->headerPos = headerEnd;
	if (headerEnd != std::string::npos) {
		parseSimpleCGIHeaders(cgiState, headerEnd);
		cgiState->headers_parsed = true;
	}
}

void Webserv::handleCGIRead(int clientFd, CgiState *cgiState) {
	char buffer[4096];
	ssize_t bytesRead = read(cgiState->stdout_fd, buffer, sizeof(buffer));

	if (bytesRead > 0) {
		cgiState->response_buffer.append(buffer, bytesRead);

		tryParseCGIHeaders(cgiState);

		if (cgiState->headers_parsed) {
			std::map<std::string, std::string>::iterator it = cgiState->_headers.find("content-length");
			if (it != cgiState->_headers.end()) {
				size_t expected = atoi(it->second.c_str());
				if (cgiState->response_buffer.size() >= expected) {
					close(cgiState->stdout_fd);
					cgiState->stdout_fd = -1;
					handleCGICompletion(clientFd, cgiState);
					return;
				}
			}
		}
	} else if (bytesRead == 0) {
		close(cgiState->stdout_fd);
		cgiState->stdout_fd = -1;
		handleCGICompletion(clientFd, cgiState);
	} else {
		_clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
		cleanupCGI(clientFd, cgiState);
		processAndSendResponse(clientFd);
	}
}

void Webserv::handleCGICompletion(int clientFd, CgiState *cgiState) {
	int status;
	pid_t result = waitpid(cgiState->pid, &status, WNOHANG);

	if (result == cgiState->pid) {
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			std::string bodyContent;
			if (cgiState->headers_parsed && cgiState->headerPos != std::string::npos)
				bodyContent = cgiState->response_buffer.substr(cgiState->headerPos);
			else
				bodyContent = cgiState->response_buffer;
			_clients[clientFd]->httpReq->setCGIResult(bodyContent);
			_clients[clientFd]->httpReq->setStatus(OK);
		} else {
			_clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
		}

		cleanupCGI(clientFd, cgiState);
		processAndSendResponse(clientFd);
	} else if (result == -1) {
		_clients[clientFd]->httpReq->setStatus(INTERNAL_ERROR);
		cleanupCGI(clientFd, cgiState);
		processAndSendResponse(clientFd);
	}
}