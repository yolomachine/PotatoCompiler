#pragma once
#include "LexicalAnalyzer.hpp"

Token LexicalAnalyzer::currentToken() const {
    return _currentToken;
};
Token LexicalAnalyzer::nextToken() {
    return _currentToken;
};

void LexicalAnalyzer::open(const std::string &filename) {
    _file.open(filename);
};
void LexicalAnalyzer::open(const char* filename) {
    _file.open(filename);
};