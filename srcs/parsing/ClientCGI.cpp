#include "Client.hpp"

int Client::handleCGIWrite(CgiState *cgiState) {
	std::cerr << "\033[36m[DEBUG] Entering handleCGIWrite for CGI stdin_fd " << cgiState->stdin_fd << "\033[0m" << std::endl;
	size_t remaining = cgiState->request_body.size() - cgiState->bytes_written;

	if (remaining <= 0) {
		std::cerr << "\033[32m[DEBUG] All CGI request body written (WRITE_COMPLETE)\033[0m" << std::endl;
		return WRITE_COMPLETE;
	}

	ssize_t written = write(cgiState->stdin_fd, cgiState->request_body.c_str()
							+ cgiState->bytes_written, remaining);
	std::cerr << "\033[33m[DEBUG] write() to CGI stdin_fd returned " << written << " bytes\033[0m" << std::endl;

	if (written == 0) {
		std::cerr << "\033[32m[DEBUG] CGI stdin closed (WRITE_COMPLETE)\033[0m" << std::endl;
		return WRITE_COMPLETE;
	} else if (written < 0) {
		std::cerr << "\033[31m[DEBUG] CGI WRITE_ERROR\033[0m" << std::endl;
		httpReq->setStatus(INTERNAL_ERROR);
		return WRITE_ERROR;
	}
	cgiState->bytes_written += written;
	if (cgiState->bytes_written >= cgiState->request_body.size()) {
		std::cerr << "\033[32m[DEBUG] CGI request body fully written (WRITE_COMPLETE)\033[0m" << std::endl;
		return WRITE_COMPLETE;
	}
	std::cerr << "\033[33m[DEBUG] Partial CGI write, bytes_written=" << cgiState->bytes_written << "/" << cgiState->request_body.size() << "\033[0m" << std::endl;
	std::cerr << "\033[36m[DEBUG] Exiting handleCGIWrite\033[0m" << std::endl;
	return WRITE_INCOMPLETE;
}

int Client::handleCGIRead(CgiState *cgiState) {
	std::cerr << "\033[36m[DEBUG] Entering handleCGIRead for CGI stdout_fd " << cgiState->stdout_fd << "\033[0m" << std::endl;
	char buffer[4096];
	ssize_t bytesRead = read(cgiState->stdout_fd, buffer, sizeof(buffer));
	std::cerr << "\033[33m[DEBUG] read() from CGI stdout_fd returned " << bytesRead << " bytes\033[0m" << std::endl;

	if (bytesRead == 0) {
		std::cerr << "\033[32m[DEBUG] CGI stdout closed (READ_COMPLETE)\033[0m" << std::endl;
		return READ_COMPLETE;
	} else if (bytesRead < 0) {
		std::cerr << "\033[31m[DEBUG] CGI READ_ERROR\033[0m" << std::endl;
		httpReq->setStatus(INTERNAL_ERROR);
		return READ_ERROR;
	}

	cgiState->response_buffer.append(buffer, bytesRead);
	tryParseCGIHeaders(cgiState);

	if (cgiState->headers_parsed) {
		std::map<std::string, std::string>::iterator it = cgiState->_headers.find("content-length");
		if (it != cgiState->_headers.end()) {
			size_t expected = atoi(it->second.c_str());
			if (cgiState->response_buffer.size() >= expected) {
				std::cerr << "\033[32m[DEBUG] CGI response buffer reached content-length (READ_COMPLETE)\033[0m" << std::endl;
				return READ_COMPLETE;
			}
		}
	}
	std::cerr << "\033[33m[DEBUG] CGI response not complete, waiting for more data\033[0m" << std::endl;
	std::cerr << "\033[36m[DEBUG] Exiting handleCGIRead\033[0m" << std::endl;
	return READ_INCOMPLETE;
}

void Client::handleCGICompletion(CgiState *cgiState) {
	std::cerr << "\033[36m[DEBUG] Entering handleCGICompletion for CGI pid " << cgiState->pid << "\033[0m" << std::endl;
	int status;
	pid_t result = waitpid(cgiState->pid, &status, WNOHANG);

	if (result == cgiState->pid) {
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			std::cerr << "\033[32m[DEBUG] CGI process exited normally (OK)\033[0m" << std::endl;
			std::string bodyContent;
			if (cgiState->headers_parsed && cgiState->headerPos != std::string::npos)
				bodyContent = cgiState->response_buffer.substr(cgiState->headerPos);
			else
				bodyContent = cgiState->response_buffer;
			httpReq->setCGIResult(bodyContent);
			httpReq->setStatus(OK);
			httpRes->parseResponse();
			_state = REQUEST_READY;
		} else {
			std::cerr << "\033[31m[DEBUG] CGI process exited with error\033[0m" << std::endl;
			httpReq->setStatus(INTERNAL_ERROR);
		}
	} else if (result == -1) {
		std::cerr << "\033[31m[DEBUG] waitpid() failed for CGI process\033[0m" << std::endl;
		httpReq->setStatus(INTERNAL_ERROR);
	}
	std::cerr << "\033[36m[DEBUG] Exiting handleCGICompletion\033[0m" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////
//								CGI HEADER PARSER								   /
////////////////////////////////////////////////////////////////////////////////////

void Client::parseCGIHeaders(CgiState *cgiState, size_t headerEnd) {
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

void Client::parseSimpleCGIHeaders(CgiState *cgiState, size_t headerEnd) {
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

void Client::tryParseCGIHeaders(CgiState *cgiState) {
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