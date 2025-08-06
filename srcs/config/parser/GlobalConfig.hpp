#ifndef GLOBALCONFIG_HPP
#define GLOBALCONFIG_HPP

#include "../General.hpp"

class GlobalConfig {
	private :
		std::string		_error_log;
		size_t			_clientMaxBodySize;
		bool			_sizeDefined;

	public :
		GlobalConfig();
		~GlobalConfig();

		void	setSizeDefined(bool status);
		void	setErrorLog(std::string &errorLog);
		void	setClientMaxBodySize(std::string &size, t_token &token);
		size_t	getClientMaxBodySize(void) const;
		void	printGlobal();
		void	Check();
};

#endif
