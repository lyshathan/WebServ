#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "../utils/Utils.hpp"

class ServerConfig;

class LocationConfig {
	private :
		std::vector< LocationConfig >	&_locations;
		std::vector<t_token>			&_tokens;

		bool							_autoIndex;
		size_t							_clientMaxBodySize;
		std::string						_path;
		std::string						_uploadPath;
		std::map<std::string, std::string> _cgiData;
		std::string						_root;
		std::vector< std::string >		_indexFiles;
		std::vector< std::string >		_allowMethod;
		std::vector< std::string >		_validMethod;
		std::pair< int , std::string >	_return;
		std::map< int , std::string >	_errorPages;

		const size_t					_currentLevel;
		bool							_isExactPath;

		void	locationConfigParser(std::vector< t_token>::iterator &it);
		void	parsePath(std::vector< t_token>::iterator &it);
		void	parseReturn(std::vector< t_token>::iterator &it);
		void	parseAutoIndex(std::vector< t_token>::iterator &it);
		void	parseErrorPage(std::vector< t_token>::iterator &it);
		void	parseCGI(std::vector<t_token>::iterator &it);

	public :
		LocationConfig(ServerConfig &server, std::vector< LocationConfig > &locations, std::vector<t_token> &tokenList);
		LocationConfig(std::vector<t_token> &tokenList, std::vector< t_token>::iterator &it, std::vector< LocationConfig > &locations);
		~LocationConfig();
		LocationConfig & operator=(LocationConfig const &otherLocationConfig);
		void						printLocation(void) const;
		void						check(ServerConfig &server);

		bool						getExactMatch(void) const;
		bool						getAutoIndex(void) const;
		std::string					getPath(void) const;
		std::string					getRoot(void) const;
		std::vector<std::string>	getIndex(void) const;
		std::map<int,std::string>	getErrorPages(void) const;
		std::vector<std::string>	getAllowMethods(void) const;
		std::string					getUploadPath(void) const;
		size_t						getClientMaxBodySize() const;
		std::pair<int, std::string>	getReturn() const;
		std::string					getCGIPath() const;
		std::string					getCGIExtension() const;
		std::map<std::string, std::string> getCGIData() const;

		void						setPath(std::string newPath);
};

#endif