#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "../utils/Utils.hpp"
#include <string>

typedef enum	e_level {
	GLOBAL,
	SERVICE,
	LOCATION
}				t_level;

typedef enum	e_quote {
	OUT,
	SINGLE,
	DOUBLE
}				t_quote;

typedef enum	e_type {
	DIRECTIVE,
	VALUE,
	OPEN_BRACE,
	CLOSE_BRACE,
	SEMICOLON,
	SEMICOLON_OR_VALUE,
	PATH,
	EQUAL,
	UNDEFINED
}				t_type;

typedef struct	s_token {
	std::string	content;
	t_type		type;
	size_t		line;
	size_t		level;

}				t_token;

#endif
