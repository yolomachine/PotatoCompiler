#pragma once
#include "LexicalAnalyzer.hpp"
#include "Node.hpp"
#include <set>
#include <vector>

class Parser {

    typedef std::shared_ptr<LexicalAnalyzer> PLexicalAnalyzer_t;
    typedef Node::PNode_t (Parser::*PFunction_t)(void);

    enum class Precedence {
        First,
        Second,
        Third,
        Last,
    };

    public:
        Parser() {};
        Parser(const char* filename);
        ~Parser() {};

        void buildTree();

        template<typename T>
        void open(T filename);
        void log(std::wostream& os);

    private:
        void visualizeTree(std::wostream& os, Node::PNode_t node, bool isLastChild, std::vector<std::pair<int, bool>> margins);

        Node::PNode_t parseBinOp(Precedence p, PFunction_t pf);
        Node::PNode_t parseExpr();
        Node::PNode_t parseSimpleExpr();
        Node::PNode_t parseTerm();
        Node::PNode_t parseFactor();
        Node::PNode_t parseIdentifier(Token t);
        std::vector<Node::PNode_t> parseArgs();

        void throwException(Token::Position_t pos, std::string msg);
        void expect(Token t, Token::SubClass expected);
        bool checkPrecedence(Precedence p, Token::SubClass s);
        PLexicalAnalyzer_t _lexicalAnalyzer;
        Node::PNode_t _root;
        static const std::vector<std::set<Token::SubClass>> _precedences;
};