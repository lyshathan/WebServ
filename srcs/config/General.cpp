#include "General.hpp"

void	throwErrorToken(const char* msg, t_token &token)
{
	std::ostringstream errorMsg;

	errorMsg << RED << "[Config file] " << msg << " at line : " << token.line << " \'" << token.content << "\'" << RESET <<std::endl;
	throw std::invalid_argument(errorMsg.str());
}

void	throwError(const char* msg)
{
	std::ostringstream errorMsg;

	errorMsg << RED << "[Config file] " << msg << RESET <<std::endl;
	throw std::invalid_argument(errorMsg.str());
}