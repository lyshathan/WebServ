#include "HttpRequest.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpRequest::HttpRequest(const Config& config) : _config(config), _location(NULL), _status(0) {};

HttpRequest::~HttpRequest() {};

/******************************************************************************/
/*							PARSE FUNCTIONS									  */
/******************************************************************************/

void HttpRequest::handleRequest(std::string data) {
	std::string	firstLine;
	std::string	headers;

	//std::cout << "Body size " <<_config->getClientMaxBodySize() << std::endl;

	if (!extractUntil(firstLine, data, "\r\n") || !parseFirstLine(firstLine))
		return errorHandler(BAD_REQUEST);
	if (!extractUntil(headers, data, "\r\n\r\n") || !parseHeaders(headers))
		return errorHandler(BAD_REQUEST);
	if (!data.empty() && !parseBody(data))
		return errorHandler(BAD_REQUEST);
	if (!validateUri())
		return errorHandler(BAD_REQUEST);
	if (!validatePath())
		return errorHandler(NOT_FOUND);
	else
		_status = OK;
}

bool HttpRequest::parseFirstLine(std::string data) {
	const int NUM_TOKENS = 3;
	std::istringstream ss(data);
	std::string firstLineTokens[NUM_TOKENS];
	int i = 0;

	if (data.empty() || data[data.size() - 1] == ' ')
		return false;
	for (; !ss.eof() && i < NUM_TOKENS; ++i) {
		std::getline(ss, firstLineTokens[i], ' ');
		if (firstLineTokens[i].empty())
			return false;
	}
	if (!ss.eof() || i < NUM_TOKENS)
		return false;
	if (firstLineTokens[0] != "GET" && firstLineTokens[0] != "POST"
		&& firstLineTokens[0] != "DELETE")
		return false;
	if (firstLineTokens[1].size() > 8000
		|| !validateVersion(firstLineTokens[2]))
		return false;
	_method = firstLineTokens[0];
	_uri = firstLineTokens[1];
	_version = firstLineTokens[2];
	return true;
}

bool HttpRequest::parseHeaders(std::string data) {
	std::string	header;

	while (extractUntil(header, data, "\r\n"))
		if (!mapHeaders(header))
			return false;
	if (data.find('\r') != std::string::npos
		|| data.find('\n') != std::string::npos)
		return false;
	if (!data.empty() && !mapHeaders(data))
		return false;
	if (_headers.count("host") == 0)
		return false;
	return true;
}


bool HttpRequest::parseBody(std::string data) {
	(void)data;
	return true;
}
