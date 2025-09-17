#include "LocationConfig.hpp"
#include "../utils/Utils.hpp"

std::string	LocationConfig::getPath(void) const { return (_path); }

std::string LocationConfig::getRoot(void) const { return (_root); }

bool LocationConfig::getAutoIndex(void) const { return (_autoIndex); }

std::vector<std::string> LocationConfig::getIndex(void) const { return _indexFiles;}

std::map<int, std::string> LocationConfig::getErrorPages(void) const { return _errorPages;}

bool LocationConfig::getExactMatch(void) const { return _isExactPath; }

std::vector<std::string> LocationConfig::getAllowMethods() const { return _allowMethod; }

std::string	LocationConfig::getUploadPath() const { return _uploadPath; }

size_t	LocationConfig::getClientMaxBodySize() const { return _clientMaxBodySize; }