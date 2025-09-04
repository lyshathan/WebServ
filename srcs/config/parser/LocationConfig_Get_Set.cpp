#include "LocationConfig.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////////
//								GET
////////////////////////////////////////////////////////////////////////////////////

std::string	LocationConfig::getPath(void) const
{
	return (_path);
}

std::string LocationConfig::getRoot(void) const
{
	return (_root);
}

bool LocationConfig::getAutoIndex(void) const
{
	return (_autoIndex);
}

std::vector<std::string> LocationConfig::getIndex(void) const { return _indexFiles;}

bool LocationConfig::getExactMatch(void) const { return _isExactPath; }

////////////////////////////////////////////////////////////////////////////////////
//								SET
////////////////////////////////////////////////////////////////////////////////////

void	LocationConfig::setPath(std::string newPath)
{
	this->_path = newPath;
}