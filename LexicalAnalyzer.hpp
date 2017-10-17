#pragma once
#include "FiniteAutomata.hpp"
#include "Token.hpp"
#include <fstream>
#include <list>

class LexicalAnalyzer {
    public:
        LexicalAnalyzer() : _currentState(FiniteAutomata::States::Whitespace), _row(1), _column(1), _storingTokens(false) {};
        ~LexicalAnalyzer() {};

        Token currentToken() const;
        Token nextToken();
        void enableStoringTokens();
        void disableStoringTokens();
        void displayTokens(std::ofstream &os);
        
        void open(const std::string &filename);
        void open(const char* filename);
        bool eof();

        char codeToChar(FiniteAutomata::States state, std::string code);

    private:
        int _row;
        int _column;
        bool _storingTokens;
        std::ifstream _file;
        Token _currentToken;
        std::list<Token> _tokens;
        FiniteAutomata::States _currentState;
};