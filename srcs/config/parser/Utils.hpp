#ifndef UTILS_HPP
#define UTILS_HPP

#include "../General.hpp"

typedef enum	e_check {
	NO_EXIST,
	NO_PERMISSION,
	NOT_A_DIRECTORY,
	VALID
}				t_check;

int		IsValidDirPath(std::string &dir);
int		IsValidFile(std::string &dir);
// void	ThrowErrorToken(const char* msg, t_token &token);
void	ACheckForSemicolon(std::vector< t_token>::iterator &it, std::vector< t_token> &tokens);
void	setString(std::string &element, std::string &value, t_token &token);
void	AddListToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it, std::vector< t_token> &tokenList, std::vector<std::string> *validValue);
void	ParseClientMaxBodySize(std::vector< t_token>::iterator &it, size_t &clientMaxBodySize, std::vector< t_token> &tokenList);

#endif