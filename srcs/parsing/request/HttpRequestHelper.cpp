#include "HttpRequest.hpp"

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

void HttpRequest::cleanReqInfo() {
	_method.clear();
	_uri.clear();
	_version.clear();
	_headers.clear();
	_status = 0;
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