#ifndef GLOBALCONFIG_HPP
#define GLOBALCONFIG_HPP

#include "ConfigIncludes.hpp"
#include "Token.hpp"

class GlobalConfig {
	private :
		std::string		_error_log;
		size_t			_client_max_body_size;

	public :
		GlobalConfig();
		~GlobalConfig();

		void setErrorLog(std::string &errorLog);
		void setClientMaxBodySize(std::string &size);
		void printGlobal();
};

#endif
