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

Parser::Parser(const char* filename) : _root(Node::PNode(new Node(Node::Type::Root))) {
    open(filename);
};

Node::PNode Parser::parseExpr() {
    Node::PNode binOp, leftSimple = parseSimpleExpr();
    Token t = _lexicalAnalyzer->currentToken();
    while (checkPrecedence(Precedence::Last, t._subClass)) {
        _lexicalAnalyzer->nextToken();
        Node::PNode rightSimple = parseSimpleExpr();
        binOp = Node::PNode(new Node(Node::Type::BinaryOperator, t));
        binOp->addChild(leftSimple);
        binOp->addChild(rightSimple);
        leftSimple = binOp;
        t = _lexicalAnalyzer->currentToken();
    }
    return leftSimple;
};
Node::PNode Parser::parseSimpleExpr() {
    Node::PNode binOp, leftTerm = parseTerm();
    Token t = _lexicalAnalyzer->currentToken();
    while (checkPrecedence(Precedence::Third, t._subClass)) {
        _lexicalAnalyzer->nextToken();
        Node::PNode rightTerm = parseTerm();
        binOp = Node::PNode(new Node(Node::Type::BinaryOperator, t));
        binOp->addChild(leftTerm);
        binOp->addChild(rightTerm);
        leftTerm = binOp;
        t = _lexicalAnalyzer->currentToken();
    }
    return leftTerm;
};
Node::PNode Parser::parseTerm() {
    Node::PNode binOp, leftFactor = parseFactor();
    Token t = _lexicalAnalyzer->currentToken();
    while (checkPrecedence(Precedence::Second, t._subClass)) {
        _lexicalAnalyzer->nextToken();
        Node::PNode rightFactor = parseFactor();
        binOp = Node::PNode(new Node(Node::Type::BinaryOperator, t));
        binOp->addChild(leftFactor);
        binOp->addChild(rightFactor);
        leftFactor = binOp;
        t = _lexicalAnalyzer->currentToken();
    }
    return leftFactor;
};

Node::PNode Parser::parseFactor() {
    Node::PNode e;
    Token t = _lexicalAnalyzer->currentToken();
    if (checkPrecedence(Parser::Precedence::First, t._subClass))
        return Node::PNode(new Node(Node::Type::UnaryOperator, t));
    switch (t._subClass) {
    case Token::SubClass::EndOfFile:
        throwException(t._pos, "Unexpected end of file");
        break;
    case Token::SubClass::Identifier:
        return parseIdentifier();
        break;
    case Token::SubClass::IntConst:
        _lexicalAnalyzer->nextToken();
        return Node::PNode(new Node(Node::Type::IntConst, t));
        break;
    case Token::SubClass::FloatConst:
        _lexicalAnalyzer->nextToken();
        return Node::PNode(new Node(Node::Type::FloatConst, t));
        break;
    case Token::SubClass::StringLiteral:
        _lexicalAnalyzer->nextToken();
        return Node::PNode(new Node(Node::Type::StringLiteral, t));
        break;
    case Token::SubClass::LeftParenthesis:
        _lexicalAnalyzer->nextToken();
        e = parseExpr();
        expect(_lexicalAnalyzer->currentToken()._pos, _lexicalAnalyzer->currentToken()._subClass, Token::SubClass::RightParenthesis);
        _lexicalAnalyzer->nextToken();
        return e;
        break;
    default:
        throwException(t._pos, "Illegal expression");
        break;
    }
};

Node::PNode Parser::parseIdentifier() {
    Token t = _lexicalAnalyzer->currentToken();
    _lexicalAnalyzer->nextToken();
    return Node::PNode(new Node(Node::Type::Identifier, t));
};

void Parser::buildTree() {
    _lexicalAnalyzer->nextToken();
    _root->addChild(parseExpr());
};

template<typename T>
void Parser::open(T filename) {
    _lexicalAnalyzer = Parser::PLexicalAnalyzer(new LexicalAnalyzer(filename));
};

void Parser::log(std::ostream& os) {

};

void Parser::expect(Token::Position_t pos, Token::SubClass received, Token::SubClass expected) {
    std::stringstream ss;
    if (expected != received) {
        ss << "Syntax error, " 
           << _lexicalAnalyzer->_subClassDict.at(expected)
           << " expected, but " 
           << _lexicalAnalyzer->_subClassDict.at(received) 
           << " found";
        throwException(pos, ss.str());
    }
};

bool Parser::checkPrecedence(Parser::Precedence p, Token::SubClass s) {
    return _precedences.at(static_cast<int>(p)).count(s);
}

void Parser::throwException(Token::Position_t pos, std::string msg) {
    std::stringstream ss;
    ss << "(" << pos.first << ", " << pos.second << "): " << msg.c_str();
    throw std::exception(ss.str().c_str());
}