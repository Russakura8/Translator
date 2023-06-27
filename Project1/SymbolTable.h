#pragma once
#include "Atoms.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>

typedef int Scope;
const Scope GlobalScope = -1;


class SymbolTable {
public:
	struct TableRecord {
		std::string _name;
		enum class RecordKind {unknown, var, func};
		enum class RecordType { unknown, integer, chr };
		RecordKind _kind = RecordKind::unknown;
		RecordType _type = RecordType::unknown;
		int _len = -1;
		int _init = 0;
		Scope _scope = GlobalScope;
		int _offset = -1;
		int _Const = -1;

		TableRecord(std::string s, Scope scope) : _name{ s }, _scope{ scope } {};
		TableRecord(std::string s, Scope scope, RecordKind kind, RecordType type) : _name{ s }, _scope{ scope }, _type{ type }, _kind{ kind } {};
		TableRecord(std::string s, Scope scope, RecordType type, int _const) : _name{ s }, _scope{ scope }, _type{ type }, _kind{ RecordKind::func }, _Const{_const} {};
		TableRecord(std::string s, Scope scope, RecordKind kind, RecordType type, int init, int len) : _name{ s }, _scope{ scope }, _type{ type }, _kind{ kind }, _init{ init }, _len{ len } {};
		TableRecord(std::string s, Scope scope, RecordKind kind, RecordType type, int init) : _name{ s }, _scope{ scope }, _type{ type }, _kind{ kind }, _init{ init } {};
	};
	

	const TableRecord& operator[] (const int index) const;
	
	void set_len(int indx, int ln) {
		_records[indx]._len = ln;
	}

	void set_const(int indx, int _const) {
		_records[indx]._Const = _const;
	}

	std::shared_ptr<MemoryOperand> alloc(Scope scope);
	std::shared_ptr<MemoryOperand> addVar(const std::string& name,
		const Scope scope,
		const TableRecord::RecordType type,
		const int init = 0);

	std::shared_ptr<MemoryOperand> addFunc(const std::string& name,
		const TableRecord::RecordType type,
		const int len);

	std::shared_ptr<MemoryOperand> checkVar(const Scope scope,
		const std::string& name);

	std::shared_ptr<MemoryOperand> checkFunc(const std::string& name,
		int len);

	int getM(int scope) const;
	void calculateOffset();
	std::vector<std::string> functionNames() const;
	void generateGlobals(std::ostream& stream) const;

	std::vector<TableRecord> get_records() {
		return _records;
	}


	friend std::ostream& operator<<(std::ostream& stream, const SymbolTable& symbolTable);
	friend std::ostream& operator<<(std::ostream& stream, SymbolTable::TableRecord::RecordKind kind);
	friend std::ostream& operator<<(std::ostream& stream, SymbolTable::TableRecord::RecordType type);

private:
	std::vector<TableRecord> _records;
};
