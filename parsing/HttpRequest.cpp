#include "HttpRequest.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpRequest::HttpRequest(std::string data) : _status(0) {
	handleParsing(data);
};

HttpRequest::~HttpRequest() {};


/******************************************************************************/
/*							PARSE FUNCTIONS									  */
/******************************************************************************/

void HttpRequest::errorHandler(int status) {
	if (status == BAD_REQUEST) {
		_status = BAD_REQUEST;
		std::cout << "400 Bad request\n";
	}
}

bool HttpRequest::extractUntil(std::string &line,
	std::string &data, const std::string &del) {
	size_t pos = data.find(del);
	if (pos == std::string::npos)
		return false;
	line = data.substr(0, pos);
	data = data.substr(pos + del.length());
	return true;
}

void HttpRequest::handleParsing(std::string data) {
	std::string	firstLine;
	std::string	headers;

	if (!extractUntil(firstLine, data, "\r\n") || !parseFirstLine(firstLine))
		return errorHandler(BAD_REQUEST);
	if (!extractUntil(headers, data, "\r\n\r\n") || !parseHeaders(headers))
		return errorHandler(BAD_REQUEST);
	if (!data.empty())
		if (!parseBody(data))
			return errorHandler(BAD_REQUEST);
}

bool HttpRequest::parseBody(std::string data) {
	return true;
}

bool HttpRequest::parseHeaders(std::string data) {
	return true;
}

bool HttpRequest::parseFirstLine(std::string data) {
	const int NUM_TOKENS = 3;
	std::istringstream ss(data);
	std::string firstLineTokens[NUM_TOKENS];
	int i = 0;

	if (data.empty() || data.back() == ' ')
		return false;
	for (; ss.peek() != EOF && i < NUM_TOKENS; ++i) {
		std::getline(ss, firstLineTokens[i], ' ');
		if (firstLineTokens[i].empty())
			return false;
	}
	if (ss.peek() != EOF || i < NUM_TOKENS)
		return false;

	_method = firstLineTokens[0];
	_uri = firstLineTokens[1];
	_version = firstLineTokens[2];
	return true;
}

// "GET /index.html HTTP/1.1\r\nHost:localhost:8080\r\nUser-Agent:Mozilla/5.0\r\n\r\n"

// Request-Line = GET /index.html HTTP/1.1\r\n

// Headers = Host:localhost:8080\r\nUser-Agent:Mozilla/5.0

// {
// 	method:"GET"
// 	path:"/index.html"
// 	version:"HTTP/1.1"
// 	headers: {
// 		"Host":"localhost:8080",
// 		"User-Agent":"Mozilla/5.0"
// 	}
// 	body:NULL
// 	valid:true
// }