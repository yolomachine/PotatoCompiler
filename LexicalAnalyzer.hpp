#pragma once
#include "FiniteAutomata.hpp"
#include "Token.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <iomanip>

class LexicalAnalyzer {

    typedef std::map<Token::Class, std::string> Dict_t;

    public:
        LexicalAnalyzer() : _currentState(FiniteAutomata::States::Whitespace), _row(1), _column(1) {};
        LexicalAnalyzer(char* filename);
        ~LexicalAnalyzer() {};

        Token currentToken() const;
        Token nextToken();

        template<typename T>
        void log(T& os);
        
        template<typename T>
        void open(T filename);

        bool eof();
        void throwException(FiniteAutomata::States state, std::pair<int, int> pos);

        char codeToChar(FiniteAutomata::States state, std::string code);

    private:
        int _row;
        int _column;
        std::ifstream _file;
        Token _currentToken;
        FiniteAutomata::States _currentState;
        static const Dict_t _dict;
};