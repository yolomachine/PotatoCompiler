#include "Token.hpp"

const Token::Dict_t Token::_dict = {
    { "+",                     Token::Class::Operator },
    { "-",                     Token::Class::Operator },
    { "*",                     Token::Class::Operator },
    { "/",                     Token::Class::Operator },
    { "=",                     Token::Class::Operator },
    { "<",                     Token::Class::Operator },
    { ">",                     Token::Class::Operator },
    { "@",                     Token::Class::Operator },
    { "^",                     Token::Class::Operator },
    { ".",                     Token::Class::Operator },
    { "+=",                    Token::Class::Operator },
    { "-=",                    Token::Class::Operator },
    { "*=",                    Token::Class::Operator },
    { "/=",                    Token::Class::Operator },
    { ":=",                    Token::Class::Operator },
    { "<<",                    Token::Class::Operator },
    { ">>",                    Token::Class::Operator },
    { "<>",                    Token::Class::Operator },
    { "><",                    Token::Class::Operator },
    { "**",                    Token::Class::Operator },
    { "..",                    Token::Class::Operator },
    { "and",                   Token::Class::Operator },
    { "div",                   Token::Class::Operator },
    { "mod",                   Token::Class::Operator },
    { "not",                   Token::Class::Operator },
    { "shl",                   Token::Class::Operator },
    { "shr",                   Token::Class::Operator },
    { "xor",                   Token::Class::Operator },
                              
    { ";",                    Token::Class::Separator },
    { ":",                    Token::Class::Separator },
    { ",",                    Token::Class::Separator },
    { "(",                    Token::Class::Separator },
    { ")",                    Token::Class::Separator },
    { "[",                    Token::Class::Separator },
    { "]",                    Token::Class::Separator },
    { "(.",                   Token::Class::Separator },
    { ".)",                   Token::Class::Separator },

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
    case FiniteAutomata::States::Operator:
    case FiniteAutomata::States::OperatorDot:
    case FiniteAutomata::States::OperatorGreater:
    case FiniteAutomata::States::OperatorLess:
    case FiniteAutomata::States::OperatorMult:
    case FiniteAutomata::States::OperatorPlus:
    case FiniteAutomata::States::Slash:
    case FiniteAutomata::States::Colon:
    case FiniteAutomata::States::Separator:
    case FiniteAutomata::States::LeftParenthesis:
        _vtype = ValueType::String;
        _value.s = new char[value.length() + 1];
        memcpy(_value.s, value.c_str(), value.length() + 1);
        _class = _dict.count(value) ? _dict.at(value) : Class::Identifier;
        break;

    case FiniteAutomata::States::Decimal:
    case FiniteAutomata::States::Bin:
    case FiniteAutomata::States::Hex:
    case FiniteAutomata::States::Oct:
        _vtype = ValueType::ULL;
        _value.ull = std::stoull(value, 0, static_cast<unsigned int>(state));
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::Float:
    case FiniteAutomata::States::FloatEnd:
    case FiniteAutomata::States::FloatingPoint:
        _vtype = ValueType::Double;
        _value.d = std::stod(value);
        _class = Class::Constant;
        break;

    case FiniteAutomata::States::DecimalCharCode:
    case FiniteAutomata::States::BinCharCode:
    case FiniteAutomata::States::HexCharCode:
    case FiniteAutomata::States::OctCharCode:
    case FiniteAutomata::States::StringEnd:
    case FiniteAutomata::States::EndOfFile:
        _vtype = ValueType::String;
        _value.s = new char[value.length() + 1];
        memcpy(_value.s, value.c_str(), value.length() + 1);
        _class = Class::Constant;

    default:
        break;
    };
};