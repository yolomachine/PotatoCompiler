#pragma once
#include "LexicalAnalyzer.hpp"

const LexicalAnalyzer::Dict_t LexicalAnalyzer::_dict = {
    { Token::Class::ReservedWord,    "Reserved word" },
    { Token::Class::Identifier,         "Identifier" },
    { Token::Class::Operator,             "Operator" },
    { Token::Class::Separator,           "Separator" },
    { Token::Class::Constant,             "Constant" },
};

LexicalAnalyzer::LexicalAnalyzer(char* filename) : _currentState(FiniteAutomata::States::Whitespace), _row(1), _column(1) {
    open(filename);
};

Token LexicalAnalyzer::currentToken() const {
    return _currentToken;
};

Token LexicalAnalyzer::nextToken() {
    char c;
    std::string raw;
    std::string val;
    std::string code;
    FiniteAutomata::States state;
    while (!eof()) {
        if (_file.eof())
            c = 128;
        else 
            _file >> std::noskipws >> c;
        state = FiniteAutomata::states[static_cast<unsigned int>(_currentState)][tolower(abs(c)) - 1];
        switch (state) {
        case FiniteAutomata::States::NewLine: 
        case FiniteAutomata::States::CommentNewLine:
            _column = 0;
            ++_row;
            break;
        case FiniteAutomata::States::BeginComment:
        case FiniteAutomata::States::BeginMultilineComment:
            raw = val = "";
        case FiniteAutomata::States::Whitespace:
        case FiniteAutomata::States::Comment:
        case FiniteAutomata::States::MultilineComment:
        case FiniteAutomata::States::Asterisk:
            break;
        case FiniteAutomata::States::ControlString:
        case FiniteAutomata::States::StringStart:
            if (code.length()) {
                val += codeToChar(_currentState, code);
                code = "";
            }
        case FiniteAutomata::States::StringEnd:
        case FiniteAutomata::States::Percent:
        case FiniteAutomata::States::Dollar:
        case FiniteAutomata::States::Ampersand:
            raw += c;
            break;
        case FiniteAutomata::States::Identifier:
            if (raw.length() && raw.back() == '&')
                val += '&';
            raw += c;
            val += tolower(c);
            break;
        case FiniteAutomata::States::OperatorDot:
        case FiniteAutomata::States::OperatorGreater:
        case FiniteAutomata::States::OperatorLess:
        case FiniteAutomata::States::OperatorMult:
        case FiniteAutomata::States::OperatorPlus:
        case FiniteAutomata::States::Operator:
            raw += c;
            val += tolower(c);
            break;
        case FiniteAutomata::States::DecimalCharCode:
        case FiniteAutomata::States::BinCharCode:
        case FiniteAutomata::States::OctCharCode:
        case FiniteAutomata::States::HexCharCode:
            if (c > '&')
                code += c;
            raw += c;
            break;
        case FiniteAutomata::States::Decimal:
        case FiniteAutomata::States::Bin:
        case FiniteAutomata::States::Oct:
        case FiniteAutomata::States::Hex:
        case FiniteAutomata::States::Float:
        case FiniteAutomata::States::FloatingPoint:
        case FiniteAutomata::States::FloatExp:
        case FiniteAutomata::States::FloatExpSign:
        case FiniteAutomata::States::FloatEnd:
        case FiniteAutomata::States::String:
        case FiniteAutomata::States::Separator:
        case FiniteAutomata::States::Colon:
        case FiniteAutomata::States::LeftParenthesis:
        case FiniteAutomata::States::Slash:
            raw += c;
            val += c;
            break;
        case FiniteAutomata::States::LookBack:
            --_column;
            state = FiniteAutomata::States::TokenEnd;
            _currentState = FiniteAutomata::States::Decimal;
            _file.putback(c);
            _file.putback(raw.back());
            raw.pop_back();
            val.pop_back();
            goto token;
        case FiniteAutomata::States::TokenEnd:
            if (code.length()) {
                val += codeToChar(_currentState, code);
                code = "";
            }
            if (!_file.eof())
                _file.putback(c);
            goto token;
        case FiniteAutomata::States::EndOfFile:
            _currentState = state = FiniteAutomata::States::EndOfFile;
            raw = val = "EOF";
            _column += 3;
            goto token;
        default:
            throwException(state, { _row, _column });
            break;
        }
        _file.peek();
        ++_column;
        _currentState = state;
    }

    token:
    Token t(_currentState, { _row, _column - raw.length() }, raw, val);
    _currentToken = t;
    _currentState = state;
    return t;
};

char LexicalAnalyzer::codeToChar(FiniteAutomata::States state, std::string code) {
    char c;
    switch (state) {
    case FiniteAutomata::States::BinCharCode:
        c = static_cast<char>(std::stoi(code, 0, 2));
        break;
    case FiniteAutomata::States::HexCharCode:
        c = static_cast<char>(std::stoi(code, 0, 16));
        break;
    case FiniteAutomata::States::OctCharCode:
        c = static_cast<char>(std::stoi(code, 0, 8));
        break;
    default:
        c = static_cast<char>(std::stoi(code));
        break;
    }
    return c;
};

template<typename T>
void LexicalAnalyzer::log(T& os) {
    std::list<Token> tokens;
    while (!eof())        tokens.push_back(nextToken());
    for (Token t : tokens) {
        std::string pos, type, raw, val;
        pos += "(";
        pos += std::to_string(t._pos.first);
        pos += ", ";
        pos += std::to_string(t._pos.second);
        pos += ")";
        type = _dict.at(t._class);
        raw = t._raw;
        os << pos << std::string(std::abs(static_cast<int>(20 - pos.length())), ' ') << type << std::string(std::abs(static_cast<int>(20 - type.length())), ' ')
           << raw << std::string(std::abs(static_cast<int>(30 - raw.length())), ' ');
        switch (t._vtype) {
        case Token::ValueType::ULL:
            val = std::to_string(t._value.ull);
            break;
        case Token::ValueType::Double:
            val = std::to_string(t._value.d);
            break;
        case Token::ValueType::String:
            val = t._value.s;
            break;
        default:
            break;
        }
        os << val << std::endl;
    }
};
template void LexicalAnalyzer::log<std::ostream>(std::ostream&);
template void LexicalAnalyzer::log<std::ofstream>(std::ofstream&);

template<typename T>
void LexicalAnalyzer::open(T filename) {
    _file.open(filename);
};
template void LexicalAnalyzer::open<const char*>(const char*);
template void LexicalAnalyzer::open<const std::string&>(const std::string&);

bool LexicalAnalyzer::eof() {
    return _file.eof() && (_currentState == FiniteAutomata::States::EndOfFile);
};
void LexicalAnalyzer::throwException(FiniteAutomata::States state, std::pair<int, int> pos) {
    std::string error;
    error += "(";
    error += std::to_string(pos.first);
    error += ", ";
    error += std::to_string(pos.second);
    error += "): ";
    switch (state) {
    case FiniteAutomata::States::EOLnWhileReading:
        error += "Unexpected end of line";
        break;
    case FiniteAutomata::States::FractionalPartExpected:
        error += "Fractional part of float expected";
        break;
    case FiniteAutomata::States::IllegalSymbol:
        error += "Illegal symbol";
        break;
    case FiniteAutomata::States::NumberExpected:
        error += "Number expected";
        break;
    case FiniteAutomata::States::ScaleFactorExpected:
        error += "Exponent scale factor expected";
        break;
    case FiniteAutomata::States::UnexpectedEndOfFile:
        error += "Unexpected end of file";
        break;
    case FiniteAutomata::States::UnexpectedSymbol:
        error += "Unexpected symbol";
        break;
    default:
        break;
    }

    std::cerr << error << std::endl;
    throw std::exception(error.c_str());
};