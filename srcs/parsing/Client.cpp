#include "Client.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

Client::Client(int fd, const Config &config, const std::string &clientIP) :
	 _fd(fd), _recvSize(0), _clientIP(clientIP), httpReq(new HttpRequest(config, fd, _clientIP)),
	httpRes(new HttpResponse(httpReq)), _state(READING_HEADERS) {}

Client::~Client() {
	delete httpReq;
	delete httpRes;
}

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

const std::string &Client::getRes() const { return _reqBuffer;}

int		Client::readRequest() {
	char	buffer[BUFSIZ];
	ssize_t	bytesRead;

	bytesRead = recv(_fd, buffer, BUFSIZ, 0);
	if (bytesRead <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return READ_INCOMPLETE; // No data yet, try again later
		return READ_ERROR;
	}

	appendBuffer(buffer, bytesRead); // keeps appending the buffer

	if (_state == READING_HEADERS) {
		if (!httpReq->getHeadersParsed()) {
			int status = httpReq->requestHeaderParser(_reqBuffer);
			if (status == 1)
				return READ_INCOMPLETE; // there's still some headers to be parsed
			else if (status == -1)
				return READ_COMPLETE; // there was an error so we need to stop parsing and send response back to client
			else {
				// check if request has body [TO DO]
				_state = READING_BODY; // headers parse finished
			}
		}
	} if (_state == READING_BODY) {
		if (isReqComplete()) {
			httpReq->requestBodyParser(_reqBuffer); // when body is fully received it should be parsed
			_state = REQUEST_READY;
			return READ_COMPLETE;
		}
		return READ_INCOMPLETE;
	}
	return READ_INCOMPLETE;
}

void	Client::clearBuffer() {
	_reqBuffer = "";
	_recvSize = 0;
	_clientIP = "";
	httpReq->setHeadersParsed();
	httpReq->cleanReqInfo();
}

bool	Client::appendBuffer(const char *data, size_t size) {
	_reqBuffer.insert(_reqBuffer.end(), data, data + size);
	_recvSize += size;
	return true;
}

bool Client::isReqComplete() const {
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
		return bodyReceived >= contentLength;
	}
	return true;
}