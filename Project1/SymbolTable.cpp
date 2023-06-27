#include "SymbolTable.h"

typedef SymbolTable::TableRecord TSrec;

const TSrec& SymbolTable::operator[] (const int index) const {
	if (index < 0 || index >= _records.size()) throw "Index out of range";

	return _records[index];
}


std::shared_ptr<MemoryOperand> SymbolTable::alloc(Scope scope) {
	_records.push_back(TSrec("",  scope, SymbolTable::TableRecord::RecordKind::var, SymbolTable::TableRecord::RecordType::integer));
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

std::ostream& operator<<(std::ostream& stream, SymbolTable::TableRecord::RecordKind kind) {
	std::vector<std::string> strs = { "unknown", "var", "func"};
	stream << strs[(int)kind];

	return stream;

}

std::ostream& operator<<(std::ostream& stream, SymbolTable::TableRecord::RecordType type) {
	std::vector<std::string> strs = { "unknown", "integer", "chr" };
	stream << strs[(int)type];

	return stream;

}


std::ostream& operator<<(std::ostream& stream, const SymbolTable& symbolTable) {
	stream << "Symbol Table\n";
	std::vector<std::string> collumn{ "code", "name", "kind", "type", "len", "init", "scope", "offset", "const"};
	int line = 0;
	for (int i = 0; i < collumn.size(); i++) {
		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << collumn[i];
	}
	stream << std::endl;
	

	for (auto& item : symbolTable._records) {
		stream << std::setiosflags(std::ios::left) << std::setw(10) << line++;
		stream << std::setiosflags(std::ios::left) << std::setw(10);
		if (item._name.size() == 0) {
			stream << "[TMP" + std::to_string(line - 1) + "]";
		}
		else {
			stream << item._name;
		}

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << item._kind;

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << item._type;

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		if (item._len == -1) {
			stream << "None";
		}
		else {
			stream << item._len;
		}

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << item._init;

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << item._scope;

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << item._offset;

		stream << std::setiosflags(std::ios::left) << std::setw(10);
		stream << item._Const;

		stream << std::endl;
	}
	return stream;
}

std::shared_ptr<MemoryOperand> SymbolTable::addVar(const std::string& name,
	const Scope scope,
	const TableRecord::RecordType type,
	const int init) {
	
	auto is_ok = [&name, scope](TSrec& _s) {return _s._name == name && _s._scope == scope;};
	auto result = find_if(_records.begin(), _records.end(), is_ok);

	if (result != _records.end()) {
		return nullptr;
	}

	_records.push_back(TSrec(name, scope, SymbolTable::TableRecord::RecordKind::var, type, init));

	return std::make_shared<MemoryOperand>(_records.size() - 1, this);


}


std::shared_ptr<MemoryOperand> SymbolTable::addFunc(const std::string& name,
	const TableRecord::RecordType type,
	const int len) {

	auto is_ok = [&name](TSrec& _s) {return _s._name == name && _s._scope == -1; };
	auto result = find_if(_records.begin(), _records.end(), is_ok);

	if (result != _records.end()) {
		return nullptr;
	}

	_records.push_back(TSrec(name, -1, SymbolTable::TableRecord::RecordKind::func, type, 0, len));

	return std::make_shared<MemoryOperand>(_records.size() - 1, this);

}

std::shared_ptr<MemoryOperand> SymbolTable::checkVar(const Scope scope,
	const std::string& name) {

	auto is_ok = [&name, scope](TSrec& _s) {return _s._name == name && _s._scope == scope; };
	auto result = find_if(_records.begin(), _records.end(), is_ok);

	if (result == _records.end()) {
		auto is_ok = [&name](TSrec& _s) {return _s._name == name && _s._scope == -1;};
		result = find_if(_records.begin(), _records.end(), is_ok);
	}

	if ((result == _records.end()) || (result->_kind != TableRecord::RecordKind::var)) {

		return nullptr;
	}
	
	for (int i = 0; i < _records.size(); i++) {
		if ((_records[i]._name == name) && ((_records[i]._scope == -1) || (_records[i]._scope == scope))) {
			return std::make_shared<MemoryOperand>(i, this);

		}
	}
	
}

std::shared_ptr<MemoryOperand> SymbolTable::checkFunc(const std::string& name,
	int len) {

	auto is_ok = [&name](TSrec& _s) {return _s._name == name && _s._scope == -1;};
	auto result = find_if(_records.begin(), _records.end(), is_ok);

	if ((result == _records.end()) || (result->_kind != TableRecord::RecordKind::func) || (result->_len != len)) {
		return nullptr;
	}

	for (int i = 0; i < _records.size(); i++) {
		if (_records[i]._name == name) {
			return std::make_shared<MemoryOperand>(i, this);

		}
	}

}

int SymbolTable::getM(int scope) const{
	int m = 0;
	for (int i = 0; i < _records.size(); i++) {
		if ((i == scope) && (_records[i]._kind == TableRecord::RecordKind::func)) {
			m -= _records[i]._len;
		}
		else if ((_records[i]._scope == scope) && (_records[i]._kind == TableRecord::RecordKind::var)) {
			m += 1;
		}
	}

	return m;
}

void SymbolTable::calculateOffset() {

	for (int i = 0; i < _records.size(); i++) {
		if (_records[i]._kind == TableRecord::RecordKind::func) {
			int n = _records[i]._len;
			int m = getM(i);
			_records[i]._offset = 2 * (m + n + 1);
			for (int j = i + 1; j < _records.size(); j++) {
				int tmp = j - i;
				if (_records[j]._scope == i) {
					if (tmp <= n) {
						_records[j]._offset = 2 * (m + n + 1 - tmp);

					}
					else {
						_records[j]._offset = 2 * (m + n - tmp);
					}
				}
			}
		}
	}
}

std::vector<std::string> SymbolTable::functionNames() const {
	std::vector<std::string> funcs;
	for (int i = 0; i < _records.size(); i++) {
		if (_records[i]._kind == TableRecord::RecordKind::func) {
			funcs.push_back(_records[i]._name);
		}
	}
	return funcs;
}

void SymbolTable::generateGlobals(std::ostream& stream) const {
	for (int i = 0; i < _records.size(); i++) {
		if ((_records[i]._scope == -1) && (_records[i]._kind == SymbolTable::TableRecord::RecordKind::var)) {
			stream << "var" << i << ": DB " << _records[i]._init << "\n";
		}
	}
}





