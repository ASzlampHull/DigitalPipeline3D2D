#include "ParserBase.h"

std::string const ParserBase::Trim(const std::string& str)
{
	if (str.empty()) {
		return str;
	}

	const size_t start = str.find_first_not_of(" \t\r\n");
	const size_t end = str.find_last_not_of(" \t\r\n");
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

