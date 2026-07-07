#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ParserBase final {
private:
	// Base class for parsers; common parsing utilities can be added here
public:
	ParserBase() = default;
	~ParserBase() = default;

	static const std::string Trim(const std::string& str);
};