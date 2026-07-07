#include "INIParser.h"

void INIParser::ParseINIFile()
{
    std::ifstream file(filename);
    if (!file.is_open()) { throw std::runtime_error("Could not open config file: " + filename); }
    std::string line, currentSection;

    while (std::getline(file, line)) {
        line = ParserBase::Trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        // Section header
        if (line.front() == '[' && line.back() == ']') {
            currentSection = ParserBase::Trim(line.substr(1, line.size() - 2));
            continue;
        }

        // Key-value pair
        size_t equals = line.find('=');
        if (equals != std::string::npos) {
            const std::string key = ParserBase::Trim(line.substr(0, equals));
            const std::string value = ParserBase::Trim(line.substr(equals + 1));
            iniData[currentSection][key] = value;
        }
    }
}

glm::vec3 INIParser::GetVec3Data(const std::string& section, const std::string& key)
{
    const std::string value = GetData(section, key);
	glm::vec3 vec;

    const size_t firstComma = value.find(',');
    const size_t secondComma = value.find(',', firstComma + 1);
	if (firstComma != std::string::npos && secondComma != std::string::npos) {
		try {
			vec.x = std::stof(ParserBase::Trim(value.substr(0, firstComma)));
			vec.y = std::stof(ParserBase::Trim(value.substr(firstComma + 1, secondComma - firstComma - 1)));
			vec.z = std::stof(ParserBase::Trim(value.substr(secondComma + 1)));
			return vec;
		}
		catch (const std::invalid_argument& e) {
			throw std::runtime_error("Invalid vector format for key: " + key + " in section: " + section);
		}
	}

    return vec;
}

 void INIParser::GetKeys(const std::string& section, std::vector<std::string>& keys_)
{
    std::vector<std::string> keys;
    const auto it = iniData.find(section);
    if (it != iniData.end()) {
        for (const auto& pair : it->second) {
            keys.push_back(pair.first);
        }
    }
    keys_ = keys;
}
