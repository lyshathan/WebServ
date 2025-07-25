#include "HttpRequest.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpRequest::HttpRequest(std::string data) {
	handleParsing(data);
};

HttpRequest::~HttpRequest() {};


/******************************************************************************/
/*							PARSE FUNCTIONS									  */
/******************************************************************************/

void HttpRequest::handleParsing(std::string data) {
	std::istringstream	ss(data);
	std::string	firstLine;
	_status = 0;

	// Extracts the first line out of the request string
	// Validate if the string finishes by CRLF
	// Extract the last char of the string (\r)
	std::getline(ss, firstLine);
	if (firstLine.empty() || firstLine.back() != '\r') {
		_status = BAD_REQUEST;
		return ;
	}
	else {
		firstLine.pop_back();
		parseFirstLine(firstLine);
	}
}

void HttpRequest::parseFirstLine(std::string data) {
	const int NUM_TOKENS = 3;
	std::istringstream ss(data);
	std::string firstLineTokens[NUM_TOKENS];

	if (data.empty() || data.back() == ' ') {
		_status = 400;
		return ;
	}
	for (int i = 0; ss.peek() != EOF && i < NUM_TOKENS; ++i) {
		std::getline(ss, firstLineTokens[i], ' ');
		if (firstLineTokens[i].empty()) {
			_status = BAD_REQUEST;
			return ;
		}
	}
	if (ss.peek() != EOF) {
		_status = BAD_REQUEST;
		return ;
	}

	_method = firstLineTokens[0];
	_uri = firstLineTokens[1];
	_version = firstLineTokens[2];
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