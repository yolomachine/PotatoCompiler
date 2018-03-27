#include "Token.hpp"

const Token::Dict_t Token::_dict = {
    { "+",                 { Token::SubClass::Add,              Token::Class::Operator }},
    { "-",                 { Token::SubClass::Sub,              Token::Class::Operator }},
    { "*",                 { Token::SubClass::Mult,             Token::Class::Operator }},
    { "/",                 { Token::SubClass::Div,              Token::Class::Operator }},
    { "=",                 { Token::SubClass::Equal,            Token::Class::Operator }},
    { "<",                 { Token::SubClass::Less,             Token::Class::Operator }},
    { ">",                 { Token::SubClass::More,             Token::Class::Operator }},
    { "@",                 { Token::SubClass::Address,          Token::Class::Operator }},
    { "^",                 { Token::SubClass::Pointer,          Token::Class::Operator }},
    { ".",                 { Token::SubClass::Dot,              Token::Class::Operator }},
    { "+=",                { Token::SubClass::AddAssign,        Token::Class::Operator }},
    { "-=",                { Token::SubClass::SubAssign,        Token::Class::Operator }},
    { "*=",                { Token::SubClass::MultAssign,       Token::Class::Operator }},
    { "/=",                { Token::SubClass::DivAssign,        Token::Class::Operator }},
    { ":=",                { Token::SubClass::Assign,           Token::Class::Operator }},
    { "<=",                { Token::SubClass::LEQ,              Token::Class::Operator }},
    { ">=",                { Token::SubClass::MEQ,              Token::Class::Operator }},
    { "<<",                { Token::SubClass::SHL,              Token::Class::Operator }},
    { ">>",                { Token::SubClass::SHR,              Token::Class::Operator }},
    { "<>",                { Token::SubClass::NEQ,              Token::Class::Operator }},
    { "><",                { Token::SubClass::SymDiff,          Token::Class::Operator }},
    { "**",                { Token::SubClass::Exp,              Token::Class::Operator }},
    { "..",                { Token::SubClass::Subrange,         Token::Class::Operator }},
    { "and",               { Token::SubClass::And,              Token::Class::Operator }},
    { "or",                { Token::SubClass::Or,               Token::Class::Operator }},
    { "div",               { Token::SubClass::IntDiv,           Token::Class::Operator }},
    { "mod",               { Token::SubClass::Mod,              Token::Class::Operator }},
    { "not",               { Token::SubClass::Not,              Token::Class::Operator }},
    { "shl",               { Token::SubClass::SHL,              Token::Class::Operator }},
    { "shr",               { Token::SubClass::SHR,              Token::Class::Operator }},
    { "xor",               { Token::SubClass::Xor,              Token::Class::Operator }},
                              
    { ":",                 { Token::SubClass::Colon,            Token::Class::Separator }},
    { ",",                 { Token::SubClass::Comma,            Token::Class::Separator }},
    { ";",                 { Token::SubClass::Semicolon,        Token::Class::Separator }},
    { "[",                 { Token::SubClass::LeftBracket,      Token::Class::Separator }},
    { "]",                 { Token::SubClass::RightBracket,     Token::Class::Separator }},
    { "(.",                { Token::SubClass::LeftBracket,      Token::Class::Separator }},
    { ".)",                { Token::SubClass::RightBracket,     Token::Class::Separator }},
    { "(",                 { Token::SubClass::LeftParenthesis,  Token::Class::Separator }},
    { ")",                 { Token::SubClass::RightParenthesis, Token::Class::Separator }},

    { "absolute",          { Token::SubClass::Absolute,         Token::Class::ReservedWord }},
    { "array",             { Token::SubClass::Array,            Token::Class::ReservedWord }},
    { "asm",               { Token::SubClass::Asm,              Token::Class::ReservedWord }},
    { "begin",             { Token::SubClass::Begin,            Token::Class::ReservedWord }},
    { "break",             { Token::SubClass::Break,            Token::Class::ReservedWord }},
    { "case",              { Token::SubClass::Case,             Token::Class::ReservedWord }},
    { "const",             { Token::SubClass::Const,            Token::Class::ReservedWord }},
    { "constructor",       { Token::SubClass::Constructor,      Token::Class::ReservedWord }},
    { "continue",          { Token::SubClass::Continue,         Token::Class::ReservedWord }},
    { "destructor",        { Token::SubClass::Destructor,       Token::Class::ReservedWord }},
    { "do",                { Token::SubClass::Do,               Token::Class::ReservedWord }},
    { "downto",            { Token::SubClass::DownTo,           Token::Class::ReservedWord }},
    { "else",              { Token::SubClass::Else,             Token::Class::ReservedWord }},
    { "end",               { Token::SubClass::End,              Token::Class::ReservedWord }},
    { "false",             { Token::SubClass::False,            Token::Class::ReservedWord }},
    { "file",              { Token::SubClass::File,             Token::Class::ReservedWord }},
    { "for",               { Token::SubClass::For,              Token::Class::ReservedWord }},
    { "function",          { Token::SubClass::Function,         Token::Class::ReservedWord }},
    { "goto",              { Token::SubClass::Goto,             Token::Class::ReservedWord }},
    { "if",                { Token::SubClass::If,               Token::Class::ReservedWord }},
    { "implementation",    { Token::SubClass::Implementation,   Token::Class::ReservedWord }},
    { "in",                { Token::SubClass::In,               Token::Class::ReservedWord }},
    { "inline",            { Token::SubClass::Inline,           Token::Class::ReservedWord }},
    { "interface",         { Token::SubClass::Interface,        Token::Class::ReservedWord }},
    { "label",             { Token::SubClass::Label,            Token::Class::ReservedWord }},
    { "nil",               { Token::SubClass::Nil,              Token::Class::ReservedWord }},
    { "object",            { Token::SubClass::Object,           Token::Class::ReservedWord }},
    { "of",                { Token::SubClass::Of,               Token::Class::ReservedWord }},
    { "packed",            { Token::SubClass::Packed,           Token::Class::ReservedWord }},
    { "procedure",         { Token::SubClass::Procedure,        Token::Class::ReservedWord }},
    { "program",           { Token::SubClass::Program,          Token::Class::ReservedWord }},
    { "record",            { Token::SubClass::Record,           Token::Class::ReservedWord }},
    { "repeat",            { Token::SubClass::Repeat,           Token::Class::ReservedWord }},
    { "set",               { Token::SubClass::Set,              Token::Class::ReservedWord }},
    { "string",            { Token::SubClass::String,           Token::Class::ReservedWord }},
    { "then",              { Token::SubClass::Then,             Token::Class::ReservedWord }},
    { "to",                { Token::SubClass::To,               Token::Class::ReservedWord }},
    { "true",              { Token::SubClass::True,             Token::Class::ReservedWord }},
    { "type",              { Token::SubClass::Type,             Token::Class::ReservedWord }},
    { "unit",              { Token::SubClass::Unit,             Token::Class::ReservedWord }},
    { "until",             { Token::SubClass::Until,            Token::Class::ReservedWord }},
    { "uses",              { Token::SubClass::Uses,             Token::Class::ReservedWord }},
    { "var",               { Token::SubClass::Var,              Token::Class::ReservedWord }},
    { "while",             { Token::SubClass::While,            Token::Class::ReservedWord }},
    { "with",              { Token::SubClass::With,             Token::Class::ReservedWord }},
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
        _class = _dict.count(value) ? _dict.at(value).second : Class::Identifier;
        _subClass = _dict.count(value) ? _dict.at(value).first : SubClass::Identifier;
        break;

    case FiniteAutomata::States::Decimal:
    case FiniteAutomata::States::Bin:
    case FiniteAutomata::States::Hex:
    case FiniteAutomata::States::Oct:
        _vtype = ValueType::ULL;
        _value.ull = std::stoull(value, 0, static_cast<unsigned int>(state));
        _class = Class::Constant;
        _subClass = SubClass::IntConst;
        break;

    case FiniteAutomata::States::Float:
    case FiniteAutomata::States::FloatEnd:
    case FiniteAutomata::States::FloatingPoint:
        _vtype = ValueType::Double;
        _value.d = std::stod(value);
        _class = Class::Constant;
        _subClass = SubClass::FloatConst;
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
        _class = Class::StringLiteral;
        _subClass = (state == FiniteAutomata::States::EndOfFile ? SubClass::EndOfFile : SubClass::StringLiteral);

    default:
        break;
    };
};

std::string Token::toString() {
    std::stringstream ss;
    switch (_vtype) {
    case Token::ValueType::ULL:
        ss << _value.ull;
        break;
    case Token::ValueType::Double:
        ss << std::scientific << _value.d;
        break;
    case Token::ValueType::String:
        ss << _value.s;
        break;
    default:
        break;
    }
    return ss.str();
};