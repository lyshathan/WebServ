#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>

#define RED		"\033[38;2;255;0;0m"
#define BLUE	"\033[38;2;0;0;255m"
#define YELLOW	"\033[38;2;255;255;0m"
#define GREEN	"\033[38;2;0;255;0m"
#define PURPLE	"\033[38;2;128;0;128m"
#define RESET	"\033[0m"

uint32_t	fromIPToIntHost(const std::string &IPstr);
uint32_t	fromIPToIntNetwork(const std::string &IPstr);
std::string	getCurrentTimeLocal();
void		printLog(std::string, std::string, std::string);