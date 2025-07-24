#include "Client.hpp"

Client::Client(std::string _rawData) : _rawMsg(_rawMsg) {}

Client::~Client() {}

std::string const Client::getRawMsg() const {
	return this->_rawMsg;
}