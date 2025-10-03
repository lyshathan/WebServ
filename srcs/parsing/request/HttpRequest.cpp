#include "HttpRequest.hpp"
#include "../../ProjectTools.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpRequest::HttpRequest(const Config& config, int &fd) : _config(config), _location(NULL), _status(0),
_clientfd(fd), _areHeadersParsed(false), _isProccessingError(false), _isCGI(false) {};

HttpRequest::~HttpRequest() {};

/******************************************************************************/
/*							PARSE FUNCTIONS									  */
/******************************************************************************/

void HttpRequest::requestHeaderParser(std::string data) {
	// std::cout << "[Data..]\n\n" << data << "\n";
	std::string	firstLine;
	std::string	headers;

	size_t headerEnd = data.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return ;

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
	pickServerConfig();
	pickLocationConfig();
	_areHeadersParsed = true;
}

void HttpRequest::requestBodyParser(std::string data) {
	if (!data.empty() && !parseBody(data))
		_status = BAD_REQUEST;
}

void HttpRequest::parseQueries() {
	size_t pos = _uri.find("?");
	if (pos == std::string::npos) return;

	std::string base = _uri.substr(0, pos);
	std::string queries = _uri.substr(pos + 1);
	_uri = base;
	_queries = queries;
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
	if (firstLineTokens[1].size() > 8000
		|| !validateVersion(firstLineTokens[2]))
		return false;
	_method = firstLineTokens[0];
	_uri = firstLineTokens[1];
	_version = firstLineTokens[2];
	parseQueries();
	printLog(PURPLE, "INFO", "Request received: " + _method + " " + _uri + " " + _version);
	if (_status)
		return false;
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
	size_t headerEnd = data.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;
	std::string body = data.substr(headerEnd + 4);

	std::map<std::string, std::string>::const_iterator transferEncoding = _headers.find("transfer-encoding");
	if (transferEncoding != _headers.end()) {
		if (transferEncoding->second.find("chunked") != std::string::npos)
			return parseChunk(body);
		_status = 501;
		return false;
	}

	std::map<std::string, std::string>::const_iterator contentType = _headers.find("content-type");
	if (contentType != _headers.end()) {
		if (contentType->second.find("multipart") != std::string::npos) {
			parseMultiPartBody(contentType, body);
		} else {
			_body[""] = body;
		}
	} else {
		_body[""] = body;
	}
	return true;
}

