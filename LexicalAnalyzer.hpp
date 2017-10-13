#pragma once
#include "FiniteAutomata.hpp"
#include "Token.hpp"
#include <fstream>

class LexicalAnalyzer {
    public:
        LexicalAnalyzer() {};
        ~LexicalAnalyzer() {};

        Token currentToken() const;
        Token nextToken();
        
        void open(const std::string &filename);
        void open(const char* filename);

    private:
        std::ifstream _file;
        Token _currentToken;
        FiniteAutomata::States _currentState;
};