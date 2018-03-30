#pragma once
#include "LexicalAnalyzer.hpp"
#include "Node.hpp"
#include "AsmCode.hpp"
#include <set>
#include <vector>

class Parser {

    typedef Node::PNode_t (Parser::*PNodeFunction_t)(void);
    typedef std::set<Node::Type> ScalarTypesDict_t;
    typedef std::map<Node::Type, std::string> NodeTypesDict_t;
    typedef std::map<std::string, Node::Type> IdentifierTypeDict_t;
    typedef std::map<Token::SubClass, Node::Type> SubClassTypeDict_t;
    typedef std::map<Token::SubClass, PNodeFunction_t> FunctionDict_t;
    typedef std::map<Node::Type, Node::Type> OrdinalInitializersDict_t;
    typedef std::set<Token::SubClass> DeclarationsKeywordsSet_t;
    typedef std::shared_ptr<LexicalAnalyzer> PLexicalAnalyzer_t;
    typedef std::vector<Node::PSymTable_t> VecPSymTable_t;
    typedef std::shared_ptr<std::vector<Node::PSymTable_t>> PVecPSymTable_t;
    typedef std::pair<Node::PNode_t, Node::PNode_t> PNodePair_t;

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
        Node::PNode_t parseProcedure();
        Node::PNode_t parseFunction();
        Node::PNode_t parseProgramHeading();
        Node::PNode_t parseDeclaration();
        Node::PNode_t parseStatement();

        Node::PNode_t parseVarDecl(Token t);
        Node::PNode_t parseTypeDecl(Token t);
        Node::PNode_t parseProcDecl(Token t);
        Node::PNode_t parseFuncDecl(Token t);
        Node::PNode_t parseConstDecl(Token t);
        
        Node::PNode_t parseType();
        Node::VecPNode_t parseInitialization(Node::PNode_t type);

        Node::PNode_t parseBinOp(Precedence p, PNodeFunction_t pf);
        Node::PNode_t parseExpr();
        Node::PNode_t parseSimpleExpr();
        Node::PNode_t parseTerm();
        Node::PNode_t parseFactor();
        Node::PNode_t parseConstExpr();
        Node::PNode_t parseIdentifier(Token t);
        Node::PNode_t parseScalarIdentifier();
        Node::VecPNode_t parseArgs();
        Node::VecPNode_t parseIdentifierList();
        Node::VecPNode_t parseDeclarations(Token::SubClass separator, bool restrictedInitialization = false, bool isLocal = false, bool isParamList = false);

        void throwException(Token::Position_t pos, std::string msg);
        void expect(Token::SubClass expected);
        void expect(Token t, Token::SubClass expected);
        void expect(Token t, Node::Type received, Node::Type expected);
        bool checkPrecedence(Precedence p, Token::SubClass s);
        void checkExpr(Node::PNode_t expr);
        void checkExprType(Node::PNode_t expr, Node::Type type);
        void checkIfExprIsConst(Node::PNode_t expr);
        void checkDuplicity(Token t);
        void checkDuplicity(Token t, Node::PSymTable_t symTable);
        void validateAssignment(Node::PNode_t left, Node::PNode_t right);
        void validateNodeTypes(Node::PNode_t leftTypeNode, Node::PNode_t rightTypeNode, const Token::Position_t pos);
        Node::Type defineNodeType(Token t);
        Node::PNode_t defineConstType(Token t);
        Node::PNode_t defineConstType(Node::Type type);
        Node::Type validateAndReturnExprType(Node::PNode_t expr);
        PNodePair_t* findSymbol(std::string name);
        PNodePair_t* findSymbol(std::string name, Node::PSymTable_t symTable);

        Node::PNode_t _root;
        PVecPSymTable_t _symTables;
        Node::PSymTable_t _typeAliases;
        PLexicalAnalyzer_t _lexicalAnalyzer;
        std::shared_ptr<std::set<std::string>> _funcIdentifiersTable;
        static const ScalarTypesDict_t _reducibleScalarTypes;
        static const NodeTypesDict_t _nodeTypes;
        static const SubClassTypeDict_t _subClassNodeTypes;
        static const IdentifierTypeDict_t _identifierNodeTypes;
        static const OrdinalInitializersDict_t _ordinalInitializers;
        static const DeclarationsKeywordsSet_t _declKeywords;
        static const std::vector<std::set<Token::SubClass>> _precedences;
};