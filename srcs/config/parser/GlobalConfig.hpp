#ifndef GLOBALCONFIG_HPP
#define GLOBALCONFIG_HPP

#include "../General.hpp"

class GlobalConfig {
	private :
		std::string		_error_log;
		size_t			_client_max_body_size;

	public :
		GlobalConfig();
		~GlobalConfig();

		void setErrorLog(std::string &errorLog);
		void setClientMaxBodySize(std::string &size, t_token &token);
		void printGlobal();
};

#endif
