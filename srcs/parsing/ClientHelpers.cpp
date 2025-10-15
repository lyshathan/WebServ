#include "Client.hpp"

void Client::resetClient() {
	_reqBuffer = "";
	_resBuffer = "";
	_bytesSent = 0;
	_recvSize = 0;
	_clientIP = "";
	_state = READING_HEADERS;
	httpReq->setHeadersParsed();
	httpReq->cleanReqInfo();
}

bool Client::isCGI() {
	int	status = httpReq->getStatus();
	if (status == CGI_PENDING) {
		_state = CGI_PROCESSING;
		return true;
	}
	return false;
}

bool Client::connectionShouldClose() const {
	size_t pos = httpRes->getResHeaders().find("Connection: keep-alive");
	if (pos != std::string::npos)
		return false;
	return true;
}

bool Client::appendBuffer(const char *data, size_t size) {
	_reqBuffer.insert(_reqBuffer.end(), data, data + size);
	_recvSize += size;
	return true;
}

bool Client::isReqComplete() const {
	std::map<std::string, std::string> headers = httpReq->getHeaders();

	size_t headerEnd = _reqBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;

	std::map<std::string, std::string>::const_iterator it = headers.find("transfer-encoding");
	if (it != headers.end() && it->second.find("chunked") != std::string::npos) {
		size_t zeroChunkPos = _reqBuffer.find("0\r\n");
		if (zeroChunkPos != std::string::npos) {
			size_t finalEnd = _reqBuffer.find("\r\n\r\n", zeroChunkPos);
			return finalEnd != std::string::npos;
		}
		return false;
	}

	size_t maxBodySize = httpReq->getMaxBody();
	it = headers.find("content-length");
	if (it != headers.end()) {
		std::istringstream iss(it->second);
		size_t contentLength = 0;
		iss >> contentLength;
		size_t bodyReceived = _reqBuffer.length() - (headerEnd + 4);
		if (contentLength > maxBodySize)
			httpReq->setStatus(413);
		return bodyReceived >= contentLength;
	}
	return true;
}

void Client::updateActivity() { _lastActivity = time(NULL); }