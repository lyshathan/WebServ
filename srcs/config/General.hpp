#ifndef General_HPP
#define General_HPP

#include "tokenizer/Token.hpp"

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

#define RED		"\033[38;2;255;0;0m"
#define BLUE	"\033[38;2;0;0;255m"
#define YELLOW	"\033[38;2;255;255;0m"
#define GREEN	"\033[38;2;0;255;0m"
#define PURPLE	"\033[38;2;128;0;128m"
#define RESET	"\033[0m"


void	ThrowError(const char* msg, t_token &token);


#endif