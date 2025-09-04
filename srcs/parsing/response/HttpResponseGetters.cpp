#include "HttpResponse.hpp"

/******************************************************************************/
/*								GETTERS										  */
/******************************************************************************/

const std::string& HttpResponse::getRes() const {return _res;}
const std::string& HttpResponse::getResHeaders() const {return _headers;}
const std::vector<char>& HttpResponse::getBinRes() const {return _binRes;}
bool	HttpResponse::getIsTextContent() const {return _isTextContent;}
