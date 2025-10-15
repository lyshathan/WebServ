#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const Config &config, const std::string &clientIP, size_t pollIndex) :
	_pollIndex(pollIndex), _fd(fd), _reqBuffer(), _resBuffer(), _recvSize(0), _bytesSent(0),
	_clientIP(clientIP), _state(READING_HEADERS), _cgi(NULL), _lastActivity(time(NULL)),
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

ClientState Client::getClientState() const {return (_state); }

std::string	Client::getClientIp() const { return _clientIP; }

CgiHandler	*Client::getCgi() const { return _cgi; }

void	Client::launchCGI() {
	_cgi = new CgiHandler(this);
	_cgi->cgiStart();
}

bool	Client::hasTimedOut(time_t now) {
	time_t elapse = now - _lastActivity;

	switch (_state)
	{
		case(READING_HEADERS): return (elapse > HEADER_TIMEOUT);
		case(READING_BODY): return (elapse > BODY_TIMEOUT);
		case(CGI_PROCESSING): return (elapse > CGI_TIMEOUT);
		case(SENDING_RESPONSE): return (elapse > WRITE_TIMEOUT);
		default : return (false);
	}
}

void	Client::setCgi() {
	_cgi = NULL;
}