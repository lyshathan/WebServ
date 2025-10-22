#include "Client.hpp"
#include "response/HttpResponse.hpp"

int	Client::writeResponse() {
	updateActivity();

	if (_state == REQUEST_READY) { 
		_resBuffer.clear();
		_resBuffer += httpRes->getResHeaders();

		std::vector<char> binaryContent = httpRes->getBinRes();
		_resBuffer.append(binaryContent.begin(), binaryContent.end());
		
		_bytesSent = 0;
		_state = SENDING_RESPONSE;
	}

	ssize_t sent = send(_fd, _resBuffer.c_str() + _bytesSent,_resBuffer.length() - _bytesSent, MSG_NOSIGNAL);
	if (sent <= 0)
		return WRITE_ERROR;

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

	updateActivity();

	bytesRead = recv(_fd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		if (bytesRead == 0) {
			if (_reqBuffer.empty()) {
				return READ_ERROR;
			}
			return READ_COMPLETE;
		}
		return READ_ERROR;
	}

	appendBuffer(buffer, bytesRead);

	if (_state == READING_HEADERS || _state == CONNECTION_IDLE) {
		if (!httpReq->getHeadersParsed()) {
			int status = httpReq->requestHeaderParser(_reqBuffer);
			if (status == 1)
				return READ_INCOMPLETE;
			else if (status == -1) {
				httpReq->setErrorPage();
				_state = REQUEST_READY;
				return READ_HEADER_ERROR;
			} else {
				_state = READING_BODY;
			}
		}
	} if (_state == READING_BODY) {
		if (isReqComplete()) {
			httpReq->requestBodyParser(_reqBuffer);
			_state = REQUEST_READY;
			return READ_COMPLETE;
		}
		return READ_INCOMPLETE;
	}
	return READ_INCOMPLETE;
}