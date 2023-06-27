#include "StringTable.h"

const std::string& StringTable::operator[](const int index) const {
	if (index < 0 || index >= _strings.size()) throw "Index out of range";

	return _strings[index];
}

std::shared_ptr<StringOperand> StringTable::add(const std::string name) {
	for (int i = 0; i < _strings.size(); i++) {
		if (name == _strings[i]) {
			return std::make_shared<StringOperand>(i, this);
		}
	}
	_strings.push_back(name);

	return std::make_shared<StringOperand>(_strings.size() - 1, this);;

}

std::ostream& operator<<(std::ostream& stream, const StringTable& stringTable)
{
	stream << "String Table\n";
	int line = 0;
	for (auto& item : stringTable._strings) {
		stream << std::setiosflags(std::ios::left) << std::setw(10) << line++;
		stream << item << std::endl;
	}
	return stream;
}

void StringTable::generateStrings(std::ostream& stream) const {
	for (int i = 0; i < _strings.size(); i++) {
		stream << "str" << i << ": DB " << _strings[i] << "\n";
	}
}