#include "HttpRequest.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpRequest::HttpRequest(const Config& config, int &fd) : _config(config), _location(NULL), _status(0),
_clientfd(fd), _areHeadersParsed(false) {};

HttpRequest::~HttpRequest() {};

/******************************************************************************/
/*							PARSE FUNCTIONS									  */
/******************************************************************************/

void HttpRequest::requestHeaderParser(std::string data) {
	std::string	firstLine;
	std::string	headers;

	if (!extractUntil(firstLine, data, "\r\n") || !parseFirstLine(firstLine)) {
		_status = BAD_REQUEST;
		return ;
	}
	if (!extractUntil(headers, data, "\r\n\r\n") || !parseHeaders(headers)) {
		_status = BAD_REQUEST;
		return;
	}
	if (!validateUri()) {
		_status = BAD_REQUEST;
		return;
	}
	_areHeadersParsed = true;
}

void HttpRequest::requestBodyParser(std::string data) {
	if (!data.empty() && !parseBody(data))
		_status = BAD_REQUEST;
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

bool HttpRequest::validateUri() {
	if (_uri[0] != '/') return false;
	if (_uri.find("..") != std::string::npos) return false;
	if (_uri.find('\0') != std::string::npos) return false;
	for (size_t i = 0; i < _uri.length(); ++i) {
		if (!std::isalnum(_uri[i])) {
			if (_uri[i] == '/' || _uri[i] == '.' || _uri[i] == '-' || _uri[i] == '_')
				continue;
			return false;
		}
	}
	return true;
}

bool HttpRequest::parseBody(std::string data) {
	size_t headerEnd = data.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;
	_body = data.substr(headerEnd + 4);

	// std::string fullPath = "./uploaded_body.png";

	// // 3. Open file in binary mode
	// std::ofstream file(fullPath, std::ios::binary);
	// if (!file.is_open()) {
	// 	std::cout << "Error opening the file\n";
	// 	return false; // Error opening file
	// }
	// file.write(body.data(), body.size());
	// if (file.fail()) {
	// 	return false;
	// }

	// file.close();
	return true;
}
