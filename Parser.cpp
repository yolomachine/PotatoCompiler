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

Node::PNode_t Parser::parseBinOp(Parser::Precedence p, Parser::PFunction_t pf) {
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
        expect(_lexicalAnalyzer->currentToken(), Token::SubClass::RightParenthesis);
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
    std::set<Token::SubClass> allowed = { Token::SubClass::Dot, Token::SubClass::LeftBracket, Token::SubClass::LeftParenthesis };
    while (allowed.count(_lexicalAnalyzer->currentToken()._subClass)) {
        Token t = _lexicalAnalyzer->currentToken();
        _lexicalAnalyzer->nextToken();
        switch (t._subClass) {
            case Token::SubClass::Dot:
                expect(_lexicalAnalyzer->currentToken(), Token::SubClass::Identifier);
                ident = Node::PNode_t(new RecordAccess(ident, Node::PNode_t(new Identifier(_lexicalAnalyzer->currentToken()))));
                break;
            case Token::SubClass::LeftBracket:
                ident = Node::PNode_t(new ArrayIndex(ident, parseExpr()));
                expect(_lexicalAnalyzer->currentToken(), Token::SubClass::RightBracket);
                break;
            case Token::SubClass::LeftParenthesis:
                ident = Node::PNode_t(new FunctionCall(ident, parseArgs()));
                expect(_lexicalAnalyzer->currentToken(), Token::SubClass::RightParenthesis);
                break;
            default:
                break;
        }
        _lexicalAnalyzer->nextToken();
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
    _lexicalAnalyzer->nextToken();
    _root = parseExpr();
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
    return _precedences.at(static_cast<int>(p)).count(s);
}

void Parser::throwException(Token::Position_t pos, std::string msg) {
    std::stringstream ss;
    ss << "(" << pos.first << ", " << pos.second << "): " << msg.c_str();
    throw std::exception(ss.str().c_str());
}