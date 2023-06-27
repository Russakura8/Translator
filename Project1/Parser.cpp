#include "Parser.h"
#include <map>

std::map<LexemType, std::string> jumps{
	{LexemType::opeq, "EQ"},
	{LexemType::opne, "NE"},
	{LexemType::oplt, "LT"},
	{LexemType::ople, "LE"},
	{LexemType::opgt, "GT"}
};

typedef std::shared_ptr<RValue> RV;



void Parser::printAtoms(std::ostream& stream) {
	for (auto& item : _atoms) {
		for (auto& atom : item.second) {
			stream << std::setiosflags(std::ios::left) << std::setw(10);
			stream << item.first;
			stream << atom->toString() << std::endl;
		}

	}
}

void Parser::generateAtom(std::unique_ptr<Atom> atom, Scope scope) {

	_atoms[scope].push_back(move(atom));
}
std::shared_ptr<LabelOperand> Parser::newLabel() {
	return std::make_shared<LabelOperand>(_currentLabel++);
}

void Parser::SyntaxError(const std::string& message) {
	try {
		throw -1;
	}
	catch (int a) {
		std::cerr << "Syntax Error: " + message;;
		exit(0);
	}
}

void Parser::lexicalError(const std::string& message) {
	try {
		throw - 1;
	}
	catch (int a) {
		std::cerr << "Lexical Error: " + message;;
		exit(0);
	}

}

void Parser::lexCheck() {
	if (_currentToken.type() == LexemType::error) {
		lexicalError(_currentToken.str());
	}
}


RV Parser::E(Scope scope) {
	return E7(scope);
}

RV Parser::E1(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::num) {
		auto q = std::make_shared<NumberOperand>(_currentToken.value());
		_currentToken = _scanner.getNextToken();
		return q;
	}
	else if (_currentToken.type() == LexemType::lpar) {
		_currentToken = _scanner.getNextToken();
		auto q = E(scope);
		lexCheck();
		if (_currentToken.type() != LexemType::rpar) {
			SyntaxError("Expected )");
		}
		_currentToken = _scanner.getNextToken();
		return q;
	}
	else if (_currentToken.type() == LexemType::chr) {
		auto q = std::make_shared<NumberOperand>(_currentToken.value());
		_currentToken = _scanner.getNextToken();
		return q;
	}
	else if (_currentToken.type() == LexemType::opinc) {
		_currentToken = _scanner.getNextToken();
		if (_currentToken.type() == LexemType::id) {
			auto q = _symbolTable.checkVar(scope, _currentToken.str());

			if ((scope != -1) && (_symbolTable[scope]._Const == 1) &&
				(_symbolTable[q->index()]._scope == -1)) {
				SyntaxError("Can't change global variable in constant functions");
			}
			
			if (q == nullptr) {
				SyntaxError("There's unannounced variable");
			}
			auto one = std::make_shared<NumberOperand>(1);
			_currentToken = _scanner.getNextToken();
			std::string add = "ADD";
			generateAtom(std::make_unique <BinaryOpAtom>(add, q, one, std::dynamic_pointer_cast<MemoryOperand>(q)), scope);
			return q;
		}
	}

	else if (_currentToken.type() == LexemType::id) {
		auto name = _currentToken.str();
		_currentToken = _scanner.getNextToken();
		return E1_(name, scope);
	}

}

RV Parser::E1_(std::string name, Scope scope) {
	auto p = _symbolTable.checkVar(scope, name);
	lexCheck();
	if (_currentToken.type() == LexemType::opinc) {
		if (p == nullptr) {
			SyntaxError("There's unannounced variable");
		}
		if ((scope != -1) && (_symbolTable[scope]._Const == 1) &&
			(_symbolTable[p->index()]._scope == -1)) {
			SyntaxError("Can't change global variable in constant functions");
		}

		_currentToken = _scanner.getNextToken();
		auto r = _symbolTable.alloc(scope);
		auto one = std::make_shared<NumberOperand>(1);
		std::string mov = "MOV";
		generateAtom(std::make_unique<UnaryOpAtom>(mov, p, r), scope);
		std::string add = "ADD";
	    generateAtom(std::make_unique<BinaryOpAtom>(add, p, one, std::dynamic_pointer_cast<MemoryOperand>(p)), scope);
		return r;
	}
	else if (_currentToken.type() == LexemType::lpar) {
		_currentToken = _scanner.getNextToken();
		auto n = ArgList(scope);
		if (_currentToken.type() != LexemType::rpar) {
			SyntaxError("Expected )");
		}
		_currentToken = _scanner.getNextToken();
		auto s = _symbolTable.checkFunc(name, n);
		if (s == nullptr) {
			SyntaxError("Something is wrong with the function call");
		}
		auto r = _symbolTable.alloc(scope);
		generateAtom(std::make_unique<CallAtom>(s, r), scope);
		return r;
	}

	if (p == nullptr) {
		SyntaxError("There's unannounced variable");
	}
	return p;
}


RV Parser::E2(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::opnot) {
		_currentToken = _scanner.getNextToken();
		auto r = _symbolTable.alloc(scope);
		auto q = E1(scope);
		std::string _not = "NOT";
		generateAtom(std::make_unique<UnaryOpAtom>(_not, q, r), scope);
		return r;
	}

	return E1(scope);

}


RV Parser::E3_(RV p, Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::opmult) {
		_currentToken = _scanner.getNextToken();
		auto r = E2(scope);
		auto s = _symbolTable.alloc(scope);
		std::string mul = "MUL";
		generateAtom(std::make_unique<BinaryOpAtom>(mul, p, r, std::dynamic_pointer_cast<MemoryOperand>(s)), scope);
		return E3_(s, scope);

	}

	return p;
}


RV Parser::E3(Scope scope) {
	
	return E3_(E2(scope), scope);
}


RV Parser::E4_(RV p, Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::opplus) {
		_currentToken = _scanner.getNextToken();
		auto r = E3(scope);
		auto s = _symbolTable.alloc(scope);
		std::string add = "ADD";
		generateAtom(std::make_unique<BinaryOpAtom>(add, p, r, std::dynamic_pointer_cast<MemoryOperand>(s)), scope);
		return E4_(s, scope);

	}

	else if (_currentToken.type() == LexemType::opminus) {
		_currentToken = _scanner.getNextToken();
		auto r = E3(scope);
		auto s = _symbolTable.alloc(scope);
		std::string sub = "SUB";
		generateAtom(std::make_unique<BinaryOpAtom>(sub, p, r, std::dynamic_pointer_cast<MemoryOperand>(s)), scope);
		return E4_(s, scope);

	}

	return p;
}

RV Parser::E4(Scope scope) {

	return E4_(E3(scope), scope);
}

RV Parser::E5_(RV p, Scope scope) {
	lexCheck();
	if ((_currentToken.type() == LexemType::opne) || (_currentToken.type() == LexemType::opeq) || (_currentToken.type() == LexemType::ople) || (_currentToken.type() == LexemType::opgt) || (_currentToken.type() == LexemType::oplt)) {
		std::string atom = jumps[_currentToken.type()];
		_currentToken = _scanner.getNextToken();
		auto r = E4(scope);
		auto s = _symbolTable.alloc(scope);
		auto l = newLabel();
		auto one = std::make_shared<NumberOperand>(1);
		auto zero = std::make_shared<NumberOperand>(0);
		std::string mov = "MOV";
		generateAtom(std::make_unique<UnaryOpAtom>(mov, one, s), scope);
		generateAtom(std::make_unique<ConditionalJumpAtom>(atom, p, r, l), scope);
		generateAtom(std::make_unique<UnaryOpAtom>(mov, zero, s), scope);
		generateAtom(std::make_unique<LabelAtom>(l), scope);
		return s;


	}
	return p;
}

RV Parser::E5(Scope scope) {

	return E5_(E4(scope), scope);
}

RV Parser::E6_(RV p, Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::opand) {
		_currentToken = _scanner.getNextToken();
		auto r = E5(scope);
		auto s = _symbolTable.alloc(scope);
		std::string _and = "AND";
		generateAtom(std::make_unique<BinaryOpAtom>(_and, p, r, std::dynamic_pointer_cast<MemoryOperand>(s)), scope);
		return E6_(s, scope);

	}

	return p;
}

RV Parser::E6(Scope scope) {

	return E6_(E5(scope), scope);
}

RV Parser::E7_(RV p, Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::opor) {
		_currentToken = _scanner.getNextToken();
		auto r = E6(scope);
		auto s = _symbolTable.alloc(scope);
		std::string _or = "OR";
		generateAtom(std::make_unique<BinaryOpAtom>(_or, p, r, std::dynamic_pointer_cast<MemoryOperand>(s)), scope);
		return E7_(s, scope);

	}

	return p;
}

RV Parser::E7(Scope scope) {

	return E7_(E6(scope), scope);
}

void Parser::DeclareStmt(Scope scope) {
	auto p = Type(scope);
	lexCheck();
	if (_currentToken.type() == LexemType::id) {
		auto name = _currentToken.str();
		_currentToken = _scanner.getNextToken();
		DeclareStmt_(p, name, scope);
	}

	else {
		SyntaxError("Expected variable or function");
	}

}

void Parser::DeclareStmt_(SymbolTable::TableRecord::RecordType p, std::string q, Scope scope) {
	
		lexCheck();
		if (_currentToken.type() == LexemType::lpar) {
			if (scope > -1) {
				SyntaxError("Function definition inside function");

			}
			_currentToken = _scanner.getNextToken();
			auto _func = _symbolTable.addFunc(q, p, 0);
			auto C_ = (*_func).index();
			auto ln = ParamList(C_);
			_symbolTable.set_len(C_, ln);
			
			lexCheck();
			if (_currentToken.type() != LexemType::rpar) {
				SyntaxError("Expected )");
			}
			_currentToken = _scanner.getNextToken();

			lexCheck();
			if (_currentToken.type() == LexemType::kwconst) {
				_symbolTable.set_const(C_, 1);
				_currentToken = _scanner.getNextToken();
			}
			else {
				_symbolTable.set_const(C_, 0);
			}
			

			lexCheck();
			if (_currentToken.type() == LexemType::lbrace) {
				_currentToken = _scanner.getNextToken();
				StmtList(C_);

				lexCheck();
				if (_currentToken.type() != LexemType::rbrace) {
					SyntaxError("Expected }");
				}
				_currentToken = _scanner.getNextToken();

				auto zero = std::make_shared<NumberOperand>(0);
				generateAtom(std::make_unique<RetAtom>(zero), C_);


			}


		}

		else if (_currentToken.type() == LexemType::opassign) {
			_currentToken = _scanner.getNextToken();
			lexCheck();
			if (_currentToken.type() == LexemType::num) {
				auto val = _currentToken.value();
				_currentToken = _scanner.getNextToken();
				_symbolTable.addVar(q, scope, p, val);

				DeclareVarList(p, scope);

				lexCheck();
				if (_currentToken.type() != LexemType::semicolon) {
					SyntaxError("Expected ;");
				}
				_currentToken = _scanner.getNextToken();
			}
		}

		else {
			_symbolTable.addVar(q, scope, p);
			DeclareVarList(p, scope);
			lexCheck();
			if (_currentToken.type() != LexemType::semicolon) {
				SyntaxError("Expected ;");
			}
			_currentToken = _scanner.getNextToken();
		}



}


SymbolTable::TableRecord::RecordType Parser::Type(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::kwchar) {
		_currentToken = _scanner.getNextToken();
		return SymbolTable::TableRecord::RecordType::chr;
	}

	else if (_currentToken.type() == LexemType::kwint) {
		_currentToken = _scanner.getNextToken();
		return SymbolTable::TableRecord::RecordType::integer;
	}

	else {
		SyntaxError("Expected char or int");
	}
	
}

void Parser::DeclareVarList(SymbolTable::TableRecord::RecordType p, Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::comma) {
		_currentToken = _scanner.getNextToken();
		lexCheck();
		if (_currentToken.type() == LexemType::id) {
			auto name = _currentToken.str();
			_currentToken = _scanner.getNextToken();
			InitVar(p, name, scope);
			DeclareVarList(p, scope);
		}
	}

}

void Parser::InitVar(SymbolTable::TableRecord::RecordType p, std::string q, Scope scope) {

	lexCheck();
	if (_currentToken.type() == LexemType::opassign) {
		_currentToken = _scanner.getNextToken();
		lexCheck();
		if ((_currentToken.type() == LexemType::num) || (_currentToken.type() == LexemType::chr)) {
			auto val = _currentToken.value();
			_currentToken = _scanner.getNextToken();
			_symbolTable.addVar(q, scope, p, val);
		}
	}

	_symbolTable.addVar(q, scope, p);
}

int Parser::ParamList(Scope scope) {
	if ((_currentToken.type() == LexemType::kwint) || (_currentToken.type() == LexemType::kwchar)) {
		auto q = Type(scope);
		lexCheck();
		if (_currentToken.type() == LexemType::id) {
			auto name = _currentToken.str();
			_currentToken = _scanner.getNextToken();
			_symbolTable.addVar(name, scope, q);

			return ParamList_(scope) + 1;
		}
	}

	return 0;
}

int Parser::ParamList_(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::comma) {
		_currentToken = _scanner.getNextToken();
		lexCheck();
		auto q = Type(scope);
		if (_currentToken.type() == LexemType::id) {
			auto name = _currentToken.str();
			_currentToken = _scanner.getNextToken();
			_symbolTable.addVar(name, scope, q);

			return ParamList_(scope) + 1;
		}
	}

	return 0;
}

int Parser::ArgList(Scope scope) {
	lexCheck();
	if (_currentToken.type() != LexemType::rpar) {
		auto p = E(scope);
		auto m = ArgList_(scope);
		generateAtom(std::make_unique<ParamAtom>(p), scope);
		return m + 1;
	}

	if (_currentToken.type() == LexemType::eof) {
		SyntaxError("Expected )");
	}

	return 0;
}

int Parser::ArgList_(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::comma) {
		_currentToken = _scanner.getNextToken();
		auto p = E(scope);
		auto m = ArgList_(scope);
		generateAtom(std::make_unique<ParamAtom>(p), scope);
		return m + 1;
	}
	
	if (_currentToken.type() == LexemType::eof) {
		SyntaxError("Expected )");
	}

	return 0;
}

void Parser::StmtList(Scope scope) {
	lexCheck();
	if ((_currentToken.type() != LexemType::eof) && 
		(_currentToken.type() != LexemType::rbrace) && 
		(_currentToken.type() != LexemType::kwcase) && 
		(_currentToken.type() != LexemType::kwdefault)) {
		Stmt(scope);
		StmtList(scope);
	}
}

void Parser::Stmt(Scope scope) {
	lexCheck();
	if ((_currentToken.type() == LexemType::kwint) || (_currentToken.type() == LexemType::kwchar)) {
		DeclareStmt(scope);
	}

	else if (_currentToken.type() == LexemType::id) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		AssignOrCallOp(scope);
	}

	else if (_currentToken.type() == LexemType::kwwhile) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		WhileOp(scope);
	}

	else if (_currentToken.type() == LexemType::kwfor) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		ForOp(scope);
	}

	else if (_currentToken.type() == LexemType::kwif) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		IfOp(scope);
	}

	else if (_currentToken.type() == LexemType::kwswitch) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		SwitchOp(scope);
	}

	else if (_currentToken.type() == LexemType::kwin) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		IOp(scope);
	}

	else if (_currentToken.type() == LexemType::kwout) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		OOp(scope);
	}

	else if (_currentToken.type() == LexemType::lbrace) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		_currentToken = _scanner.getNextToken();
		StmtList(scope);

		lexCheck();
		if (_currentToken.type() != LexemType::rbrace) {
			SyntaxError("Expected }");
		}
		_currentToken = _scanner.getNextToken();
	}

	else if (_currentToken.type() == LexemType::kwreturn) {
		if (scope == -1) {
			SyntaxError("Operator should be inside function");
		}
		_currentToken = _scanner.getNextToken();

		auto p = E(scope);

		generateAtom(std::make_unique<RetAtom>(p), scope);

		lexCheck();
		if (_currentToken.type() != LexemType::semicolon) {
			SyntaxError("Expected ;");
		}
		_currentToken = _scanner.getNextToken();

	}

	else if (_currentToken.type() == LexemType::semicolon) {
		_currentToken = _scanner.getNextToken();
	}

	else{
		SyntaxError("There's something strange");
	}


}

void Parser::AssignOrCallOp(Scope scope) {
	AssignOrCall(scope);
	lexCheck();
	if (_currentToken.type() == LexemType::semicolon) {
	_currentToken = _scanner.getNextToken();
	}
	else {
		SyntaxError("Expected ;");
	}
}

void Parser::AssignOrCall(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::id) {
		auto name = _currentToken.str();

		auto p = _symbolTable.checkVar(scope, name);
		if ((scope != -1) && (_symbolTable[scope]._Const == 1) &&
			(_symbolTable[p->index()]._scope == -1)) {
			SyntaxError("Can't change global variable in constant functions");
		}
		_currentToken = _scanner.getNextToken();
		AssignOrCall_(name, scope);
	}
	else {
		SyntaxError("Expected variable or function");
	}

}

void Parser::AssignOrCall_(std::string p, Scope scope) {

	lexCheck();
	if (_currentToken.type() == LexemType::opassign) {
		_currentToken = _scanner.getNextToken();
		auto q = E(scope);
		auto r = _symbolTable.checkVar(scope, p);

		if (r == nullptr) {
			SyntaxError("There's unannounced variable");
		}
		std::string mov = "MOV";
		generateAtom(std::make_unique<UnaryOpAtom>(mov, q, r), scope);

	}

	else if (_currentToken.type() == LexemType::lpar) {
		_currentToken = _scanner.getNextToken();
		auto n = ArgList(scope);

		lexCheck();
		if (_currentToken.type() != LexemType::rpar) {
			SyntaxError("Expected )");
		}
		_currentToken = _scanner.getNextToken();

		auto q = _symbolTable.checkFunc(p, n);
		if (q == nullptr) {
			SyntaxError("Something is wrong with the function call");
		}
		auto r = _symbolTable.alloc(scope);

		generateAtom(std::make_unique<CallAtom>(q, r), scope);

	}

	else {
		SyntaxError("Expected = or (");
	}
}

void Parser::WhileOp(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::kwwhile) {
		_currentToken = _scanner.getNextToken();
		auto l1 = newLabel();
		generateAtom(std::make_unique<LabelAtom>(l1), scope);

		lexCheck();
		if (_currentToken.type() == LexemType::lpar) {
			_currentToken = _scanner.getNextToken();
			auto p = E(scope);
			if (_currentToken.type() != LexemType::rpar) {
				SyntaxError("Expected )");
			}
			_currentToken = _scanner.getNextToken();

			auto l2 = newLabel();
			std::string eq = "EQ";
			auto zero = std::make_shared<NumberOperand>(0);
			generateAtom(std::make_unique<ConditionalJumpAtom>(eq, p, zero, l2), scope);

			Stmt(scope);

			

			generateAtom(std::make_unique<JumpAtom>(l1), scope);

			generateAtom(std::make_unique<LabelAtom>(l2), scope);


		}
		else{
			SyntaxError("Expected (");
		}
	}
}

void Parser::ForOp(Scope scope) {
	auto l1 = newLabel();
	auto l2 = newLabel();
	auto l3 = newLabel();
	auto l4 = newLabel();
	lexCheck();
	if (_currentToken.type() == LexemType::kwfor) {
		_currentToken = _scanner.getNextToken();

		lexCheck();
		if (_currentToken.type() == LexemType::lpar) {
			_currentToken = _scanner.getNextToken();

			ForInit(scope);

			lexCheck();
			if (_currentToken.type() == LexemType::semicolon) {
				_currentToken = _scanner.getNextToken();

				
				generateAtom(std::make_unique<LabelAtom>(l1), scope);

				auto p = ForExp(scope);

				lexCheck();
				if (_currentToken.type() == LexemType::semicolon) {
					_currentToken = _scanner.getNextToken();

					std::string eq = "EQ";
					auto zero = std::make_shared<NumberOperand>(0);
					generateAtom(std::make_unique<ConditionalJumpAtom>(eq, p, zero, l4), scope);
					generateAtom(std::make_unique<JumpAtom>(l3), scope);
					generateAtom(std::make_unique<LabelAtom>(l2), scope);

					ForLoop(scope);

					generateAtom(std::make_unique<JumpAtom>(l1), scope);

					if (_currentToken.type() != LexemType::rpar) {
						SyntaxError("Expected )");
					}
					_currentToken = _scanner.getNextToken();

					generateAtom(std::make_unique<LabelAtom>(l3), scope);

					Stmt(scope);

					generateAtom(std::make_unique<JumpAtom>(l2), scope);
					generateAtom(std::make_unique<LabelAtom>(l4), scope);

				}
				else {
					SyntaxError("Expected ;");
				}



			}
			else {
				SyntaxError("Expected ;");
			}
		}
		else {
			SyntaxError("Expected (");
		}
	}
}

void Parser::ForInit(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::id) {
		AssignOrCall(scope);
	}

}

RV Parser::ForExp(Scope scope) {
	// E = [!, (, ++, chr, id, num]

	lexCheck();
	if ((_currentToken.type() == LexemType::opnot) ||
		(_currentToken.type() == LexemType::lpar) ||
		(_currentToken.type() == LexemType::chr) ||
		(_currentToken.type() == LexemType::id) ||
		(_currentToken.type() == LexemType::num) ||
		(_currentToken.type() == LexemType::opinc)) {
		return E(scope);
	}

	return std::make_shared<NumberOperand>(1);
}

void Parser::ForLoop(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::id) {
		AssignOrCall(scope);
	}

	else if (_currentToken.type() == LexemType::opinc) {
		_currentToken = _scanner.getNextToken();

		lexCheck();
		if (_currentToken.type() == LexemType::id) {
			auto name = _currentToken.str();
			_currentToken = _scanner.getNextToken();
			auto p = _symbolTable.checkVar(scope, name);
		if ((scope != -1) && (_symbolTable[scope]._Const == 1) &&
			(_symbolTable[p->index()]._scope == -1)) {
			SyntaxError("Can't change global variable in constant functions");
		}
			if (p == nullptr) {
				SyntaxError("There's unannounced variable");
			}
			std::string add = "ADD";

			auto one = std::make_shared<NumberOperand>(1);

			generateAtom(std::make_unique <BinaryOpAtom>(add, p, one, p), scope);

			


		}
		else {
			SyntaxError("Expected variable");
		}
	}
}

void Parser::IfOp(Scope scope) {
	auto l1 = newLabel();
	auto l2 = newLabel();

	lexCheck();
	if (_currentToken.type() == LexemType::kwif) {
		_currentToken = _scanner.getNextToken();

		lexCheck();
		if (_currentToken.type() == LexemType::lpar) {
			_currentToken = _scanner.getNextToken();
			auto p = E(scope);

			if (_currentToken.type() != LexemType::rpar) {
				SyntaxError("Expected )");
			}
			_currentToken = _scanner.getNextToken();

			std::string eq = "EQ";
			auto zero = std::make_shared<NumberOperand>(0);
			generateAtom(std::make_unique<ConditionalJumpAtom>(eq, p, zero, l1), scope);

			Stmt(scope);

			generateAtom(std::make_unique<JumpAtom>(l2), scope);
			generateAtom(std::make_unique<LabelAtom>(l1), scope);

			ElsePart(scope);

			generateAtom(std::make_unique<LabelAtom>(l2), scope);

		}
		else {
			SyntaxError("Expected (");
		}
	}
}

void Parser::ElsePart(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::kwelse) {
		_currentToken = _scanner.getNextToken();
		Stmt(scope);
	}

}


void Parser::IOp(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::kwin) {
		_currentToken = _scanner.getNextToken();

		lexCheck();
		if (_currentToken.type() == LexemType::id) {
			auto name = _currentToken.str();
			_currentToken = _scanner.getNextToken();
			auto p = _symbolTable.checkVar(scope, name);
			if (p == nullptr) {
				SyntaxError("There's unannounced variable");
			}

			lexCheck();
			if (_currentToken.type() == LexemType::semicolon) {
				_currentToken = _scanner.getNextToken();

				generateAtom(std::make_unique<InAtom>(p), scope);
			}
			else {
				SyntaxError("Expected ;");
			}
		}
		else {
			SyntaxError("Expected variable");
		}
	}
}


void Parser::OOp(Scope scope) {
	lexCheck();
	if (_currentToken.type() == LexemType::kwout) {
		_currentToken = _scanner.getNextToken();

		OOp_(scope);

		lexCheck();
		if (_currentToken.type() != LexemType::semicolon) {
			SyntaxError("Expected ;");
		}
		_currentToken = _scanner.getNextToken();

	}
}

void Parser::OOp_(Scope scope) {

	lexCheck();
	if (_currentToken.type() == LexemType::str) {

		auto s = _currentToken.str();
		auto str = _stringTable.add(s);

		_currentToken = _scanner.getNextToken();


		generateAtom(std::make_unique<OutAtom>(str), scope);
	}

	else {
		auto p = E(scope);

		generateAtom(std::make_unique<OutAtom>(p), scope);
	}


}

void Parser::SwitchOp(Scope scope) {

	lexCheck();
	if (_currentToken.type() == LexemType::kwswitch) {
		_currentToken = _scanner.getNextToken();
		lexCheck();

		if (_currentToken.type() == LexemType::lpar) {
			_currentToken = _scanner.getNextToken();

			auto p = E(scope);

			lexCheck();
			if (_currentToken.type() != LexemType::rpar) {
				SyntaxError("Expected )");
			}
			_currentToken = _scanner.getNextToken();

			auto end = newLabel();

			lexCheck();
			if (_currentToken.type() == LexemType::lbrace) {
				_currentToken = _scanner.getNextToken();

				Cases(p, end, scope);

				lexCheck();
				if (_currentToken.type() != LexemType::rbrace) {
					SyntaxError("Expected }");
				}
				_currentToken = _scanner.getNextToken();

				generateAtom(std::make_unique<LabelAtom>(end), scope);

			}
			else {
				SyntaxError("Expected {");
			}
		}
		else {
			SyntaxError("Expected (");
		}
	}

}


void Parser::Cases(std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, Scope scope) {

	auto def1 = Acase(p, end, scope);

	Cases_(p, end, def1, scope);

}

void Parser::Cases_(std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, std::shared_ptr<LabelOperand> def, Scope scope) {

	lexCheck();
	if ((_currentToken.type() == LexemType::kwcase) ||
		(_currentToken.type() == LexemType::kwdefault)) {
		auto def1 = Acase(p, end, scope);

		if ((def != nullptr) && (def1 != nullptr)) {
			SyntaxError("Two default sect");
		}
		else {
			auto _def = -1;
			auto _def1 = -1;
			if (def != nullptr) {
				_def = (*def).labelID();
			}

			if (def1 != nullptr) {
				_def1 = (*def1).labelID();
			}

			if (_def > _def1) {
				Cases_(p, end, def, scope);
			}

			else {
				Cases_(p, end, def1, scope);
			}
		}
	}
	else {
		auto q = def;
		if (def != nullptr) {
			q = def;
		}
		else {
			q = end;
		}
		generateAtom(std::make_unique<JumpAtom>(q), scope);

	}

}

std::shared_ptr<LabelOperand> Parser::Acase(std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, Scope scope) {

	lexCheck();
	if (_currentToken.type() == LexemType::kwcase) {
		_currentToken = _scanner.getNextToken();

		lexCheck();
		if (_currentToken.type() == LexemType::num) {
			auto val = _currentToken.value();
			_currentToken = _scanner.getNextToken();
			
			auto vl = std::make_shared<NumberOperand>(val);

			auto next = newLabel();

			std::string ne = "NE";

			generateAtom(std::make_unique<ConditionalJumpAtom>(ne, p, vl, next), scope);

			lexCheck();
			if (_currentToken.type() == LexemType::colon) {
				_currentToken = _scanner.getNextToken();

				StmtList(scope);

				generateAtom(std::make_unique<JumpAtom>(end),scope);
				generateAtom(std::make_unique<LabelAtom>(next), scope);

				return nullptr;
			}
			else {
				SyntaxError("Expected :");
			}


		}

		else {
			SyntaxError("Expected number");
		}
	}

	else if (_currentToken.type() == LexemType::kwdefault) {
		_currentToken = _scanner.getNextToken();

		lexCheck();
		if (_currentToken.type() == LexemType::colon) {
			_currentToken = _scanner.getNextToken();

			auto next = newLabel();
			auto def = newLabel();

			generateAtom(std::make_unique<JumpAtom>(next), scope);
			generateAtom(std::make_unique<LabelAtom>(def), scope);

			StmtList(scope);

			generateAtom(std::make_unique<JumpAtom>(end), scope);
			generateAtom(std::make_unique<LabelAtom>(next), scope);

			return def;
		}

		else {
			SyntaxError("Expected :");
		}
	}

	else {
		SyntaxError("Expected case or default");
	}
}

bool Parser::translate() {
	StmtList(-1);
	auto records = _symbolTable.get_records();
	bool flag = false;
	for (int i = 0; i < records.size(); i++) {
		if (records[i]._name == "main") {
			flag = true;
			break;
		}
	}
	if (!flag) {
		SyntaxError("No main in program");
	}
	return true;
}




void Parser::generateProlog(std::ostream& stream) {
	stream << "ORG 0\n";
	stream << "LXI H, 0\n";
	stream << "SPHL\n";
	stream << "CALL main\n";
	stream << "END\n";
	stream << "@MULT:\n";
	stream << "; Code for MULT library function\n";
	stream << "@PRINT:\n";
	stream << "; Code for PRINT library function\n";

}

void Parser::generateFunction(std::ostream& stream, std::string
	function) {
	stream << function << ":\n";
	
	auto _records = _symbolTable.get_records();

	int index = -1;

	for (int i = 0; i < _records.size(); i++) {
		if ((_records[i]._kind == SymbolTable::TableRecord::RecordKind::func) &&
			(function == _records[i]._name)) {
			index = i;
			break;
		}
	}

	int m = -_records[index]._len;
	for (int i = 0; i < _records.size(); i++) {
		if ((_records[i]._kind == SymbolTable::TableRecord::RecordKind::var) &&
			(index == _records[i]._scope)) {
			m += 1;
		}
	}

	for (int i = 0; i < m; i++) {
		stream << "PUSH B\n";
	}

	for (auto& item : _atoms) {
		for (auto& atom : item.second) {
			if (item.first == index) {
				atom->generate(stream, _symbolTable, item.first);
			}
		}

	}

}

void Parser::generateCode(std::ostream&
	stream) {
	_symbolTable.calculateOffset();
	stream << "ORG 8000H\n";
	_symbolTable.generateGlobals(stream);
	_stringTable.generateStrings(stream);
	generateProlog(stream);
	auto functions = _symbolTable.functionNames();
	for (int i = 0; i < functions.size(); i++) {
		generateFunction(stream, functions[i]);
	}
}

void Parser::printResult(std::ostream& stream) {
	translate();
	printAtoms(stream);
	printSymbolTable(stream);
	printStringTable(stream);
	generateCode(stream);
}
