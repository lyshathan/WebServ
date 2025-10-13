#include "Client.hpp"

int	Client::writeResponse() {
	if (_state == REQUEST_READY) {  // Get the response header + body and clean the response buffer
		_resBuffer.clear();
		_resBuffer += httpRes->getResHeaders();

		if (httpRes->getIsTextContent()) {
			_resBuffer += httpRes->getRes();
		} else {
			std::vector<char> binaryContent = httpRes->getBinRes();
			_resBuffer.append(binaryContent.begin(), binaryContent.end());
		}
		_bytesSent = 0;
		_state = SENDING_RESPONSE;
	}

	ssize_t sent = send(_fd, _resBuffer.c_str() + _bytesSent,
						_resBuffer.length() - _bytesSent, MSG_NOSIGNAL);
	if (sent <= 0) {
		return WRITE_ERROR;
	}

	_bytesSent += sent;

	if (_bytesSent >= _resBuffer.length()) {
		_state = DONE;
		return WRITE_COMPLETE;
	}

	return WRITE_INCOMPLETE;
}

int	Client::readAndParseRequest() {
	char	buffer[BUFSIZ];
	ssize_t	bytesRead;

	bytesRead = recv(_fd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return READ_INCOMPLETE; // No data yet, try again later
		else if (bytesRead == 0)
			return READ_COMPLETE;
		return READ_ERROR;
	}

	appendBuffer(buffer, bytesRead); // keeps appending the buffer

	if (_state == READING_HEADERS) {
		if (!httpReq->getHeadersParsed()) {
			int status = httpReq->requestHeaderParser(_reqBuffer);
			if (status == 1)
				return READ_INCOMPLETE; // there's still some headers to be parsed
			else if (status == -1)
				return READ_COMPLETE; // there was an error so we need to stop parsing and send response back to client
			else {
				// check if request has body [TO DO]
				_state = READING_BODY; // headers parse finished
			}
		}
	} if (_state == READING_BODY) {
		if (isReqComplete()) {
			httpReq->requestBodyParser(_reqBuffer); // when body is fully received it should be parsed
			_state = REQUEST_READY;
			return READ_COMPLETE;
		}
		return READ_INCOMPLETE;
	}
	return READ_INCOMPLETE;
}