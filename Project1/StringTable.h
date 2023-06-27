#pragma once
#include "Atoms.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>

class StringTable {
private:
	std::vector<std::string> _strings;

public:
	const std::string& operator[](const int index) const;
	std::shared_ptr<StringOperand> add(const std::string name);
	friend std::ostream& operator<<(std::ostream& stream, const StringTable& stringTable);
	void generateStrings(std::ostream& stream) const;
};
