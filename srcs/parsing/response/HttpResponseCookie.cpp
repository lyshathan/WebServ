#include "HttpResponse.hpp"

/******************************************************************************/
/*								COOKIE HANDLER								  */
/******************************************************************************/

void cleanupOldSessions() {
	std::map<std::string, UserData>& sessions = HttpResponse::getSessions();
	if (sessions.size() > 100) {
		sessions.clear();
	}
}

std::string extractSessionId(const std::string& cookieHeader) {
	size_t pos = cookieHeader.find("session=");
	if (pos != std::string::npos) {
		size_t start = pos + 8;
		size_t end = cookieHeader.find(";", start);
		if (end == std::string::npos) end = cookieHeader.length();
		return cookieHeader.substr(start, end - start);
	}
	return "";
}

std::string	HttpResponse::generateSession() {
	std::ostringstream sessionName;
	sessionName << "id_" << std::time(NULL);
	return sessionName.str();
}

std::string	HttpResponse::buildSimpleHTML(const UserData& data) {
	std::string html = "<!DOCTYPE html>\n";
	html += "<html>\n";
	html += "<head>\n";
	html += "    <title>User Profile</title>\n";
	html += "    <style>\n";
	html += "        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
	html += "        .container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; }\n";
	html += "        h1 { color: #333; }\n";
	html += "        .info { margin: 20px 0; }\n";
	html += "        .label { font-weight: bold; color: #666; }\n";
	html += "        .value { color: #333; margin-left: 10px; }\n";
	html += "    </style>\n";
	html += "</head>\n";
	html += "<body>\n";
	html += "    <div class='container'>\n";
	html += "        <h1>Welcome, " + data.username + "!</h1>\n";
	html += "        <div class='info'>\n";
	html += "            <span class='label'>Name:</span>\n";
	html += "            <span class='value'>" + data.username + "</span>\n";
	html += "        </div>\n";
	html += "        <div class='info'>\n";
	html += "            <span class='label'>Age:</span>\n";
	html += "            <span class='value'>" + data.age + "</span>\n";
	html += "        </div>\n";
	html += "        <div class='info'>\n";
	html += "            <span class='label'>City:</span>\n";
	html += "            <span class='value'>" + data.city + "</span>\n";
	html += "        </div>\n";
	html += "        <p><a href='/'>Back to Home</a></p>\n";
	html += "    </div>\n";
	html += "</body>\n";
	html += "</html>\n";
	return html;
}

bool	HttpResponse::handleCookie(int status) {
	std::string uri = _request->getUri();
	if (uri.find("/cookie") == std::string::npos)
		return false;

	cleanupOldSessions();

	std::map<std::string, std::string>::iterator it = _request->getHeaders().find("cookie");
	if (status == 200)
		return handleCookieGet(it);
	else
		return handleCookiePost(it);
}

bool	HttpResponse::handleCookieGet(std::map<std::string, std::string>::iterator& cookieIt) {
	if (cookieIt == _request->getHeaders().end()) {
		std::string session = "session=" + generateSession();
		addHeader("Set-Cookie: ", session);
		return false;
	}
	std::string sessionId = extractSessionId(cookieIt->second);
	std::map<std::string, UserData>::iterator sessionIt = _sessions.find(sessionId);

	if (sessionIt != _sessions.end()) {
		std::string html = buildSimpleHTML(sessionIt->second);
		addHeader("Content-Type:", "text/html");
		_res = html;
		_isTextContent = true;
		return true;
	}
	return false;
}

bool	HttpResponse::handleCookiePost(std::map<std::string, std::string>::iterator& cookieIt) {
	std::string sessionId;

	if (cookieIt != _request->getHeaders().end()) {
		sessionId = extractSessionId(cookieIt->second);
	} else {
		sessionId = generateSession();
		std::string cookie = "session=" + sessionId;
		addHeader("Set-Cookie: ", cookie);
	}

	std::map<std::string, std::string> body = _request->getBody();
	UserData userData;

	std::map<std::string, std::string>::iterator bodyIt = body.find("name");
	if (bodyIt != body.end())
		userData.username = bodyIt->second;
	bodyIt = body.find("city");
	if (bodyIt != body.end())
		userData.city = bodyIt->second;
	bodyIt = body.find("age");
	if (bodyIt != body.end())
		userData.age = bodyIt->second;

	_sessions[sessionId] = userData;
	setStatusLine(302);
	addHeader("Location: ", "/cookie/");
	_isTextContent = false;
	return true;
}