#pragma once
#include "LexicalAnalyzer.hpp"
#include "Node.hpp"
#include <set>
#include <vector>

class Parser {

    typedef std::shared_ptr<LexicalAnalyzer> PLexicalAnalyzer;

    enum class Precedence {
        First,
        Second,
        Third,
        Last,
    };

    public:
        Parser() : _root(Node::PNode(new Node(Node::Type::Root))){};
        Parser(const char* filename);
        ~Parser() {};

        void buildTree();

        template<typename T>
        void open(T filename);
        void log(std::ostream& os);

    private:
        Node::PNode parseExpr();
        Node::PNode parseSimpleExpr();
        Node::PNode parseTerm();
        Node::PNode parseFactor();
        Node::PNode parseIdentifier();

        void throwException(Token::Position_t pos, std::string msg);
        void expect(Token::Position_t pos, Token::SubClass received, Token::SubClass expected);
        bool Parser::checkPrecedence(Parser::Precedence p, Token::SubClass s);
        PLexicalAnalyzer _lexicalAnalyzer;
        Node::PNode _root;
        static const std::vector<std::set<Token::SubClass>> _precedences;
};