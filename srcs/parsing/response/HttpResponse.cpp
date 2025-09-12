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

	setStatusLine(status);
	setBody(status);
	setContentHeaders();
	setStatusSpecificHeaders(status);
}