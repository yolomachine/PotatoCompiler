#include "Parser.hpp"

const std::vector<std::set<Token::SubClass>> Parser::_precedences = {
    {
        Token::SubClass::Not,
        Token::SubClass::Add,
        Token::SubClass::Sub,
        Token::SubClass::Address,
        Token::SubClass::Exp,
    },
    {
        Token::SubClass::Mult,
        Token::SubClass::Div,
        Token::SubClass::IntDiv,
        Token::SubClass::Mod,
        Token::SubClass::And,
        Token::SubClass::SHL,
        Token::SubClass::SHR,
    },
    {
        Token::SubClass::Add,
        Token::SubClass::Sub,
        Token::SubClass::Or,
        Token::SubClass::Xor,
    },
    {
        Token::SubClass::Equal,
        Token::SubClass::NEQ,
        Token::SubClass::Less,
        Token::SubClass::More,
        Token::SubClass::LEQ, 
        Token::SubClass::MEQ,
        Token::SubClass::In,
    },
};

const Parser::NodeTypesDict_t Parser::_nodeTypes = {
    { Node::Type::Identifier,      "Identifier"      },
    { Node::Type::ConstIdentifier, "ConstIdentifier" },
    { Node::Type::Integer,         "Integer"         },
    { Node::Type::Real,            "Real"            },
    { Node::Type::Char,            "Char"            },
    { Node::Type::Subrange,        "Subrange"        },
    { Node::Type::String,          "String"          },
    { Node::Type::Record,          "Record"          },
    { Node::Type::Array,           "Array"           },
    { Node::Type::IntConst,        "IntConst"        },
    { Node::Type::FloatConst,      "FloatConst"      },
    { Node::Type::CharConst,       "CharConst"       },
    { Node::Type::StringLiteral,   "StringLiteral"   },
};

const Parser::IdentifierTypeDict_t Parser::_identifierNodeTypes = {
    { "integer", Node::Type::Integer },
    { "char",    Node::Type::Char    },
    { "real",    Node::Type::Real    },
};

const Parser::SubClassTypeDict_t Parser::_subClassNodeTypes = {
    { Token::SubClass::String, Node::Type::String },
    { Token::SubClass::Array,  Node::Type::Array  },
    { Token::SubClass::Record, Node::Type::Record },
};

const Parser::OrdinalInitializersDict_t Parser::_ordinalInitializers = {
    { Node::Type::Integer,  Node::Type::IntConst      },
    { Node::Type::Subrange, Node::Type::IntConst      },
    { Node::Type::Real,     Node::Type::FloatConst    },
    { Node::Type::String,   Node::Type::StringLiteral },
    { Node::Type::Char,     Node::Type::CharConst     },
};

Parser::Parser(const char* filename) {
    open(filename);
};

Node::PNode_t Parser::parseProgram() {
    Node::PNode_t program = parseProgramHeading();
    std::set<Token::SubClass> declKeywords = { 
        Token::SubClass::Var, 
        Token::SubClass::Type,
        Token::SubClass::Const,
    };
    if (declKeywords.count(_lexicalAnalyzer->currentToken()._subClass))
        program->addChild(parseDeclaration());
    return program;
};

Node::PNode_t Parser::parseProgramHeading() {
    Node::PNode_t heading;
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Program);
    heading = Node::PNode_t(new Node(Node::Type::Program, _lexicalAnalyzer->currentToken()));
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
    heading->addChild(Node::PNode_t(new Identifier(_lexicalAnalyzer->currentToken())));
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Semicolon);
    _lexicalAnalyzer->nextToken();
    return heading;
};

Node::PNode_t Parser::parseDeclaration() {
    std::vector<Node::PNode_t> declarations;
    while (true) {
        Token t = _lexicalAnalyzer->currentToken();
        switch (t._subClass) {
        case Token::SubClass::Var:
            _lexicalAnalyzer->nextToken();
            declarations.push_back(parseVarDecl(t));
            break;
        case Token::SubClass::Type:
            _lexicalAnalyzer->nextToken();
            declarations.push_back(parseTypeDecl(t));
            break;
        case Token::SubClass::Const:
            declarations.push_back(parseConstDecl());
            break;
        default:
            return Node::PNode_t(new DeclarationsBlock(declarations));
        }
    }
};

std::vector<Node::PNode_t> Parser::parseDeclarations(Token::SubClass separator, bool restrictedInitialization) {
    std::vector<Node::PNode_t> declarations;
    expect(Token::SubClass::Identifier);

    do {
        std::vector<Node::PNode_t> identifiers;
        if (separator == Token::SubClass::Colon)
            identifiers = parseIdentifierList();
        else if (separator == Token::SubClass::Equal)
            identifiers.push_back(parseScalarIdentifier());
        expect(separator);
        _lexicalAnalyzer->nextToken();
        Node::PNode_t type(new AccessNode(Node::Type::Type, parseType(), "type"));
        for (auto i : identifiers)
            declarations.push_back(Node::PNode_t(new ParentNode(Node::Type::Identifier, i->_token, type)));
        if (!restrictedInitialization)
            if (_lexicalAnalyzer->currentToken()._subClass == Token::SubClass::Equal)
                if (identifiers.size() > 1)
                    throwException(_lexicalAnalyzer->currentToken()._pos, "Only one variable can be initialized");
                else {
                    _lexicalAnalyzer->nextToken();
                    declarations.back()->addChild(Node::PNode_t(new AccessNode(Node::Type::Value, parseInitialization(type), "value")));
                };
        expect(Token::SubClass::Semicolon);
    } while (_lexicalAnalyzer->nextToken()._subClass == Token::SubClass::Identifier);
    return declarations;
};

Node::PNode_t Parser::parseVarDecl(Token t) {
    return Node::PNode_t(new Declaration(Node::Type::VarDecl, t, parseDeclarations(Token::SubClass::Colon)));
};

Node::PNode_t Parser::parseTypeDecl(Token t) {
    return Node::PNode_t(new Declaration(Node::Type::TypeDecl, t, parseDeclarations(Token::SubClass::Equal, true)));
};

Node::PNode_t Parser::parseConstDecl() {
    Token constant = _lexicalAnalyzer->currentToken();
    Token identifier = _lexicalAnalyzer->nextToken();
    expect(identifier, Token::SubClass::Identifier);
    std::vector<Node::PNode_t> constants;
    do {
        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Equal);
        _lexicalAnalyzer->nextToken();
        constants.push_back(Node::PNode_t(new ParentNode(Node::Type::ConstDecl, identifier, parseExpr())));
        expect(Token::SubClass::Semicolon);
    } while ((identifier = _lexicalAnalyzer->nextToken())._subClass == Token::SubClass::Identifier);
    return Node::PNode_t(new Declaration(Node::Type::ConstDecl, constant, constants));
};

Node::PNode_t Parser::parseType() {
    Token t;
    Token current = _lexicalAnalyzer->currentToken();
    Token next = _lexicalAnalyzer->nextToken();
    Node::Type type;
    Node::PNode_t result;

    if (current._subClass == Token::SubClass::IntConst)
        type = Node::Type::IntConst;
    else if (current._subClass == Token::SubClass::Identifier)
        type = (_identifierNodeTypes.count(current._value.s) ? _identifierNodeTypes.at(current._value.s) : Node::Type::CustomType);
    else if (_subClassNodeTypes.count(current._subClass))
        type = _subClassNodeTypes.at(current._subClass);
    else
        throwException(current._pos, "Error in type definition");

    switch (type) {
    case Node::Type::Integer:
    case Node::Type::Real:
    case Node::Type::Char:
    case Node::Type::String:
        if (next._subClass == Token::SubClass::Range)
            throwException(current._pos, "Error in type definition");
        return Node::PNode_t(new Node(type, current));
    case Node::Type::CustomType:
        if (next._subClass != Token::SubClass::Range)
            return Node::PNode_t(new CustomType(current));
    case Node::Type::IntConst:
        expect(next, Token::SubClass::Range);
        t = _lexicalAnalyzer->nextToken();
        result = Node::PNode_t(new ParentNode(Node::Type::Subrange, next, Node::PNode_t(new Node(type, current))));
        if (t._subClass == Token::SubClass::IntConst)
            result->addChild(Node::PNode_t(new IntConst(t)));
        else if (t._subClass == Token::SubClass::Identifier && _identifierNodeTypes.count(t._value.s) == 0)
            result->addChild(Node::PNode_t(new CustomType(t)));
        else
            throwException(t._pos, "Error in type definition");
        _lexicalAnalyzer->nextToken();
        break;
    case Node::Type::Record:
        result = Node::PNode_t(new Declaration(type, current, parseDeclarations(Token::SubClass::Colon, true)));
        expect(Token::SubClass::End);
        _lexicalAnalyzer->nextToken();
        break;
    case Node::Type::Array:
        result = Node::PNode_t(new Node(type, current));
        expect(next, Token::SubClass::LeftBracket);
        _lexicalAnalyzer->nextToken();
        result->addChild(parseType());
        expect(result->_children.back()->_token, Token::SubClass::Range);
        expect(Token::SubClass::RightBracket);
        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Of);
        _lexicalAnalyzer->nextToken();
        result->addChild(Node::PNode_t(new AccessNode(Node::Type::Type, parseType(), "type")));
        break;
    }
        return result;
};

std::vector<Node::PNode_t> Parser::parseInitialization(Node::PNode_t type) {
    Node::PNode_t result;
    std::vector<Node::PNode_t> values, nodes;
    std::map<Node::PNode_t, bool> initialized;
    uint64_t lowerBound, upperBound;

    Node::Type currentType = type->_children.back()->_type;
    switch (currentType) {
    case Node::Type::Integer:
    case Node::Type::Subrange:
    case Node::Type::Real:
    case Node::Type::String:
    case Node::Type::Char:
        checkExprType(result = parseExpr(), _ordinalInitializers.at(currentType));
        values.push_back(result);
        break;
    case Node::Type::Array:
        lowerBound = type->_children.back()->_children.front()->_children.front()->_token._value.ull;
        upperBound = type->_children.back()->_children.front()->_children.back()->_token._value.ull;
        expect(Token::SubClass::LeftParenthesis);
        for (uint64_t i = lowerBound; i <= upperBound; ++i) {
            _lexicalAnalyzer->nextToken();
            for (auto j : parseInitialization(type->_children.back()->_children.back()))
                nodes.push_back(j);
            if (i < upperBound)
                expect(Token::SubClass::Comma);
        };
        expect(Token::SubClass::RightParenthesis);
        values.push_back(Node::PNode_t(new AccessNode(Node::Type::Value, nodes, "array")));
        _lexicalAnalyzer->nextToken();
        break;
    case Node::Type::Record:
        expect(Token::SubClass::LeftParenthesis);
        for (auto i : type->_children.back()->_children) {
            expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
            if (!std::strcmp(_lexicalAnalyzer->currentToken()._value.s, i->_token._value.s)) {
                expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Colon);
                _lexicalAnalyzer->nextToken();
                values.push_back(Node::PNode_t(new ParentNode(Node::Type::Identifier, *new Token(i->_token), parseInitialization(i->_children.back()))));
                initialized.insert({i, true});
            }
            else {
                for (auto j : type->_children.back()->_children)
                    if (!std::strcmp(_lexicalAnalyzer->currentToken()._value.s, j->_token._value.s))
                        if (initialized.count(j))
                            throwException(_lexicalAnalyzer->currentToken()._pos, "Field has already been initialized");
                        else if (&j != &i)
                            throwException(_lexicalAnalyzer->currentToken()._pos, "Incorrect initialization order");
                throwException(_lexicalAnalyzer->currentToken()._pos, "Unknown field");
            };
            expect(Token::SubClass::Semicolon);
        };
        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::RightParenthesis);
        _lexicalAnalyzer->nextToken();
        break;
    default:
        throwException(_lexicalAnalyzer->currentToken()._pos, "Can't initialize variable of this type");
    }
    return values;
};

Node::PNode_t Parser::parseScalarIdentifier() {
    expect(Token::SubClass::Identifier);
    Node::PNode_t node(new Identifier(_lexicalAnalyzer->currentToken()));
    _lexicalAnalyzer->nextToken();
    return node;
};

std::vector<Node::PNode_t> Parser::parseIdentifierList() {
    std::vector<Node::PNode_t> identifiers;
    do {
        identifiers.push_back(parseScalarIdentifier());
        if (_lexicalAnalyzer->currentToken()._subClass != Token::SubClass::Comma)
            break;
        _lexicalAnalyzer->nextToken();
    } while (true);
    return identifiers;
};

Node::PNode_t Parser::parseBinOp(Parser::Precedence p, Parser::PNodeFunction_t pf) {
    Node::PNode_t left = (this->*pf)();
    Token t = _lexicalAnalyzer->currentToken();
    while (checkPrecedence(p, t._subClass)) {
        _lexicalAnalyzer->nextToken();
        left = Node::PNode_t(new BinaryOperator(t, left, (this->*pf)()));
        t = _lexicalAnalyzer->currentToken();
    }
    return left;
};

Node::PNode_t Parser::parseExpr() {
    return parseBinOp(Precedence::Last, &Parser::parseSimpleExpr);
};
Node::PNode_t Parser::parseSimpleExpr() {
    return parseBinOp(Precedence::Third, &Parser::parseTerm);
};
Node::PNode_t Parser::parseTerm() {
    return parseBinOp(Precedence::Second, &Parser::parseFactor);
};

Node::PNode_t Parser::parseFactor() {
    Node::PNode_t e;
    Token t = _lexicalAnalyzer->currentToken();
    _lexicalAnalyzer->nextToken();
    if (checkPrecedence(Parser::Precedence::First, t._subClass))
        return Node::PNode_t(new UnaryOperator(t, parseExpr()));
    switch (t._subClass) {
    case Token::SubClass::Identifier:
        return parseIdentifier(t);
        break;
    case Token::SubClass::IntConst:
        return Node::PNode_t(new IntConst(t));
        break;
    case Token::SubClass::FloatConst:
        return Node::PNode_t(new FloatConst(t));
        break;
    case Token::SubClass::StringLiteral:
        return (t._raw.length() == 3 ? Node::PNode_t(new CharConst(t)) : Node::PNode_t(new StringLiteral(t)));
        break;
    case Token::SubClass::LeftParenthesis:
        e = parseExpr();
        expect(Token::SubClass::RightParenthesis);
        _lexicalAnalyzer->nextToken();
        return e;
        break;
    case Token::SubClass::EndOfFile:
        throwException(t._pos, "Unexpected end of file");
        break;
    default:
        throwException(t._pos, "Illegal expression");
        break;
    }
};

Node::PNode_t Parser::parseIdentifier(Token t) {
    std::vector<Node::PNode_t> args;
    Node::PNode_t ident = Node::PNode_t(new Identifier(t));
    std::set<Token::SubClass> allowed = { 
        Token::SubClass::Dot, 
        Token::SubClass::LeftBracket, 
        Token::SubClass::LeftParenthesis 
    };
    while (allowed.count(_lexicalAnalyzer->currentToken()._subClass)) {
        Token t = _lexicalAnalyzer->currentToken();
        _lexicalAnalyzer->nextToken();
        switch (t._subClass) {
            case Token::SubClass::Dot:
                ident = Node::PNode_t(new RecordAccess(ident, parseScalarIdentifier()));
                break;
            case Token::SubClass::LeftBracket:
                ident = Node::PNode_t(new ArrayIndex(ident, parseExpr()));
                expect(Token::SubClass::RightBracket);
                _lexicalAnalyzer->nextToken();
                break;
            case Token::SubClass::LeftParenthesis:
                if (ident->_type != Node::Type::Identifier)
                    throwException(t._pos, "Illegal function call");
                ident = Node::PNode_t(new FunctionCall(ident, parseArgs()));
                expect(Token::SubClass::RightParenthesis);
                _lexicalAnalyzer->nextToken();
                break;
            default:
                break;
        }
    }
    return ident;
};

std::vector<Node::PNode_t> Parser::parseArgs() {
    std::vector<Node::PNode_t> args; 
    if (_lexicalAnalyzer->currentToken()._subClass != Token::SubClass::RightParenthesis)
        do {
            args.push_back(parseExpr());
            if (_lexicalAnalyzer->currentToken()._subClass != Token::SubClass::Comma)
                break;
            _lexicalAnalyzer->nextToken();
        } while (true);
    return args;
};

void Parser::buildTree() {
    _root = parseProgram();
    _root->addChild(parseExpr());
};

template<typename T>
void Parser::open(T filename) {
    _lexicalAnalyzer = Parser::PLexicalAnalyzer_t(new LexicalAnalyzer(filename));
};

void Parser::visualizeTree(std::wostream& os, Node::PNode_t node, bool isLastChild = true,
                           std::vector<std::pair<int, bool>> margins = std::vector<std::pair<int, bool>>(0)) {
    for (auto i : margins) {
        if (i.second)
            os << L'│';
        for (auto j = 0; j < (i.first - i.second) - 1; ++j)
            os << " ";
    };
    os << (isLastChild ? L'└' : L'├') << L'─' << node->toString().c_str() << std::endl;

    for (auto i : node->_children) {
        margins.push_back({ node->toString().length() + 2, !isLastChild });
        visualizeTree(os, i, i == node->_children.back(), margins);
        margins.pop_back();
    };
};

void Parser::log(std::wostream& os) {
    try {
        buildTree();
    }
    catch (std::exception e) {
        os << e.what();
        return;
    }
    visualizeTree(os, _root);
};

void Parser::expect(Token::SubClass expected) {
    expect(_lexicalAnalyzer->currentToken(), expected);
};

void Parser::expect(Token t, Token::SubClass expected) {
    std::stringstream ss;
    if (expected != t._subClass) {
        if (t._subClass == Token::SubClass::EndOfFile)
            throwException(t._pos, "Unexpected end of file");
        else {
            ss << "Syntax error, \""
                << _lexicalAnalyzer->_subClassDict.at(expected)
                << "\" expected, but \""
                << _lexicalAnalyzer->_subClassDict.at(t._subClass)
                << "\" found";
            throwException(t._pos, ss.str());
        }
    }
};

void Parser::expect(Token t, Node::Type received, Node::Type expected) {
    std::stringstream ss;
    if (received != expected) {
        ss << "Incompatible types: \""
           << _nodeTypes.at(expected)
           << "\" expected, but \""
           << _nodeTypes.at(received)
           << "\" found";
        throwException(t._pos, ss.str());
    }
};

bool Parser::checkPrecedence(Parser::Precedence p, Token::SubClass s) {
    return _precedences.at(static_cast<int>(p)).count(s) ? true : false;
};

void Parser::checkExprType(Node::PNode_t expr, Node::Type type) {
    if (expr->_children.size()) {
        for (auto i : expr->_children)
            checkExprType(i, type);
        return;
    };
    expect(expr->_token, expr->_type, type);
};

void Parser::throwException(Token::Position_t pos, std::string msg) {
    std::stringstream ss;
    ss << "(" << pos.first << ", " << pos.second << "): " << msg.c_str();
    throw std::exception(ss.str().c_str());
};