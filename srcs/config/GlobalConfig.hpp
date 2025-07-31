#ifndef GLOBALCONFIG_HPP
#define GLOBALCONFIG_HPP

#include "ConfigIncludes.hpp"

class GlobalConfig {
	private :
		std::string		error_log;
		size_t			client_max_body_size;

	public :
		GlobalConfig();
		~GlobalConfig();
};

#endif