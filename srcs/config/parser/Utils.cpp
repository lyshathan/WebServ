#include "Utils.hpp"

int	IsValidDirPath(std::string &dir)
{
	struct stat sb;

	if (stat(dir.c_str(), &sb) != 0)
		return (NO_EXIST);
	if (!S_ISDIR(sb.st_mode))
		return (NOT_A_DIRECTORY);
	if (access(dir.c_str(), R_OK) != 0)
		return (NO_PERMISSION);
	return (VALID);
}

int	IsValidFile(std::string &dir)
{
	if (access(dir.c_str(), F_OK) != 0)
		return (NO_EXIST);
	if (access(dir.c_str(), R_OK) != 0)
		return (NO_PERMISSION);
	return (VALID);
}



void	ACheckForSemicolon(std::vector< t_token>::iterator &it, std::vector< t_token> &tokens)
{
	std::ostringstream				errorMsg;
	std::vector< t_token>::iterator	tmp = it;

	it++;
	if (it == tokens.end() || it->type != SEMICOLON)
	{
		errorMsg << RED << "[Config file] Missing semicolon at line : " << it->line - 1 << RESET;
		throw std::invalid_argument(errorMsg.str());
	}
}