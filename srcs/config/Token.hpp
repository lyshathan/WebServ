#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "ConfigIncludes.hpp"

typedef enum	e_type {
	DIRECTIVE,
	VALUE,
	OPEN_BRACE,
	CLOSE_BRACE,
	SEMICOLON,
	COMMENT,
	NEWLINE
}				t_type;


typedef struct	s_token {
	std::string	content;
	t_type		type;
	size_t		line;

}				t_token;

#endif