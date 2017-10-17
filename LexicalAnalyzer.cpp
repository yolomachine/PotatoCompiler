#pragma once
#include "LexicalAnalyzer.hpp"

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
        state = FiniteAutomata::states[static_cast<unsigned int>(_currentState)][tolower(abs(c) - 1)];
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
        case FiniteAutomata::States::Ampersand:
        case FiniteAutomata::States::Dollar:
            raw += c;
            break;
        case FiniteAutomata::States::OperatorDot:
        case FiniteAutomata::States::OperatorGreater:
        case FiniteAutomata::States::OperatorLess:
        case FiniteAutomata::States::OperatorMult:
        case FiniteAutomata::States::OperatorPlus:
        case FiniteAutomata::States::Operator:
        case FiniteAutomata::States::Identifier:
            raw += c;
            val += tolower(c);
            break;
        case FiniteAutomata::States::DecimalCharCode:
        case FiniteAutomata::States::BinCharCode:
        case FiniteAutomata::States::OctCharCode:
        case FiniteAutomata::States::HexCharCode:
            raw += c;
            if (c > '&')
                code += c;
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
            break;
        }
        _file.peek();
        ++_column;
        _currentState = state;
    }

    token:
    Token t(_currentState, { _row, _column - raw.length() }, raw, val);
    if (_storingTokens)
        _tokens.push_back(t);
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
}

void LexicalAnalyzer::enableStoringTokens() {
    _storingTokens = true;
};
void LexicalAnalyzer::disableStoringTokens() {
    _storingTokens = false;
};
void LexicalAnalyzer::open(const std::string &filename) {
    _file.open(filename);
};
void LexicalAnalyzer::open(const char* filename) {
    _file.open(filename);
};
bool LexicalAnalyzer::eof() {
    return _file.eof() && (_currentState == FiniteAutomata::States::EndOfFile);
};
