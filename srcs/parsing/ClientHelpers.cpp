#include "Client.hpp"
#include "response/HttpResponse.hpp"

void Client::resetClient() {
	_reqBuffer = "";
	_resBuffer = "";
	_bytesSent = 0;
	_recvSize = 0;
	// _clientIP = "";
	_state = CONNECTION_IDLE;
	httpReq->setHeadersParsed();
	httpReq->cleanReqInfo();
}

bool Client::connectionShouldClose() const {
	 std::string headers = httpRes->getResHeaders();
	if (headers.find("Connection: close") != std::string::npos)
		return true;
	if (headers.find("Connection: keep-alive") != std::string::npos)
		return false;
	return true;
}

bool Client::appendBuffer(const char *data, size_t size) {
	_reqBuffer.insert(_reqBuffer.end(), data, data + size);
	_recvSize += size;
	return true;
}

bool Client::isCGI() {
	int	status = httpReq->getStatus();
	if (status == CGI_PENDING) {
		_state = CGI_PROCESSING;
		return true;
	}
	return false;
}

std::string Client::getStateString() const
{
	switch(_state)
	{
		case(READING_HEADERS) : return ("[reading header]");
		case(READING_BODY) : return ("[reading body]");
		case(CGI_PROCESSING) : return ("[cgi processing]");
		case(SENDING_RESPONSE) : return ("[writing]");
		default : return ("undefined");
	}
}

void Client::updateActivity() { _lastActivity = time(NULL); }