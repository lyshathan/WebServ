#include "Client.hpp"
#include "response/HttpResponse.hpp"

int	Client::writeResponse() {
	updateActivity();
	// std::cerr << "\033[36m[DEBUG] Entering writeResponse for fd " << _fd << "\033[0m" << std::endl;
	if (_state == REQUEST_READY) {  // Get the response header + body and clean the response buffer
		// std::cerr << "\033[32m[DEBUG] State is REQUEST_READY, preparing response buffer\033[0m" << std::endl;
		_resBuffer.clear();
		_resBuffer += httpRes->getResHeaders();

		if (httpRes->getIsTextContent()) {
			// std::cerr << "\033[33m[DEBUG] Appending text content to response\033[0m" << std::endl;
			_resBuffer += httpRes->getRes();
		} else {
			// std::cerr << "\033[33m[DEBUG] Appending binary content to response\033[0m" << std::endl;
			std::vector<char> binaryContent = httpRes->getBinRes();
			_resBuffer.append(binaryContent.begin(), binaryContent.end());
		}
		_bytesSent = 0;
		_state = SENDING_RESPONSE;
	}

	ssize_t sent = send(_fd, _resBuffer.c_str() + _bytesSent,
						_resBuffer.length() - _bytesSent, MSG_NOSIGNAL);
	// std::cerr << "\033[33m[DEBUG] send() returned " << sent << " bytes for fd " << _fd << "\033[0m" << std::endl;
	if (sent <= 0) {
		// std::cerr << "\033[31m[DEBUG] WRITE_ERROR on fd " << _fd << "\033[0m" << std::endl;
		return WRITE_ERROR;
	}

	_bytesSent += sent;

	if (_bytesSent >= _resBuffer.length()) {
		// std::cerr << "\033[32m[DEBUG] All bytes sent, marking DONE for fd " << _fd << "\033[0m" << std::endl;
		_state = DONE;
		// std::cerr << "\033[36m[DEBUG] Exiting writeResponse for fd " << _fd << "\033[0m" << std::endl;
		return WRITE_COMPLETE;
	}

	// std::cerr << "\033[33m[DEBUG] Partial write, bytesSent=" << _bytesSent << "/" << _resBuffer.length() << "\033[0m" << std::endl;
	// std::cerr << "\033[36m[DEBUG] Exiting writeResponse for fd " << _fd << "\033[0m" << std::endl;
	return WRITE_INCOMPLETE;
}

int	Client::readAndParseRequest() {
	updateActivity();
	// std::cerr << "\033[36m[DEBUG] Entering readAndParseRequest for fd " << _fd << "\033[0m" << std::endl;
	char	buffer[BUFSIZ];
	ssize_t	bytesRead;

	bytesRead = recv(_fd, buffer, BUFSIZ, 0);
	// std::cerr << "\033[33m[DEBUG] recv() returned " << bytesRead << " bytes for fd " << _fd << "\033[0m" << std::endl;
	if (bytesRead <= 0)
	{
		if (bytesRead == 0) {
			// if we have no data in buffer and peer closed, treat as error/disconnect
			if (_reqBuffer.empty()) {
				// std::cerr << "\033[31m[DEBUG] READ_ERROR (peer closed without data) for fd " << _fd << "\033[0m" << std::endl;
				return READ_ERROR;
			}
			// std::cerr << "\033[32m[DEBUG] READ_COMPLETE (peer closed after sending) for fd " << _fd << "\033[0m" << std::endl;
			return READ_COMPLETE;
		}
		// std::cerr << "\033[31m[DEBUG] READ_ERROR for fd " << _fd << "\033[0m" << std::endl;
		return READ_ERROR;
	}

	appendBuffer(buffer, bytesRead); // keeps appending the buffer

	if (_state == READING_HEADERS) {
		// std::cerr << "\033[33m[DEBUG] State is READING_HEADERS for fd " << _fd << "\033[0m" << std::endl;
		if (!httpReq->getHeadersParsed()) {
			int status = httpReq->requestHeaderParser(_reqBuffer);
			// std::cerr << "[DEBUG] requestHeaderParser returned " << status << " for fd " << _fd << std::endl;
			if (status == 1)
				return READ_INCOMPLETE; // there's still some headers to be parsed
			else if (status == -1) {
				// std::cerr << "\033[31m[DEBUG] Header parse error, marking READ_COMPLETE for fd " << _fd << "\033[0m" << std::endl;
				return READ_COMPLETE; // there was an error so we need to stop parsing and send response back to client
			} else {
				// std::cerr << "\033[32m[DEBUG] Headers parsed, switching to READING_BODY for fd " << _fd << "\033[0m" << std::endl;
				// check if request has body [TO DO]
				_state = READING_BODY; // headers parse finished
			}
		}
	} if (_state == READING_BODY) {
		// std::cerr << "\033[33m[DEBUG] State is READING_BODY for fd " << _fd << "\033[0m" << std::endl;
		if (isReqComplete()) {
			// std::cerr << "\033[32m[DEBUG] Request body complete, parsing and marking REQUEST_READY for fd " << _fd << "\033[0m" << std::endl;
			httpReq->requestBodyParser(_reqBuffer); // when body is fully received it should be parsed
			_state = REQUEST_READY;
			// std::cerr << "\033[36m[DEBUG] Exiting readAndParseRequest for fd " << _fd << "\033[0m" << std::endl;
			return READ_COMPLETE;
		}
		// std::cerr << "\033[33m[DEBUG] Request body not complete, waiting for more data for fd " << _fd << "\033[0m" << std::endl;
		return READ_INCOMPLETE;
	}
	// std::cerr << "\033[33m[DEBUG] Still waiting for more data for fd " << _fd << "\033[0m" << std::endl;
	// std::cerr << "\033[36m[DEBUG] Exiting readAndParseRequest for fd " << _fd << "\033[0m" << std::endl;
	return READ_INCOMPLETE;
}