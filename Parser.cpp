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

Parser::Parser(const char* filename) {
    open(filename);
};

Token Parser::current() {
    return _lexicalAnalyzer->currentToken();
};

Token Parser::next() {
    return _lexicalAnalyzer->nextToken();
};

Node::PNode_t Parser::parseProgram() {
    Node::PNode_t program = parseProgramHeading();
    std::set<Token::SubClass> declKeywords = { 
        Token::SubClass::Var, 
        Token::SubClass::Label,
        Token::SubClass::Type,
        Token::SubClass::Const,
    };
    if (declKeywords.count(current()._subClass))
        program->addChild(parseDeclaration());
    return program;
};

Node::PNode_t Parser::parseProgramHeading() {
    Node::PNode_t heading;
    expect(next(), Token::SubClass::Program);
    heading = Node::PNode_t(new Node(Node::Type::Program, current()));
    expect(next(), Token::SubClass::Identifier);
    heading->addChild(Node::PNode_t(new Identifier(current())));
    expect(next(), Token::SubClass::Semicolon);
    next();
    return heading;
};

Node::PNode_t Parser::parseDeclaration() {
    std::vector<Node::PNode_t> declarations;
    while (true) {
        Token t = current();
        switch (t._subClass) {
        case Token::SubClass::Var:
            declarations.push_back(Node::PNode_t(new Declaration(t, parseDeclarations(Token::SubClass::Colon, Node::Type::VarDecl))));
            break;
        case Token::SubClass::Label:
            declarations.push_back(Node::PNode_t(new Declaration(t, parseDeclarations(Token::SubClass::Comma, Node::Type::LabelDecl))));
            break;
        case Token::SubClass::Type:
            declarations.push_back(Node::PNode_t(new Declaration(t, parseDeclarations(Token::SubClass::Equal, Node::Type::TypeDecl))));
            break;
        case Token::SubClass::Const:
            declarations.push_back(parseConst());
            break;
        default:
            return Node::PNode_t(new DeclarationsBlock(declarations));
        }
    }
};

std::vector<Node::PNode_t> Parser::parseDeclarations(Token::SubClass separator, Node::Type declType, Token::SubClass expectedType) {
    std::vector<Node::PNode_t> declarations;
    expect(next(), Token::SubClass::Identifier);

    if (separator == Token::SubClass::Comma) {
        declarations = parseIdentifierList();
        expect(Token::SubClass::Semicolon);
        next();
        return declarations;
    };

    do {
        std::vector<Node::PNode_t> identifiers;
        if (separator == Token::SubClass::Colon)
            identifiers = parseIdentifierList();
        else if (separator == Token::SubClass::Equal) {
            identifiers.push_back(parseScalarIdentifier());
            next();
        };
        expect(separator);
        expect(next(), expectedType);
        //Node::PNode_t type = parseType();
        for (auto i : identifiers)
            declarations.push_back(Node::PNode_t(new ParentNode(declType, i->_token, Node::PNode_t(new Identifier(current())))));
        expect(next(), Token::SubClass::Semicolon);
    } while (next()._subClass == Token::SubClass::Identifier);
    return declarations;
};

//Node::PNode_t Parser::parseVar() {
//    Token var = _lexicalAnalyzer->currentToken();
//    std::vector<Node::PNode_t> declarations, identifiers;
//    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
//    do {
//        identifiers = parseIdentifierList();
//        expect(Token::SubClass::Colon);
//        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
//        declarations.push_back(Node::PNode_t(new ParentNode(Node::Type::Declaration, _lexicalAnalyzer->currentToken(), identifiers)));
//        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Semicolon);
//    } while (_lexicalAnalyzer->nextToken()._subClass == Token::SubClass::Identifier);
//    return Node::PNode_t(new Declaration(Node::Type::Var, var, declarations));
//};
//
//Node::PNode_t Parser::parseLabel() {
//    Token label = _lexicalAnalyzer->currentToken();
//    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
//    std::vector<Node::PNode_t> labels = parseIdentifierList();
//    expect(Token::SubClass::Semicolon);
//    _lexicalAnalyzer->nextToken();
//    return Node::PNode_t(new Declaration(Node::Type::Label, label, labels));
//};
//
//Node::PNode_t Parser::parseType() {
//    Token type = _lexicalAnalyzer->currentToken();
//    Token identifier = _lexicalAnalyzer->nextToken();
//    expect(identifier, Token::SubClass::Identifier);
//    std::vector<Node::PNode_t> types;
//    do {
//        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Equal);
//        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
//
//        types.push_back(Node::PNode_t(new ParentNode(Node::Type::Declaration, identifier, Node::PNode_t(new Identifier(_lexicalAnalyzer->currentToken())))));
//
//        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Semicolon);
//    } while ((identifier = _lexicalAnalyzer->nextToken())._subClass == Token::SubClass::Identifier);
//    return Node::PNode_t(new Declaration(Node::Type::Type, type, types));
//};

Node::PNode_t Parser::parseConst() {
    Token constant = current();
    Token identifier = next();
    expect(identifier, Token::SubClass::Identifier);
    std::vector<Node::PNode_t> constants;
    do {
        expect(next(), Token::SubClass::Equal);
        next();
        constants.push_back(Node::PNode_t(new ParentNode(Node::Type::ConstDecl, identifier, parseExpr())));
        expect(Token::SubClass::Semicolon);
    } while ((identifier = next())._subClass == Token::SubClass::Identifier);
    return Node::PNode_t(new Declaration(constant, constants));
};

Node::PNode_t Parser::parseScalarIdentifier() {
    expect(Token::SubClass::Identifier);
    return Node::PNode_t(new Identifier(current()));
};

std::vector<Node::PNode_t> Parser::parseIdentifierList() {
    std::vector<Node::PNode_t> identifiers;
    do {
        identifiers.push_back(Node::PNode_t(new Identifier(current())));
        if (next()._subClass != Token::SubClass::Comma)
            break;
        expect(next(), Token::SubClass::Identifier);
    } while (true);
    return identifiers;
};

Node::PNode_t Parser::parseBinOp(Parser::Precedence p, Parser::PNodeFunction_t pf) {
    Node::PNode_t left = (this->*pf)();
    Token t = current();
    while (checkPrecedence(p, t._subClass)) {
        next();
        left = Node::PNode_t(new BinaryOperator(t, left, (this->*pf)()));
        t = current();
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
    Token t = current();
    next();      
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
        next();
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
    while (allowed.count(current()._subClass)) {
        Token t = current();
        next();
        switch (t._subClass) {
            case Token::SubClass::Dot:
                ident = Node::PNode_t(new RecordAccess(ident, parseScalarIdentifier()));
                break;
            case Token::SubClass::LeftBracket:
                ident = Node::PNode_t(new ArrayIndex(ident, parseExpr()));
                expect(Token::SubClass::RightBracket);
                break;
            case Token::SubClass::LeftParenthesis:
                ident = Node::PNode_t(new FunctionCall(ident, parseArgs()));
                expect(Token::SubClass::RightParenthesis);
                break;
            default:
                break;
        }
        next();
    }
    return ident;
};

std::vector<Node::PNode_t> Parser::parseArgs() {
    std::vector<Node::PNode_t> args; 
    if (current()._subClass != Token::SubClass::RightParenthesis)
        do {
            args.push_back(parseExpr());
            if (current()._subClass != Token::SubClass::Comma)
                break;
            next();
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
    expect(current(), expected);
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