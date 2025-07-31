#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd) : _fd(fd), httpReq(new HttpRequest()), httpRes(new HttpResponse(httpReq)),
	_recvSize(0) {
	std::cout << "Client created with fd: " << _fd << "\n";
}

Client::~Client() {
	delete httpReq;
	delete httpRes;
}

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

const std::string &Client::getRes() const { return _reqBuffer;}

bool	Client::appendBuffer(const char *data, size_t size) {
	_reqBuffer += std::string(data, size);
	_recvSize += size;
	return true;
}

bool Client::isReqComplete() const {
	size_t pos = _reqBuffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return false;
	std::cout << "FOUND at position: " << pos << std::endl;
	return true;
}