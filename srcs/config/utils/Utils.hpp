#ifndef UTILS_HPP
#define UTILS_HPP

#include "../tokenizer/Token.hpp"

#include <stdint.h> 
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <limits.h>
#include <cmath>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

#define RED		"\033[38;2;255;0;0m"
#define BLUE	"\033[38;2;0;0;255m"
#define YELLOW	"\033[38;2;255;255;0m"
#define GREEN	"\033[38;2;0;255;0m"
#define PURPLE	"\033[38;2;128;0;128m"
#define RESET	"\033[0m"

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
size_t	CountErrorCodes(std::vector<t_token> &tokenList, std::vector< t_token>::iterator it);
void	throwErrorToken(const char* msg, t_token &token);
void	throwError(const char* msg);
#endif