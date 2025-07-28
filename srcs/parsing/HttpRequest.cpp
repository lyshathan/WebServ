#include "HttpRequest.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpRequest::HttpRequest() : _status(0) {};

HttpRequest::~HttpRequest() {};

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
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

std::string HttpRequest::trim(const std::string &str) {
	const std::string WHITESPACE = " \t\n\r\f\v";

	size_t pos = str.find_first_not_of(WHITESPACE);
	if (pos == std::string::npos)
		return "";
	size_t last_pos = str.find_last_not_of(WHITESPACE);
	return str.substr(pos, last_pos - pos + 1);
}

bool HttpRequest::isValidTchar(char c) {
	if (std::isalnum(c))
		return true;
	const std::string validSpecialChars = "!#$%&'*+-.^_`|~";
	return validSpecialChars.find(c) != std::string::npos;
}

bool HttpRequest::mapHeaders(std::string &header) {
	std::string key;
	std::string value;

	size_t pos = header.find(':');
	if (pos == std::string::npos)
		return false;
	key = header.substr(0, pos);
	if (key.empty())
		return false;
	value = header.substr(pos + 1);
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	for (size_t i = 0; i < key.length(); ++i) {
		if (!isValidTchar(key[i]))
			return false;
	}
	if (_headers.count(key) > 0) {
		if (key == "host")
			return false;
		_headers[key] += ", " + trim(value);
	} else {
		_headers[key] = trim(value);
	}
	return true;
}

bool HttpRequest::validateVersion(std::string str){
	const std::string PREFIX = "HTTP/";
	const std::string VALID_VERSIONS[] = {"1.0", "1.1"};

	if (str.length() < PREFIX.length() + 3)
		return false;
	if (str.substr(0, PREFIX.length()) != PREFIX)
		return false;
	std::string version = str.substr(PREFIX.length());
	return (version == VALID_VERSIONS[0] || version == VALID_VERSIONS[1]);
}

/******************************************************************************/
/*							PARSE FUNCTIONS									  */
/******************************************************************************/

bool HttpRequest::processRequest() {

	DIR *dir = opendir("../../");
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {

	}

	std::cout << access(_uri.c_str(), R_OK) << "\n";
	return true;
}

void HttpRequest::handleRequest(std::string data) {
	std::string	firstLine;
	std::string	headers;

	std::cout << "---- DATA STRUCTURE FROM HTTP REQUEST ----\n";
	if (!extractUntil(firstLine, data, "\r\n") || !parseFirstLine(firstLine))
		return errorHandler(BAD_REQUEST);
	if (!extractUntil(headers, data, "\r\n\r\n") || !parseHeaders(headers))
		return errorHandler(BAD_REQUEST);
	if (!data.empty() && !parseBody(data))
		return errorHandler(BAD_REQUEST);
	if (!processRequest())
		return errorHandler(BAD_REQUEST);

	// std::map<std::string, std::string>::iterator it = _headers.begin();

	// for (; it != _headers.end(); ++it) {
	// 	std::cout << it->first << " : " << it->second << "\n";
	// }

	// std::cout << _method
	// << "\n" << _uri
	// << "\n" << _version << "\n";
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