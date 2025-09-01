#ifndef UTILS_HPP
#define UTILS_HPP

#include "../General.hpp"

typedef enum	e_check {
	NO_EXIST,
	NO_PERMISSION,
	NOT_A_DIRECTORY,
	VALID
}				t_check;

int		isValidDirPath(std::string &dir);
int		isValidFile(std::string &dir);
void	checkForSemicolon(std::vector< t_token>::iterator &it, std::vector< t_token> &tokens);
void	setString(std::string &element, std::string &value, t_token &token);
void	addListToVector(std::vector< std::string > &vec, std::vector< t_token>::iterator &it, std::vector< t_token> &tokenList, std::vector<std::string> *validValue);
void	parseClientMaxBodySize(std::vector< t_token>::iterator &it, size_t &clientMaxBodySize, std::vector< t_token> &tokenList);

#endif