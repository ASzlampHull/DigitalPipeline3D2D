#pragma once

#include "ParserBase.h"
#include "ModelParserDataTypes.h"

class MTLParser final {
private:
	std::string filePath;
	std::unordered_map<std::string, ModelMTL> parsedModelMTLs;

public:
	MTLParser() = default;

	void ParseMTLFile(const std::string& filePath_);
	const void DefaultMaterialValues(ModelMTL& currentModelMTL) const;

	const std::unordered_map<std::string, ModelMTL>& GetParsedModelMTLs() const { return parsedModelMTLs; };
};
