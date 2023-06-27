#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>




enum class LexemType { num, chr, str, id, lpar, rpar, lbrace, rbrace, lbracket, rbracket, semicolon, comma, colon, opassign, opplus, opminus, opmult, opinc, opeq, opne, oplt, opgt, ople, opnot, opor, opand, kwint, kwchar, kwif, kwelse, kwswitch, kwcase, kwwhile, kwfor, kwreturn, kwin, kwout, kwdefault, kwconst, eof, error };

std::string string_lexem(LexemType type);

bool is_Letter(char& c);

class Token {
private:
	LexemType _type;
	int _value;
	std::string _str;

public:
	Token(LexemType type);

	Token(int value);

	Token(LexemType type, const std::string& str);

	Token(char c);

	void print(std::ostream& stream);

	LexemType type();

	int value();

	std::string str();

};

class Scanner {
private:
	std::istream& _stream;

	int state = 0;

	bool is_end = false;

	bool go_back = false;

	char lastChar;


public:
	Scanner(std::istream& stream) : _stream(stream) {};

	Token getNextToken();
};
static std::map<char, LexemType> punctuation{
										{'[',		LexemType::lbracket},
										{']',		LexemType::rbracket},
										{'(',		LexemType::lpar},
										{')',		LexemType::rpar},
										{'{',		LexemType::lbrace},
										{'}',		LexemType::rbrace},
										{';',		LexemType::semicolon},
										{',',		LexemType::comma},
										{':',		LexemType::colon}
};


static std::map<std::string, LexemType> keywords{
										{"return",  LexemType::kwreturn},
										{"int",     LexemType::kwint},
										{"char",    LexemType::kwchar},
										{"if",      LexemType::kwif},
										{"else",    LexemType::kwelse},
										{"switch",  LexemType::kwswitch},
										{"case",    LexemType::kwcase},
										{"while",   LexemType::kwwhile},
										{"for",     LexemType::kwfor},
										{"in",      LexemType::kwin},
										{"out",     LexemType::kwout},
										{"default", LexemType::kwdefault},
										{"const", LexemType::kwconst}
};