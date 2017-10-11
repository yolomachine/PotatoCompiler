#pragma once
#include "FiniteAutomata.hpp"
#include "Token.hpp"

class LexicalAnalyzer {
    public:
        LexicalAnalyzer() {};
        ~LexicalAnalyzer() {};

        const Token currentToken();
        const Token nextToken();
        
    private:
        FiniteAutomata::States currentState;
};