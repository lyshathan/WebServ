#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const Config &config) :
	 _fd(fd), _recvSize(0), httpReq(new HttpRequest(config, fd)), httpRes(new HttpResponse(httpReq)) {}

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
	httpReq->cleanReqInfo();
}

bool	Client::appendBuffer(const char *data, size_t size) {
	_reqBuffer += std::string(data, size);
	_recvSize += size;
	return true;
}

bool Client::isReqComplete() const {
	std::map<std::string, std::string> headers = httpReq->getHeaders();

	size_t headerEnd = _reqBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;

	std::map<std::string, std::string>::const_iterator it = headers.find("content-length");
	if (it != headers.end()) {
		size_t contentLength = std::stoul(it->second);
		size_t bodyReceived = _reqBuffer.length() - (headerEnd + 4);
		return bodyReceived >= contentLength;
	}

	// it = headers.find("transfer-encoding");
	// if (it != headers.end() && it->second.find("chunked") != std::string::npos) {
	// 	// Look for chunked transfer end marker "0\r\n\r\n"
	// 	return _reqBuffer.find("0\r\n\r\n") != std::string::npos;
	// }

	return true;
}