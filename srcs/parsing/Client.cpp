#include "Client.hpp"

Client::Client() {}

Client::Client(int fd) : _fd(fd), httpReq(new HttpRequest()), httpRes(new HttpResponse()){}

Client::~Client() {
	delete httpReq;
	delete httpRes;
}
