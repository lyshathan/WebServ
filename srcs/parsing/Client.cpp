#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const Config &config, const std::string &clientIP, size_t pollIndex) :
	_pollIndex(pollIndex), _fd(fd), _reqBuffer(), _resBuffer(), _recvSize(0), _bytesSent(0),
	_clientIP(clientIP), _state(READING_HEADERS), _cgi(NULL), _lastActivity(0),
	httpReq(new HttpRequest(config, fd, _clientIP)), httpRes(new HttpResponse(httpReq)) {}

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

const std::string &Client::getRes() const { return _reqBuffer;}

int	Client::getFd() const { return _fd; }

size_t Client::getPollIndex() { return _pollIndex; }

void Client::setState(ClientState state) { _state = state; }

std::string	Client::getClientIp() const { return _clientIP; }

CgiHandler	*Client::getCgi() const { return _cgi; }

void	Client::launchCGI() {
	_cgi = new CgiHandler(this);
	_cgi->cgiStart();
}

bool	Client::hasTimedOut(time_t now) {
	if (_state == CGI_PROCESSING)
        return (now - _lastActivity > CGI_TIMEOUT);
    return false;
}

void	Client::setCgi() {
	_cgi = NULL;
}