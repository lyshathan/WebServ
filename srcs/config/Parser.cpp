#include "Config.hpp"

void	Config::Parser()
{
	for (std::vector< t_token>::iterator it = _tokens.begin() ; it != _tokens.end() ; it++)
	{
		if (it->level == GLOBAL && it->type == DIRECTIVE)
		{
			GlobalConfig global(it);
			_globalConfig = global;
		}

	}
}

