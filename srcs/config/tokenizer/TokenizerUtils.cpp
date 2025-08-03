#include "../parser/Config.hpp"

bool myIsSpace(unsigned char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

bool isSpecialChar(unsigned char c)
{
	return (c == '{' || c == '}' || c == ';');
}
