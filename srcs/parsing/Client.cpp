#include "Client.hpp"

Client::Client() : _fd(-1), httpReq(NULL), httpRes(NULL) {}

Client::Client(int fd) : _fd(fd), httpReq(new HttpRequest()), httpRes(new HttpResponse()){
	std::cout << "Client created with fd: " << _fd << "\n";
}

Client::~Client() {
	if (httpReq)
		delete httpReq;
	if (httpRes)
		delete httpRes;
}
