#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
class SymbolTable;
class StringTable;
void saveRegs(std::ostream& stream);

void loadRegs(std::ostream& stream);



class Operand {
public:
	virtual std::string toString() const = 0;
	virtual void load(std::ostream& stream) const = 0;
	virtual void save(std::ostream& stream) const = 0;

};

class RValue : public Operand {
public:
	virtual void load(std::ostream& stream) const = 0;
	virtual void work_with_stack(std::ostream& stream, SymbolTable _symbolTable) = 0;
};


class MemoryOperand : public RValue {
protected:
	int _index;
	const SymbolTable* _symbolTable;

public:


	MemoryOperand(int index, const SymbolTable* symbolTable) :
		_index{ index }, _symbolTable{ symbolTable }{}

	std::string toString() const;

	void load(std::ostream& stream) const;

	void save(std::ostream& stream) const;

	void work_with_stack(std::ostream& stream, SymbolTable _symbolTable);

	int index() const;
};

class NumberOperand : public RValue {
protected:
	int _value;

public:
	NumberOperand(int value) : _value{ value } {};
	void save(std::ostream& stream) const;

	std::string toString() const;

	void load(std::ostream& stream) const;

	void work_with_stack(std::ostream& stream, SymbolTable _symbolTable);
};

class StringOperand : public Operand {
protected:

	int _index;
	const StringTable* _stringTable;
public:

	StringOperand(int index, const StringTable* stringTable) : _index{ index }, _stringTable{ stringTable }{};
	std::string toString() const;
	void load(std::ostream& stream) const;
	void save(std::ostream& stream) const;
};

class LabelOperand : public Operand {
protected:

	int _labelID;

public:

	
	LabelOperand(int labelID) : _labelID{ labelID } {};
	std::string toString() const;
	void load(std::ostream& stream) const;
	void save(std::ostream& stream) const;
	int labelID() const;
};

class Atom {
public:

	virtual std::string toString() const = 0;
	virtual void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const = 0;
};

class UnaryOpAtom : public Atom {
protected:
	std::string _name;
	std::shared_ptr<RValue> _operand;
	std::shared_ptr<MemoryOperand> _result;
public:
	UnaryOpAtom(std::string &name, std::shared_ptr<RValue> operand, std::shared_ptr<MemoryOperand> result) :
		_name{ name }, _operand{ operand }, _result{ result }{};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;

};

class BinaryOpAtom : public Atom {
protected:
	std::string _name;
	std::shared_ptr<RValue> _left;
	std::shared_ptr<RValue> _right;
	std::shared_ptr<MemoryOperand> _result;
public:
	BinaryOpAtom(std::string &name, std::shared_ptr<RValue> left, std::shared_ptr<RValue> right, std::shared_ptr<MemoryOperand> result) :
		_name{ name }, _right{ right }, _left{ left }, _result{ result }{};
	
	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;

};

class ConditionalJumpAtom : public Atom {
protected:
	std::string _condition;
	std::shared_ptr<RValue> _left;
	std::shared_ptr<RValue> _right;
	std::shared_ptr<LabelOperand> _label;
public:
	ConditionalJumpAtom(std::string& condition, std::shared_ptr<RValue> left, std::shared_ptr<RValue> right, std::shared_ptr<LabelOperand> label) :
		_condition{ condition }, _left{ left }, _right{ right }, _label{ label } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;


};

class OutAtom : public Atom {
protected:
	std::shared_ptr<Operand> _value;
public:
	OutAtom(std::shared_ptr<Operand> value) : _value{ value } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;
};

class InAtom : public Atom {
protected:
	std::shared_ptr<Operand> _result;
public:
	InAtom(std::shared_ptr<Operand> result) : _result{ result } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;
};

class JumpAtom : public Atom {
protected:
	std::shared_ptr<LabelOperand> _label;
public:
	JumpAtom(std::shared_ptr<LabelOperand> label) : _label{ label } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;
};

class LabelAtom : public Atom {
protected:
	std::shared_ptr<LabelOperand> _label;
public:
	LabelAtom(std::shared_ptr<LabelOperand> label) : _label{ label } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;

};

class CallAtom : public Atom {
protected:
	std::shared_ptr<MemoryOperand> _func;
	std::shared_ptr<MemoryOperand> _result;

public:
	CallAtom(std::shared_ptr<MemoryOperand> func, std::shared_ptr<MemoryOperand> result) : _func{ func }, _result{ result } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;

};

class RetAtom : public Atom {
protected:
	std::shared_ptr<RValue> _result;
public:
	RetAtom(std::shared_ptr<RValue> result) : _result{ result } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;
};

class ParamAtom : public Atom {
protected:
	std::shared_ptr<RValue> _arg;
public:
	ParamAtom(std::shared_ptr<RValue> arg) : _arg{ arg } {};

	std::string toString() const;
	void generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const;
};





