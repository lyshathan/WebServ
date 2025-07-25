#include "Client.hpp"

Client::Client(std::string rawData) : httpReq(new HttpRequest(rawData)){}

Client::~Client() {
	delete httpReq;
}
