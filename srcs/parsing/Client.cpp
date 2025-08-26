#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const ServerConfig *config) :
	 _fd(fd), _recvSize(0), httpReq(new HttpRequest(*config)), httpRes(new HttpResponse(httpReq)){}

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
	size_t pos = _reqBuffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return false;
	return true;
}