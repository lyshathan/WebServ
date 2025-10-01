#include "HttpRequest.hpp"

int	hexToInt(const std::string &hexStr) {
	if (hexStr.empty())
		return -1;

	for (size_t i = 0; i < hexStr.length(); ++i) {
		if (!std::isxdigit(hexStr[i]))
			return -1;
	}

	int value = 0;
	std::istringstream iss(hexStr);
	if (!(iss >> std::hex >> value))
		return -1;
	return value;
}

bool HttpRequest::parseChunk(std::string body) {
	size_t pos = 0;
	while (pos < body.size()) {
		size_t chunkSizeEnd = body.find("\r\n", pos);
		if (chunkSizeEnd == std::string::npos)
			return false;

		std::string chunkSizeStr = body.substr(pos, chunkSizeEnd - pos);
		size_t semicolon = chunkSizeStr.find(';');
		if (semicolon != std::string::npos)
			chunkSizeStr = chunkSizeStr.substr(0, semicolon);

		int chunkSize = hexToInt(chunkSizeStr);
		if (chunkSize < 0)
			return false;
		if (chunkSize == 0) {
			pos = chunkSizeEnd + 2;

			while (pos < body.size()) {
				size_t trailerEnd = body.find("\r\n", pos);
				if (trailerEnd == std::string::npos)
					return false;
				if (trailerEnd == pos)
					return true;
				pos = trailerEnd + 2;
			}
			return false;
		}
		size_t chunkDataStart = chunkSizeEnd + 2;
		size_t chunkDataEnd = chunkDataStart + chunkSize;

		if (chunkDataEnd + 2 > body.size())
			return false;
		if (body.substr(chunkDataEnd, 2) != "\r\n")
			return false;
		std::string chunkData = body.substr(chunkDataStart, chunkSize);
		_body[""] += chunkData;
		pos = chunkDataEnd + 2;
	}
	return false;
}

void HttpRequest::parseOnePart(std::string part) {
	std::string str = "filename=";
	size_t pos = part.find(str);
	if (pos == std::string::npos) return;

	size_t start = pos + str.length() + 1;
	if (start >= part.length()) return;

	size_t end = part.find('"', start);
	if (end == std::string::npos) return;

	std::string filename = part.substr(start, end - start);

	size_t bodyPos = part.find("\r\n\r\n");
	std::string body;
	if (bodyPos != std::string::npos)
		body = part.substr(bodyPos + 4);
	_body[filename] = body;
}

bool HttpRequest::parseMultiPartBody(std::map<std::string, std::string>::const_iterator &it, std::string body) {
	size_t	pos = it->second.find("=");
	if (pos != std::string::npos) {
		std::string boundary = "--" + it->second.substr(pos + 1);
		size_t currentPos = body.find(boundary);
		while (currentPos != std::string::npos) {
			size_t nextBoundary = body.find(boundary, currentPos + boundary.length());
			if (nextBoundary != std::string::npos) {
				std::string part = body.substr(currentPos + boundary.length(), nextBoundary
												- currentPos - boundary.length());
				parseOnePart(part);
			}
			currentPos = nextBoundary;
		}
	}
	return true;
}
