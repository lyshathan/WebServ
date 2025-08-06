#include "General.hpp"

void	ThrowErrorToken(const char* msg, t_token &token)
{
	std::ostringstream errorMsg;

	errorMsg << RED << "[Config file] " << msg << " at line : " << token.line << RESET <<std::endl;
	throw std::invalid_argument(errorMsg.str());
}

void	ThrowError(const char* msg)
{
	std::ostringstream errorMsg;

	errorMsg << RED << "[Config file] " << msg << RESET <<std::endl;
	throw std::invalid_argument(errorMsg.str());
}