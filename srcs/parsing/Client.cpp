#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const Config &config, const std::string &clientIP) :
	 _fd(fd), _recvSize(0), _clientIP(clientIP), httpReq(new HttpRequest(config, fd, _clientIP)),
	httpRes(new HttpResponse(httpReq)) {}

Client::~Client() {
	delete httpReq;
	delete httpRes;
}

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

const std::string &Client::getRes() const { return _reqBuffer;}

void	Client::clearBuffer() {
	_reqBuffer = "";
	_recvSize = 0;
	httpReq->setHeadersParsed();
	httpReq->cleanReqInfo();
}

bool	Client::appendBuffer(const char *data, size_t size) {
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
		return _reqBuffer.find("0\r\n\r\n") != std::string::npos;
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