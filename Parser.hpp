#pragma once
#include "LexicalAnalyzer.hpp"
#include "Node.hpp"
#include <set>
#include <vector>

class Parser {

    typedef std::shared_ptr<LexicalAnalyzer> PLexicalAnalyzer_t;
    typedef Node::PNode_t (Parser::*PNodeFunction_t)(void);
    typedef std::map<Token::SubClass, PNodeFunction_t> Dict_t;

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

        Node::PNode_t parseProgram();
        Node::PNode_t parseProgramHeading();
        Node::PNode_t parseDeclaration();

        Node::PNode_t parseVarDecl(Token t);
        Node::PNode_t parseLabelDecl(Token t);
        Node::PNode_t parseTypeDecl(Token t);
        Node::PNode_t parseConstDecl();
        
        Node::PNode_t parseType();
        Node::PNode_t parseSubrange();
        Node::PNode_t parseString();
        Node::PNode_t parseEnum();
        Node::PNode_t parseRecord();
        Node::PNode_t parseArray();
        Node::PNode_t parseSet();

        Node::PNode_t parseBinOp(Precedence p, PNodeFunction_t pf);
        Node::PNode_t parseExpr();
        Node::PNode_t parseSimpleExpr();
        Node::PNode_t parseTerm();
        Node::PNode_t parseFactor();
        Node::PNode_t parseIdentifier(Token t);
        Node::PNode_t parseScalarIdentifier();
        std::vector<Node::PNode_t> parseArgs();
        std::vector<Node::PNode_t> parseIdentifierList();
        std::vector<Node::PNode_t> parseDeclarations(Token::SubClass separator, Token::SubClass expectedType = Token::SubClass::Identifier);

        void throwException(Token::Position_t pos, std::string msg);
        void expect(Token::SubClass expected);
        void expect(Token t, Token::SubClass expected);
        bool checkPrecedence(Precedence p, Token::SubClass s);
        PLexicalAnalyzer_t _lexicalAnalyzer;
        Node::PNode_t _root;
        static const std::vector<std::set<Token::SubClass>> _precedences;
        static const Dict_t varTypes;
};