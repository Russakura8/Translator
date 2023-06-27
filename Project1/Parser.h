#pragma once
#include <vector>
#include <memory>
#include "Atoms.h"
#include "SymbolTable.h"
#include "LexicalAnalyser.h"
#include "StringTable.h"

class Parser {
private:

	std::map<Scope, std::vector<std::unique_ptr<Atom>>> _atoms;
	StringTable _stringTable;
	SymbolTable _symbolTable;
	Scanner _scanner;
	Token _currentToken;
	int _currentLabel;
	void lexCheck();

public:

	bool translate();

	void generateCode(std::ostream&
		stream);

	void printResult(std::ostream& stream);


	void printSymbolTable(std::ostream& stream) {
		_symbolTable.calculateOffset();
		stream << _symbolTable;
	}
	void printStringTable(std::ostream& stream) {
		stream << _stringTable;
	}
	Parser(std::istream& stream) : _scanner{ stream }, _currentLabel{ 0 }, _currentToken{ 1 }{
		_currentToken = _scanner.getNextToken();
	}
	void printAtoms(std::ostream& stream);
	void generateAtom(std::unique_ptr<Atom> atom, Scope scope);
	std::shared_ptr<LabelOperand>newLabel();
	void SyntaxError(const std::string& message);
	void lexicalError(const std::string& message);

	void generateProlog(std::ostream& stream);
	void generateFunction(std::ostream& stream, std::string
		function);


	std::shared_ptr<RValue> E(Scope scope);

	std::shared_ptr<RValue> E1(Scope scope);
	std::shared_ptr<RValue> E1_(std::string p, Scope scope);

	std::shared_ptr<RValue> E2(Scope scope);

	std::shared_ptr<RValue> E3(Scope scope);
	std::shared_ptr<RValue> E3_(std::shared_ptr<RValue> p, Scope scope);

	std::shared_ptr<RValue> E4(Scope scope);
	std::shared_ptr<RValue> E4_(std::shared_ptr<RValue> p, Scope scope);

	std::shared_ptr<RValue> E5(Scope scope);
	std::shared_ptr<RValue> E5_(std::shared_ptr<RValue> p, Scope scope);

	std::shared_ptr<RValue> E6(Scope scope);
	std::shared_ptr<RValue> E6_(std::shared_ptr<RValue> p, Scope scope);

	std::shared_ptr<RValue> E7(Scope scope);
	std::shared_ptr<RValue> E7_(std::shared_ptr<RValue> p, Scope scope);

	int ArgList(Scope scope);

	int ArgList_(Scope scope);

	void DeclareStmt(Scope scope);
	void DeclareStmt_(SymbolTable::TableRecord::RecordType p, std::string q, Scope scope);
	
	SymbolTable::TableRecord::RecordType Type(Scope scope);

	void DeclareVarList(SymbolTable::TableRecord::RecordType p, Scope scope);

	void InitVar(SymbolTable::TableRecord::RecordType p, std::string q, Scope scope);

	int ParamList(Scope scope);

	int ParamList_(Scope scope);

	void StmtList(Scope scope);
	void Stmt(Scope scope);

	void AssignOrCallOp(Scope scope);

	void AssignOrCall(Scope scope);
	void AssignOrCall_(std::string, Scope scope);


	void WhileOp(Scope scope);

	void ForOp(Scope scope);
	void ForInit(Scope scope);
	std::shared_ptr<RValue> ForExp(Scope scope);
	void ForLoop(Scope scope);



	void IfOp(Scope scope);
	void ElsePart(Scope scope);

	void SwitchOp(Scope scope);
	void Cases(std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, Scope scope);
	std::shared_ptr<LabelOperand> Acase(std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, Scope scope);
	void Cases_(std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, std::shared_ptr<LabelOperand> def, Scope scope);


	void IOp(Scope scope);

	void OOp(Scope scope);
	void OOp_(Scope scope);





};
