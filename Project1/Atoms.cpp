#include "Atoms.h"
#include "SymbolTable.h"



std::vector<std::shared_ptr<RValue>> param_stack;


void saveRegs(std::ostream& stream) {
	stream << "PUSH B\n";
	stream << "PUSH D\n";
	stream << "PUSH H\n";
	stream << "PUSH PSW\n";
}

void loadRegs(std::ostream& stream) {
	stream << "POP PSW\n";
	stream << "POP H\n";
	stream << "POP D\n";
	stream << "POP B\n";
}


void NumberOperand::save(std::ostream& stream) const
{
}

std::string NumberOperand::toString() const {
	std::ostringstream stream;
	stream << "'" << _value << "'";
	return stream.str();
}

std::string MemoryOperand::toString() const {
	std::ostringstream stream;
	stream << _index;
	return stream.str();
}

std::string StringOperand::toString() const {
	std::ostringstream stream;
	stream << "S";
	stream << _index;
	return stream.str();
}

void StringOperand::load(std::ostream& stream) const
{
	stream << "LXI A, str" << _index << "\n";
}

void StringOperand::save(std::ostream& stream) const
{
}

std::string LabelOperand::toString() const {
	std::ostringstream stream;
	stream << "L" << _labelID;

	return stream.str();
}

void LabelOperand::load(std::ostream& stream) const
{
}

void LabelOperand::save(std::ostream& stream) const
{
}


std::string UnaryOpAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << _name << "," << _operand->toString() << ",,";
	stream << _result->toString() << ")";
	return stream.str();

};

std::string BinaryOpAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << _name << "," << _left->toString() << "," << _right->toString() << ",";
	stream << _result->toString() << ")";
	return stream.str();
}

std::string ConditionalJumpAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << _condition << "," << _left->toString() << "," << _right->toString() << ",";
	stream << _label->toString() << ")";
	return stream.str();
}

std::string LabelAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "LBL" << ",,," << _label->toString() << ")";
	return stream.str();
}

void LabelAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	stream << "    ;" << this->toString() << '\n';
	stream << _label->toString() << ":\n";
}

std::string JumpAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "JMP" << ",,," << _label->toString() << ")";
	return stream.str();
}

void JumpAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	stream << "    ;" << this->toString() << '\n';
	stream << "JMP " << _label->toString() << "\n";
}


std::string InAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "IN" << ",,," << _result->toString() << ")";
	return stream.str();
}

void InAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	stream << "    ;" << this->toString() << '\n';
	stream << "IN 0\n";
	_result->save(stream);
}

std::string OutAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "OUT" << ",,," << _value->toString() << ")";
	return stream.str();
}

void OutAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	stream << "    ;" << this->toString() << '\n';
	_value->load(stream);
	stream << "CALL @PRINT\n";

}


std::string CallAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "CALL" << ',' << _func->toString() << ",," << _result->toString() << ")";
	return stream.str();
}

void CallAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	stream << "    ;" << this->toString() << '\n';

	saveRegs(stream);
	stream << "LXI B, 0\n";
	stream << "PUSH B\n";

	auto _records = _symbolTable.get_records();

	auto _len = _records[_func->index()]._len;

	int n = param_stack.size() - 1;
	for (int i = n; i > n - _len; i--) {
		auto param = param_stack[i];
		param->work_with_stack(stream, _symbolTable);
	}
	for (int i = n; i > n - _len; i--) {
		param_stack.pop_back();
	}

	stream << "CALL " << _records[_func->index()]._name << "\n";

	for (int i = 0; i <= _len; i++) {
		stream << "POP B\n";
	}
	stream << "MOV A, B\n";

	_result->save(stream);

	loadRegs(stream);

}

std::string RetAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "RET" << ",,," << _result->toString() << ")";
	return stream.str();

}

void RetAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	stream << "    ;" << this->toString() << '\n';
	_result->load(stream);
	auto _records = _symbolTable.get_records();

	auto res = _records[scope]._offset;

	auto m = _symbolTable.getM(scope);

	stream << "LXI H, " << res << "\n";
	stream << "DAD SP\n";
	stream << "MOV M, A\n";
	for (int i = 0; i < m; i++) {
		stream << "POP B\n";
	}

	stream << "RET\n";

}

std::string ParamAtom::toString() const {
	std::ostringstream stream;
	stream << "(" << "PARAM" << ",,," << _arg->toString() << ")";
	return stream.str();
}

void ParamAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const
{
	param_stack.push_back(this->_arg);

}


int MemoryOperand::index() const {
	return _index;
}

int LabelOperand::labelID() const {
	return _labelID;
}


void NumberOperand::load(std::ostream& stream) const{
	stream << "MVI A, " << _value << "\n";
}



void MemoryOperand::save(std::ostream& stream) const {

	if ((*_symbolTable)[_index]._scope == -1) {
		stream << "STA " << _index << "\n";
	}
	else {
		stream << "LXI H, " << (*_symbolTable)[_index]._offset << "\n";
		stream << "DAD SP\n";
		stream << "MOV M, A\n";
	}
}

void UnaryOpAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const {
	stream << "    ;" << this->toString() << '\n';
	if (_name == "MOV") {
		_operand->load(stream);
		_result->save(stream);
	}
	else if (_name == "NOT") {
		static int count = 0;
		_operand->load(stream);

		stream << "CPI 0\n";
		stream << "MVI A, 1\n";
		stream << "JZ N" << count << "\n";
		stream << "MVI A, 0\n";
		stream << "NOT_LBL" << count++ << ":\n";

		_result->save(stream);

	}
}

void BinaryOpAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const {
	stream << "    ;" << this->toString() << '\n';
	_right->load(stream);
	if (_name == "MUL") {
		stream << "MOV D, A\n";
		_left->load(stream);
		stream << "MOV C, A\n";
		stream << "CALL @MULT\n";
		stream << "MOV A, C\n";
	}
	else {
		stream << "MOV B, A\n";
		_left->load(stream);
		if (_name == "ADD") {
			stream << "ADD B\n";
		}
		else if (_name == "SUB") {
			stream << "SUB B\n";
		}
		else if (_name == "AND") {
			stream << "ANA B\n";
		}
		else if (_name == "OR") {
			stream << "ORA B\n";
		}

	}
	_result->save(stream);

}

void ConditionalJumpAtom::generate(std::ostream& stream, SymbolTable _symbolTable, int scope) const {
	stream << "    ;" << this->toString() << '\n';
	_right->load(stream);
	stream << "MOV B, A\n";
	_left->load(stream);
	stream << "CMP B\n";
	if (_condition == "EQ") {
		stream << "JZ " << _label->toString() << "\n";
	}
	else if (_condition == "NE") {
		stream << "JNZ " << _label->toString() << "\n";
	}
	else if (_condition == "LT") {
		stream << "JM " << _label->toString() << "\n";
	}
	else if (_condition == "GT") {
		stream << "JP " << _label->toString() << "\n";
	}

	else if (_condition == "LE") {
		stream << "JM " << _label->toString() << "\n";
		stream << "JZ " << _label->toString() << "\n";
	}
}

void NumberOperand::work_with_stack(std::ostream& stream, SymbolTable _symbolTable) {
	stream << "LXI B, " << this->_value << "\n";
	stream << "PUSH B\n";
}

void MemoryOperand::work_with_stack(std::ostream& stream, SymbolTable _symbolTable) {

	stream << "LXI B, 0\n";
	load(stream);
	stream << "MOV C, A\n";
	stream << "PUSH B\n";

}

void MemoryOperand::load(std::ostream& stream) const {

	if ((*_symbolTable)[_index]._scope == -1) {
		stream << "LDA " << _index << "\n";
	}
	else {
		stream << "LXI H, " << (*_symbolTable)[_index]._offset << "\n";
		stream << "DAD SP\n";
		stream << "MOV A, M\n";
	}

}