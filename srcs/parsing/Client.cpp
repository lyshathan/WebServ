#include "Client.hpp"
#include "response/HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const Config &config, const std::string &clientIP, size_t pollIndex) :
	_pollIndex(pollIndex), _fd(fd), _reqBuffer(), _resBuffer(), _recvSize(0), _bytesSent(0),
	_clientIP(clientIP), _state(READING_HEADERS), _cgi(NULL), _lastActivity(time(NULL)),
	httpReq(new HttpRequest(config, fd, _clientIP)), httpRes(new HttpResponse(httpReq, this)) {

	}

Client::~Client() {
	if (httpReq)
		delete httpReq;
	if (httpRes)
		delete httpRes;
	if (_cgi)
		delete _cgi;
}

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

bool	Client::hasTimedOut(time_t now) {
	time_t elapse = now - _lastActivity;

	switch (_state) {
		case(READING_HEADERS): return (elapse > HEADER_TIMEOUT);
		case(READING_BODY): return (elapse > BODY_TIMEOUT);
		case(CGI_PROCESSING): return (elapse > CGI_TIMEOUT);
		case(SENDING_RESPONSE): return (elapse > WRITE_TIMEOUT);
		default : return (false);
	}
}

bool Client::isReqComplete() {
	std::map<std::string, std::string> headers = httpReq->getHeaders();

	size_t headerEnd = _reqBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;

	std::map<std::string, std::string>::const_iterator it = headers.find("transfer-encoding");
	if (it != headers.end() && it->second.find("chunked") != std::string::npos) {
		size_t zeroChunkPos = _reqBuffer.find("0\r\n");
		if (zeroChunkPos != std::string::npos) {
			size_t finalEnd = _reqBuffer.find("\r\n\r\n", zeroChunkPos);
			return finalEnd != std::string::npos;
		}
		return false;
	}

	size_t maxBodySize = httpReq->getMaxBody();
	it = headers.find("content-length");
	if (it != headers.end()) {
		std::istringstream iss(it->second);
		size_t contentLength = 0;
		iss >> contentLength;
		size_t bodyReceived = _reqBuffer.length() - (headerEnd + 4);
		if (contentLength > maxBodySize)
			httpReq->setStatus(413);
		if (bodyReceived > contentLength)
			_reqBuffer.resize(headerEnd + 4 + contentLength);
		return bodyReceived >= contentLength;
	}
	return true;
}

void		Client::cgiInitEnv() {
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_SOFTWARE"] = "webserv/1.0";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REQUEST_METHOD"] = httpReq->getMethod();
	_env["SCRIPT_NAME"] = httpReq->getUri();
	_env["PATH_INFO"] = httpReq->getUri();
	_env["REMOTE_ADDR"] = getClientIp();
	_env["REMOTE_HOST"] = getClientIp();

	const std::vector<std::string> serverNames = httpReq->getServerName();
	if (!serverNames.empty())
		_env["SERVER_NAME"] = serverNames[0];
	else
		_env["SERVER_NAME"] = "localhost";

	const std::vector<int> listenPort = httpReq->getListenPort();
	std::stringstream ss;
	ss << listenPort[0];
	_env["SERVER_PORT"] = ss.str();

	std::string queries = httpReq->getQueries();
	if (!queries.empty())
		_env["QUERY_STRING"] = queries;
	else
		_env["QUERY_STRING"] = "";

	std::map<std::string, std::string> body = httpReq->getBody();
	if (!body.empty()) {
		std::stringstream contentLengthSs;
		size_t totalLength = 0;
		for (std::map<std::string, std::string>::const_iterator bodyIt = body.begin(); bodyIt != body.end(); ++bodyIt) {
			totalLength += bodyIt->second.length();
		}
		contentLengthSs << totalLength;
		_env["CONTENT_LENGTH"] = contentLengthSs.str();
	}
	else {
		_env["CONTENT_LENGTH"] = "0";
	}

	std::map<std::string, std::string> headers = httpReq->getHeaders();
	std::map<std::string, std::string>::iterator it = headers.find("content-type");
	if (it != headers.end())
		_env["CONTENT_TYPE"] = it->second;
}

void	Client::launchCGI() {
	_cgi = new CgiHandler(this);
	_cgi->cgiStart();
}