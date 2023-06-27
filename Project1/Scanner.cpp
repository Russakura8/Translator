#include "LexicalAnalyser.h"


std::string string_lexem(LexemType type) {
	std::vector<std::string> strs = { "num", "chr", "str", "id", "lpar", "rpar", "lbrace", "rbrace", "lbracket", "rbracket", "semicolon", "comma", "colon", "opassign", "opplus", "opminus", "opmult", "opinc", "opeq", "opne", "oplt", "opgt", "ople", "opnot", "opor", "opand", "kwint", "kwchar", "kwif", "kwelse", "kwswitch", "kwcase", "kwwhile", "kwfor", "kwreturn", "kwin", "kwout", "kwdefault","kwconst", "eof", "error"};

	return strs[(int)type];
}

bool is_Letter(char& c) {
	if ((c >= 'a') && (c <= 'z'))
		return true;
	if ((c >= 'A') && (c <= 'Z'))
		return true;
	if (c == '_')
		return true;

	return false;
}

Token::Token(LexemType type) {
	_type = type;

}

Token::Token(int value) {
	_type = LexemType::num;
	_value = value;

}

Token::Token(LexemType type, const std::string& str) {
	_type = type;
	_str = str;

}

Token::Token(char c) {
	_type = LexemType::chr;
	_value = c;
}

void Token::print(std::ostream& stream) {
	stream << '[' << string_lexem(this->_type);
	if (this->_type == LexemType::num) {
		stream << ", " << this->_value << ']';

	}

	else if (this->_type == LexemType::chr) {
		stream << ", " << '\'' << (char)this->_value << '\'' << ']';
	}

	else if ((this->_type == LexemType::id) || (this->_type == LexemType::str) || (this->_type == LexemType::error)) {
		stream << ", \"" << this->_str << "\"]";

	}
	else {
		stream << "]";
	}

	stream << '\n';

}

LexemType Token::type() {
	return _type;
}

int Token::value() {
	return _value;
}

std::string Token::str() {
	return _str;
}

Token Scanner::getNextToken() {
	if (this->is_end) {
		return Token(LexemType::eof);
	}

	std::string value;

	state = 0;

	char currentChar = 0;


	while (true) {
		if (go_back) {
			currentChar = lastChar;
			go_back = false;
		}

		else if (!_stream.eof()) {

			currentChar = _stream.get();

			if (_stream.eof()) {
				this->is_end = true;

				if (state == 2) {
					return Token(LexemType::error, "Close symbol please");
				}

				if (state == 4) {
					return Token(LexemType::error, "Close string please");
				}

				currentChar = ' ';
			}
		}
		else {
			this->is_end = true;
			return Token(LexemType::eof);
		}
		switch (state) {
			std::cout << currentChar;

		case 0:

			if ((currentChar == ' ') || (currentChar == '\t') || (currentChar == '\n')) {
				continue;
			}
			if (currentChar == '>') {
				return Token(LexemType::opgt);
			}
			if (currentChar == '*') {
				return Token(LexemType::opmult);
			}
			if (punctuation.count(currentChar) > 0) {
				return Token(punctuation[currentChar]);
			}
			if ((currentChar >= '0') && (currentChar <= '9')) {
				value = currentChar;
				state = 1;
				continue;
			}
			if (currentChar == '\'') {
				value = "";
				state = 2;
				continue;

			}

			if (currentChar == '"') {
				value = "";
				state = 4;
				continue;
			}


			if (is_Letter(currentChar)) {
				value = currentChar;
				state = 5;
				continue;
			}

			if (currentChar == '-') {
				state = 6;
				continue;
			}

			if (currentChar == '!') {
				state = 7;
				continue;
			}

			if (currentChar == '<') {
				state = 8;
				continue;
			}

			if (currentChar == '=') {
				state = 9;
				continue;
			}

			if (currentChar == '+') {
				state = 10;
				continue;
			}

			if (currentChar == '|') {
				state = 11;
				continue;
			}

			if (currentChar == '&') {
				state = 12;
				continue;
			}

			is_end = true;
			return Token(LexemType::error, "What the hell is this? I don't know that symbols. Take it away");

			break;

		case 1:
			if ((currentChar >= '0') && (currentChar <= '9')) {
				value += currentChar;

			}
			else {
				lastChar = currentChar;
				go_back = true;
				return Token(std::stoi(value));
			}

			break;

		case 2:
			if (currentChar == '\'') {
				is_end = true;
				return Token(LexemType::error, "Empty char");
			}
			state = 3;
			value = currentChar;
			break;


		case 3:
			if (currentChar == '\'') {
				state = 0;
				return Token(value[0]);
			}

			is_end = true;
			return Token(LexemType::error, "Too much symbols in char or you haven't closed it");
			break;

		case 4:
			if (currentChar == '"') {
				state = 0;
				return Token(LexemType::str, value);
			}

			value += currentChar;

			break;

		case 5:
			if (is_Letter(currentChar) || ((currentChar >= '0') && (currentChar <= '9'))) {
				value += currentChar;
			}

			else {
				lastChar = currentChar;
				go_back = true;
				state = 0;


				if (keywords.count(value) > 0)
					return Token(keywords[value]);
				else
					return Token(LexemType::id, value);
			}
			break;

		case 6:
			if ((currentChar >= '0') && (currentChar <= '9')) {
				value += '-';
				value += currentChar;
				state = 1;
				continue;
			}

			lastChar = currentChar;
			go_back = true;
			state = 0;

			return Token(LexemType::opminus);
			break;

		case 7:
			if (currentChar == '=') {
				state = 0;
				return Token(LexemType::opne);

			}
			else {
				lastChar = currentChar;
				go_back = true;
				state = 0;
				return Token(LexemType::opnot);
			}

			break;

		case 8:
			if (currentChar == '=') {
				state = 0;
				return Token(LexemType::ople);
			}

			else {
				lastChar = currentChar;
				go_back = true;
				state = 0;
				return Token(LexemType::oplt);
			}

			break;

		case 9:
			if (currentChar == '=') {
				state = 0;
				return Token(LexemType::opeq);
			}

			else {
				lastChar = currentChar;
				go_back = true;
				state = 0;
				return Token(LexemType::opassign);
			}
			break;

		case 10:
			if (currentChar == '+') {
				state = 0;
				return Token(LexemType::opinc);
			}

			else {
				lastChar = currentChar;
				go_back = true;
				state = 0;
				return Token(LexemType::opplus);
			}
			break;

		case 11:
			if (currentChar == '|') {
				state = 0;
				return Token(LexemType::opor);
			}

			else {
				is_end = true;
				return Token(LexemType::error, "Where second |?????");
			}
			break;

		case 12:
			if (currentChar == '&') {
				state = 0;
				return Token(LexemType::opand);
			}

			else {
				is_end = true;
				return Token(LexemType::error, "Where second &?????");
			}
			break;

		default:
			break;
		}
	}


}