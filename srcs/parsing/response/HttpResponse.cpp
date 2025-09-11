#include "HttpResponse.hpp"

/******************************************************************************/
/*						CONSTRUCTORS & DESTRUCTORS							  */
/******************************************************************************/

HttpResponse::HttpResponse() {};

HttpResponse::HttpResponse(HttpRequest *req) : _request(req), _isTextContent(false), _res(""), _mimeType("") {
	initStatusPhrases();
	initHtmlResponses();
};

HttpResponse::~HttpResponse() {};

/******************************************************************************/
/*							PARSE FUNCTIONS								  */
/******************************************************************************/

void HttpResponse::parseResponse() {
	int	status = _request->getStatus();

	setBody(status);
	setDefaultHeaders(status);

	if (status == OK || status == FORBIDDEN)
		addHeader("Connection: ", "keep-alive");
	else if (status == NOT_FOUND)
		addHeader("Connection: ", "close");
	else if (status == MOVED_PERMANENTLY || status == MOVED_PERMANENTLY_302) {
		addHeader("Connection: ", "keep-alive");
		addHeader("Location: ", _request->getUri());
	}
}


