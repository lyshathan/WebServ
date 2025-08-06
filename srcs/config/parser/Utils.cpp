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
	std::vector< t_token>::iterator	tmp = it;

	it++;
	if (it == tokens.end() || it->type != SEMICOLON)
		ThrowErrorToken(" Missing semicolon", *it);
}

void setString(std::string &element, std::string &value, t_token &token)
{
	if (element.empty())
		element = value;
	else
		ThrowErrorToken(" Directive already set", token);
}

void	AddListToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it, std::vector< t_token> &tokenList, std::vector<std::string> *validValue)
{
	std::vector< t_token>::iterator	tmp = it;

	it++;
	while (it != tokenList.end() &&  it->type == VALUE)
	{
		if (validValue && std::find((*validValue).begin(), (*validValue).end(), it->content) == (*validValue).end())
			ThrowErrorToken(" Invalid value (not allowed)", *it);
		if (std::find(vec.begin(), vec.end(), it->content) != vec.end())
			ThrowErrorToken(" Invalid value (already existing)", *it);
		vec.push_back(it->content);
		it++;
	}
	if (it == tokenList.end() || it->type != SEMICOLON)
		ThrowErrorToken(" Missing semicolon", *it);
}

void	ParseClientMaxBodySize(std::vector< t_token>::iterator &it, size_t &clientMaxBodySize, std::vector< t_token> &tokenList)
{
	char *end;

	if (clientMaxBodySize != 0)
		ThrowErrorToken(" Size already set", *it);
	size_t bodySize = std::strtod((++it)->content.c_str(), &end);
	if (bodySize <= 0 || bodySize > INT_MAX || std::isinf(bodySize))
		ThrowErrorToken(" Invalid size", *it);
	if ((*end && *end != 'K' && *end != 'M' && *end != 'G') || std::strlen(end) > 1)
		ThrowErrorToken(" Invalid size", *it);
	if (*end == 'K')
		bodySize *= 100;
	else if (*end == 'M')
		bodySize *= 100000;
	else if (*end == 'G')
		bodySize *= 1000000000;
	clientMaxBodySize = bodySize;

	ACheckForSemicolon(it, tokenList);
}