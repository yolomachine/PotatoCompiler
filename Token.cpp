#include "Token.hpp"

const Token::Dict_t Token::_reservedWords = {
    { "and",               Token::Class::Operator },
    { "div",               Token::Class::Operator },
    { "mod",               Token::Class::Operator },
    { "not",               Token::Class::Operator },
    { "shl",               Token::Class::Operator },
    { "shr",               Token::Class::Operator },
    { "xor",               Token::Class::Operator },
    { "absolute",          Token::Class::ReservedWord },
    { "array",             Token::Class::ReservedWord },
    { "asm",               Token::Class::ReservedWord },
    { "begin",             Token::Class::ReservedWord },
    { "break",             Token::Class::ReservedWord },
    { "case",              Token::Class::ReservedWord },
    { "const",             Token::Class::ReservedWord },
    { "constructor",       Token::Class::ReservedWord },
    { "continue",          Token::Class::ReservedWord },
    { "destructor",        Token::Class::ReservedWord },
    { "do",                Token::Class::ReservedWord },
    { "downto",            Token::Class::ReservedWord },
    { "else",              Token::Class::ReservedWord },
    { "end",               Token::Class::ReservedWord },
    { "false",             Token::Class::ReservedWord },
    { "file",              Token::Class::ReservedWord },
    { "for",               Token::Class::ReservedWord },
    { "function",          Token::Class::ReservedWord },
    { "goto",              Token::Class::ReservedWord },
    { "if",                Token::Class::ReservedWord },
    { "implementation",    Token::Class::ReservedWord },
    { "in",                Token::Class::ReservedWord },
    { "inline",            Token::Class::ReservedWord },
    { "interface",         Token::Class::ReservedWord },
    { "label",             Token::Class::ReservedWord },
    { "nil",               Token::Class::ReservedWord },
    { "object",            Token::Class::ReservedWord },
    { "of",                Token::Class::ReservedWord },
    { "packed",            Token::Class::ReservedWord },
    { "procedure",         Token::Class::ReservedWord },
    { "program",           Token::Class::ReservedWord },
    { "record",            Token::Class::ReservedWord },
    { "repeat",            Token::Class::ReservedWord },
    { "set",               Token::Class::ReservedWord },
    { "string",            Token::Class::ReservedWord },
    { "then",              Token::Class::ReservedWord },
    { "to",                Token::Class::ReservedWord },
    { "true",              Token::Class::ReservedWord },
    { "type",              Token::Class::ReservedWord },
    { "unit",              Token::Class::ReservedWord },
    { "until",             Token::Class::ReservedWord },
    { "uses",              Token::Class::ReservedWord },
    { "var",               Token::Class::ReservedWord },
    { "while",             Token::Class::ReservedWord },
    { "with",              Token::Class::ReservedWord },
};

Token::Token(FiniteAutomata::States state, Position_t pos, std::string raw, std::string value) : _pos(pos), _raw(raw) {
    switch (state) {

    case FiniteAutomata::States::Identifier:        
        _value.s = new char[value.length() + 1];
        memcpy(_value.s, value.c_str(), value.length() + 1);
        _class = _reservedWords.count(value) ? _reservedWords.at(value) : Class::Identifier;
        break;

    case FiniteAutomata::States::Operator:
    case FiniteAutomata::States::OperatorDot:
    case FiniteAutomata::States::OperatorGreater:
    case FiniteAutomata::States::OperatorLess:
    case FiniteAutomata::States::OperatorMult:
    case FiniteAutomata::States::OperatorPlus:
    case FiniteAutomata::States::Slash:
        _value.s = new char[value.length() + 1];
        memcpy(_value.s, value.c_str(), value.length() + 1);
        _class = Class::Operator;
        break;

    case FiniteAutomata::States::Colon:
    case FiniteAutomata::States::Separator:
    case FiniteAutomata::States::LeftParenthesis:
        _value.s = new char[value.length() + 1];
        memcpy(_value.s, value.c_str(), value.length() + 1);
        _class = Class::Separator;

    case FiniteAutomata::States::Decimal:
        _value.ull = std::stoull(value);
        _class = Class::Constant;
        break;
    case FiniteAutomata::States::DecimalCharCode:
        memcpy(_value.s, new char(static_cast<char>(std::stoi(value.erase(0, 1)))), 2);
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::Bin:
        _value.ull = std::stoi(value.erase(0, 1), 0, 2);
        _class = Class::Constant;
        break;
    case FiniteAutomata::States::BinCharCode:
        memcpy(_value.s, new char(static_cast<char>(std::stoi(value.erase(0, 2), 0, 2))), 2);
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::Hex:
        _value.ull = std::stoi(value.erase(0, 1), 0, 16);
        _class = Class::Constant;
        break;
    case FiniteAutomata::States::HexCharCode:
        memcpy(_value.s, new char(static_cast<char>(std::stoi(value.erase(0, 2), 0, 16))), 2);
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::Oct:
        _value.ull = std::stoi(value.erase(0, 1), 0, 8);
        _class = Class::Constant;
        break;
    case FiniteAutomata::States::OctCharCode:
        memcpy(_value.s, new char(static_cast<char>(std::stoi(value.erase(0, 2), 0, 8))), 2);
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::Float:
    case FiniteAutomata::States::FloatEnd:
        _value.d = std::stod(value);
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::StringEnd:
        _value.s = new char[value.length() + 1];
        memcpy(_value.s, value.c_str(), value.length() + 1);
        _class = Class::Constant;

    default:
        break;
    };
};