#include "HttpResponse.hpp"

/******************************************************************************/
/*								SETTERS										  */
/******************************************************************************/

void HttpResponse::setTextContent() {
	std::string uri = _request->getUri();
	std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return ;

	_res.clear();
	char buffer[4096];
	std::string chunk;
	while (file.read(buffer, sizeof(buffer))) {
		chunk.assign(buffer, file.gcount());
		_res += chunk;
	}
	if (file.gcount() > 0) {
		chunk.assign(buffer, file.gcount());
		_res += chunk;
	}
	file.close();
}

void	HttpResponse::setBinContent() {
	std::string uri = _request->getUri();
	std::fstream file(uri.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return ;

	char buffer[4096];
	while (file.read(buffer, sizeof(buffer))) {
		_binRes.insert(_binRes.end(), buffer, buffer + file.gcount());
	}
	if (file.gcount() > 0) {
		_binRes.insert(_binRes.end(), buffer, buffer + file.gcount());
	}
}
