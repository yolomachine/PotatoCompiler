#pragma once
#include "LexicalAnalyzer.hpp"
#include "Node.hpp"
#include <set>
#include <vector>

class Parser {

    typedef Node::PNode_t (Parser::*PNodeFunction_t)(void);
    typedef std::map<Token::SubClass, PNodeFunction_t> FunctionDict_t;
    typedef std::map<std::string, Node::Type> IdentifierTypeDict_t;
    typedef std::map<Token::SubClass, Node::Type> SubClassTypeDict_t;
    typedef std::map<Node::Type, std::string> NodeTypesDict_t;
    typedef std::map<Node::Type, Node::Type> OrdinalInitializersDict_t;
    typedef std::shared_ptr<LexicalAnalyzer> PLexicalAnalyzer_t;

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
        //Node::PNode_t parseLabelDecl(Token t);
        Node::PNode_t parseTypeDecl(Token t);
        Node::PNode_t parseConstDecl();
        
        Node::PNode_t parseType();
        std::vector<Node::PNode_t> parseInitialization(Node::PNode_t type);

        Node::PNode_t parseBinOp(Precedence p, PNodeFunction_t pf);
        Node::PNode_t parseExpr();
        Node::PNode_t parseSimpleExpr();
        Node::PNode_t parseTerm();
        Node::PNode_t parseFactor();
        Node::PNode_t parseIdentifier(Token t);
        Node::PNode_t parseScalarIdentifier();
        std::vector<Node::PNode_t> parseArgs();
        std::vector<Node::PNode_t> parseIdentifierList();
        std::vector<Node::PNode_t> parseDeclarations(Token::SubClass separator, bool restrictedInitialization = false);

        void throwException(Token::Position_t pos, std::string msg);
        void expect(Token::SubClass expected);
        void expect(Token t, Token::SubClass expected);
        void expect(Token t, Node::Type received, Node::Type expected);
        bool checkPrecedence(Precedence p, Token::SubClass s);
        void checkExprType(Node::PNode_t expr, Node::Type type);
        PLexicalAnalyzer_t _lexicalAnalyzer;
        Node::PNode_t _root;
        static const std::vector<std::set<Token::SubClass>> _precedences;
        static const IdentifierTypeDict_t _identifierNodeTypes;
        static const SubClassTypeDict_t _subClassNodeTypes;
        static const NodeTypesDict_t _nodeTypes;
        static const OrdinalInitializersDict_t _ordinalInitializers;
};