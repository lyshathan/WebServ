#include "Utils.hpp"

int	isValidDirPath(std::string &dir)
{
	std::string str;
	if (!dir.empty() && dir[0] == '/')
		str = '.' + dir;
	else
		str = dir;
		
	struct stat sb;
	if (stat(str.c_str(), &sb) != 0)
		return (NO_EXIST);
	if (!S_ISDIR(sb.st_mode))
		return (NOT_A_DIRECTORY);
	if (access(str.c_str(), R_OK) != 0)
		return (NO_PERMISSION);
	return (VALID);
}

int	isValidFile(std::string &dir)
{
	if (access(dir.c_str(), F_OK) != 0)
		return (NO_EXIST);
	if (access(dir.c_str(), R_OK) != 0)
		return (NO_PERMISSION);
	return (VALID);
}


void	checkForSemicolon(std::vector< t_token>::iterator &it, std::vector< t_token> &tokens)
{
	it++;
	if (it == tokens.end() || it->type != SEMICOLON)
		throwErrorToken(" Missing semicolon", *it);
}

void setString(std::string &element, std::string &value, t_token &token)
{
	if (element.empty())
		element = value;
	else
		throwErrorToken(" Directive already set", token);
}

void	addListToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it, std::vector< t_token> &tokenList, std::vector<std::string> *validValue)
{
	it++;
	while (it != tokenList.end() &&  it->type == VALUE)
	{
		if (validValue && std::find((*validValue).begin(), (*validValue).end(), it->content) == (*validValue).end())
			throwErrorToken(" Invalid value (not allowed)", *it);
		if (std::find(vec.begin(), vec.end(), it->content) != vec.end())
			throwErrorToken(" Invalid value (already existing)", *it);
		vec.push_back(it->content);
		it++;
	}
	if (it == tokenList.end() || it->type != SEMICOLON)
		throwErrorToken(" Missing semicolon", *it);
}

void	parseClientMaxBodySize(std::vector< t_token>::iterator &it, size_t &clientMaxBodySize, std::vector< t_token> &tokenList)
{
	char *end;

	if (clientMaxBodySize != 0)
		throwErrorToken(" Size already set", *it);
	size_t bodySize = std::strtod((++it)->content.c_str(), &end);
	if (bodySize <= 0 || bodySize > INT_MAX || std::isinf(bodySize))
		throwErrorToken(" Invalid size", *it);
	if ((*end && *end != 'K' && *end != 'M' && *end != 'G') || std::strlen(end) > 1)
		throwErrorToken(" Invalid size", *it);
	if (*end == 'K')
		bodySize *= 100;
	else if (*end == 'M')
		bodySize *= 100000;
	else if (*end == 'G')
		bodySize *= 1000000000;
	clientMaxBodySize = bodySize;

	checkForSemicolon(it, tokenList);
}

size_t	CountErrorCodes(std::vector<t_token> &tokenList, std::vector< t_token>::iterator it)
{
	size_t	count = 0;

	while (it != tokenList.end() && it->type == VALUE)
	{
		// std::cout << it->content << std::endl;
		it++;
		count++;
	}
	return (count);
}

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