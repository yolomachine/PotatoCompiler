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

const Parser::Dict_t Parser::varTypes = {
    { Token::SubClass::Identifier,      &Parser::parseSubrange, },
    { Token::SubClass::IntConst,        &Parser::parseSubrange, },
    { Token::SubClass::String,          &Parser::parseString,   },
    { Token::SubClass::LeftParenthesis, &Parser::parseEnum,     },
    { Token::SubClass::Record,          &Parser::parseRecord,   },
    { Token::SubClass::Array,           &Parser::parseArray,    },
    { Token::SubClass::Set,             &Parser::parseSet,      },
};

Parser::Parser(const char* filename) {
    open(filename);
};

Node::PNode_t Parser::parseProgram() {
    Node::PNode_t program = parseProgramHeading();
    std::set<Token::SubClass> declKeywords = { 
        Token::SubClass::Var, 
        Token::SubClass::Label,
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
            declarations.push_back(parseVarDecl(t));
            break;
        case Token::SubClass::Label:
            declarations.push_back(parseLabelDecl(t));
            break;
        case Token::SubClass::Type:
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

std::vector<Node::PNode_t> Parser::parseDeclarations(Token::SubClass separator, Token::SubClass expectedType) {
    std::vector<Node::PNode_t> declarations;
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);

    if (separator == Token::SubClass::Comma) {
        declarations = parseIdentifierList();
        expect(Token::SubClass::Semicolon);
        _lexicalAnalyzer->nextToken();
        return declarations;
    };

    do {
        std::vector<Node::PNode_t> identifiers;
        if (separator == Token::SubClass::Colon)
            identifiers = parseIdentifierList();
        else if (separator == Token::SubClass::Equal)
            identifiers.push_back(parseScalarIdentifier());
        expect(separator);
        _lexicalAnalyzer->nextToken();
        Node::PNode_t type = parseType();
        for (auto i : identifiers)
            declarations.push_back(Node::PNode_t(new ParentNode(Node::Type::Identifier, i->_token, type)));
        expect(Token::SubClass::Semicolon);
    } while (_lexicalAnalyzer->nextToken()._subClass == Token::SubClass::Identifier);
    return declarations;
};

Node::PNode_t Parser::parseVarDecl(Token t) {
    return Node::PNode_t(new Declaration(Node::Type::VarDecl, t, parseDeclarations(Token::SubClass::Colon)));
};

Node::PNode_t Parser::parseLabelDecl(Token t) {
    return Node::PNode_t(new Declaration(Node::Type::LabelDecl, t, parseDeclarations(Token::SubClass::Comma)));
};

Node::PNode_t Parser::parseTypeDecl(Token t) {
    return Node::PNode_t(new Declaration(Node::Type::TypeDecl, t, parseDeclarations(Token::SubClass::Equal)));
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
    Token t = _lexicalAnalyzer->currentToken();
    if (!varTypes.count(t._subClass))
        throwException(t._pos, "Error in type declaration");
    return (this->*varTypes.at(t._subClass))();
};

Node::PNode_t Parser::parseSubrange() {
    if (_lexicalAnalyzer->currentToken().toString() == "char")
        return parseScalarIdentifier();
    
    Token c = _lexicalAnalyzer->currentToken();
    Token n = _lexicalAnalyzer->nextToken();

    if (c._subClass == Token::SubClass::Identifier && n._subClass != Token::SubClass::Range)
        return Node::PNode_t(new Identifier(c));

    expect(Token::SubClass::Range);
    Node::PNode_t lowerBound(new Node(c._subClass == Token::SubClass::Identifier ? Node::Type::Identifier : Node::Type::IntConst, c));
    Token t = _lexicalAnalyzer->nextToken();
    if (t._subClass != Token::SubClass::Identifier && t._subClass != Token::SubClass::IntConst)
        throwException(t._pos, "Error in type declaration");
    _lexicalAnalyzer->nextToken();
    Node::PNode_t upperBound(new Node(t._subClass == Token::SubClass::Identifier ? Node::Type::Identifier : Node::Type::Identifier, t));
    return Node::PNode_t(new ParentNode(Node::Type::Subrange, n, lowerBound, upperBound));
};

Node::PNode_t Parser::parseString() {
    Token t = _lexicalAnalyzer->currentToken();
    _lexicalAnalyzer->nextToken();
    return Node::PNode_t(new Node(Node::Type::String, t));
};

Node::PNode_t Parser::parseEnum() {
    _lexicalAnalyzer->nextToken();
    Node::PNode_t enumerated(new Enum(parseIdentifierList()));
    expect(Token::SubClass::RightParenthesis);
    _lexicalAnalyzer->nextToken();
    return enumerated;
};

Node::PNode_t Parser::parseRecord() {
    Token t = _lexicalAnalyzer->currentToken();
    Node::PNode_t record(new Declaration(Node::Type::Record, t, parseDeclarations(Token::SubClass::Colon)));
    expect(Token::SubClass::End);
    _lexicalAnalyzer->nextToken();
    return record;
};
Node::PNode_t Parser::parseArray() {
    Node::PNode_t array(new Node(Node::Type::Array, _lexicalAnalyzer->currentToken()));
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::LeftBracket);
    _lexicalAnalyzer->nextToken();
    array->addChild(parseSubrange());
    expect(Token::SubClass::RightBracket);
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Of);
    _lexicalAnalyzer->nextToken();
    array->addChild(parseType());
    return array;
};
Node::PNode_t Parser::parseSet() {
    Node::PNode_t set(new Node(Node::Type::Set, _lexicalAnalyzer->currentToken()));
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Of);
    _lexicalAnalyzer->nextToken();
    set->addChild(parseSubrange());
    return set;
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
        identifiers.push_back(Node::PNode_t(new Identifier(_lexicalAnalyzer->currentToken())));
        if (_lexicalAnalyzer->nextToken()._subClass != Token::SubClass::Comma)
            break;
        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
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
        return Node::PNode_t(new StringLiteral(t));
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

bool Parser::checkPrecedence(Parser::Precedence p, Token::SubClass s) {
    return _precedences.at(static_cast<int>(p)).count(s) ? true : false;
}

void Parser::throwException(Token::Position_t pos, std::string msg) {
    std::stringstream ss;
    ss << "(" << pos.first << ", " << pos.second << "): " << msg.c_str();
    throw std::exception(ss.str().c_str());
}