#include "General.hpp"

void	ThrowError(const char* msg, t_token &token)
{
	std::ostringstream errorMsg;

	errorMsg << RED << "[Config file] " << msg << " at line : " << token.line << RESET <<std::endl;
	throw std::invalid_argument(errorMsg.str());
}