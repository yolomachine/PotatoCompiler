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

const std::set<Node::Type> Parser::_reducibleScalarTypes = {
    Node::Type::Integer,
    Node::Type::Float,
    Node::Type::Char,
};

const Parser::NodeTypesDict_t Parser::_nodeTypes = {
    { Node::Type::Identifier,      "Identifier"      },
    { Node::Type::ConstIdentifier, "ConstIdentifier" },
    { Node::Type::Integer,         "Integer"         },
    { Node::Type::Float,           "Real"            },
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

const Parser::SubClassTypeDict_t Parser::_subClassNodeTypes = {
    { Token::SubClass::IntConst,   Node::Type::IntConst   },
    { Token::SubClass::FloatConst, Node::Type::FloatConst },
    { Token::SubClass::String,     Node::Type::String     },
    { Token::SubClass::Array,      Node::Type::Array      },
    { Token::SubClass::Record,     Node::Type::Record     },
};

const Parser::OrdinalInitializersDict_t Parser::_ordinalInitializers = {
    { Node::Type::Integer,     Node::Type::IntConst      },
    { Node::Type::IntConst,    Node::Type::IntConst      },
    { Node::Type::Subrange,    Node::Type::IntConst      },
    { Node::Type::Float,       Node::Type::FloatConst    },
    { Node::Type::FloatConst,  Node::Type::FloatConst    },
    { Node::Type::String,      Node::Type::StringLiteral },
    { Node::Type::Char,        Node::Type::CharConst     },
    { Node::Type::CharConst,   Node::Type::CharConst     },
};

const Parser::IdentifierTypeDict_t Parser::_identifierNodeTypes = {
    { "integer", Node::Type::Integer },
    { "char",    Node::Type::Char    },
    { "real",    Node::Type::Float   },
};

const Parser::DeclarationsKeywordsSet_t Parser::_declKeywords = {
    Token::SubClass::Var,
    Token::SubClass::Type,
    Token::SubClass::Const,
    Token::SubClass::Procedure,
    Token::SubClass::Function
};

Parser::Parser(const char* filename) {
    open(filename);
};

Node::PNode_t Parser::parseProgram() {
    _funcIdentifiersTable = std::make_shared<std::set<std::string>>();
    _symTables = std::make_shared<VecPSymTable_t>();
    _typeAliases = std::make_shared<Node::SymTable_t>();
    Node::PNode_t program = parseProgramHeading();
    if (_declKeywords.count(_lexicalAnalyzer->currentToken()._subClass))
        program->addChild(parseDeclaration());
    expect(Token::SubClass::Begin);
    _lexicalAnalyzer->nextToken();
    if (_lexicalAnalyzer->currentToken()._subClass != Token::SubClass::End)
        program->addChild(parseStatement());
    expect(Token::SubClass::End);
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Dot);
    return program;
};

Node::PNode_t Parser::parseProgramHeading() {
    Node::PNode_t heading;
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Program);
    heading = std::make_shared<Node>(Node::Type::Program, _lexicalAnalyzer->currentToken());
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
    heading->addChild(std::make_shared<Identifier>(_lexicalAnalyzer->currentToken()));
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Semicolon);
    _lexicalAnalyzer->nextToken();
    return heading;
};

Node::PNode_t Parser::parseDeclaration() {
    Node::VecPNode_t declarations;
    while (true) {
        Token t = _lexicalAnalyzer->currentToken();
        _symTables->push_back(std::make_shared<Node::SymTable_t>());
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
            _lexicalAnalyzer->nextToken();
            declarations.push_back(parseConstDecl(t));
            break;
        case Token::SubClass::Procedure:
            _lexicalAnalyzer->nextToken();
            declarations.push_back(parseProcDecl(t));
            break;
        case Token::SubClass::Function:
            _lexicalAnalyzer->nextToken();
            declarations.push_back(parseFuncDecl(t));
            break;
        default:
            _symTables->pop_back();
            return std::make_shared<DeclarationsBlock>(declarations);
        }
    }
};

// la patte
Node::PNode_t Parser::parseStatement() {
    Node::VecPNode_t statements;
    while (_lexicalAnalyzer->currentToken()._subClass != Token::SubClass::End) {
        Token t = _lexicalAnalyzer->currentToken();
        Node::PNode_t expr = parseExpr();
        if (expr->_type == Node::Type::Identifier && _lexicalAnalyzer->currentToken()._subClass == Token::SubClass::Assign) {
            //checkExpr(expr);
            Token op = _lexicalAnalyzer->currentToken();
            std::dynamic_pointer_cast<Identifier>(expr)->isAssignment = true;
            _lexicalAnalyzer->nextToken();
            Node::PNode_t assignmentExpr = parseExpr();
            //validateAndReturnExprType(assignmentExpr);
            //checkExpr(assignmentExpr);
            validateAssignment(expr, assignmentExpr);
            statements.push_back(std::make_shared<BinaryOperator>(op, expr, assignmentExpr));
        }
        if (expr->_type == Node::Type::FunctionCall) {
            checkExpr(expr);
            // TO DO: Add reserved words map, totally forgot they exist
            if (expr->_children.front()->toString() == "writeln")
                statements.push_back(std::make_shared<WriteLn>(expr->_children.front()->_token, expr->_children.back()));
            else if (expr->_children.front()->toString() == "write")
                statements.push_back(std::make_shared<Write>(expr->_children.front()->_token, expr->_children.back()));
        };
        if (t._subClass == Token::SubClass::For) {
            expect(Token::SubClass::Identifier);
            Node::PNode_t controlVar = std::make_shared<Identifier>(_lexicalAnalyzer->currentToken());
            checkExpr(controlVar);
            expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Assign);
            _lexicalAnalyzer->nextToken();
            Node::PNode_t initial = parseExpr();
            checkExpr(initial);
            Node::PNode_t to_downto;
            if (_lexicalAnalyzer->currentToken().toString() == "to")
                to_downto = std::make_shared<To>(_lexicalAnalyzer->currentToken());
            else if (_lexicalAnalyzer->currentToken().toString() == "downto")
                to_downto = std::make_shared<DownTo>(_lexicalAnalyzer->currentToken());
            _lexicalAnalyzer->nextToken();
            Node::PNode_t final = parseExpr();
            checkExpr(final);
            // Just end my misery
            expect(Token::SubClass::Do);
            expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Begin);
            _lexicalAnalyzer->nextToken();
            Node::PNode_t body = parseStatement();
            expect(Token::SubClass::End);
            _lexicalAnalyzer->nextToken();
            Node::PNode_t forLoop = std::make_shared<For>(t, initial, to_downto, final, body);
            controlVar->addChild(initial);
            controlVar->addChild(final);
            forLoop->addChild(controlVar);
            std::swap(forLoop->_children.back(), forLoop->_children.front());
            statements.push_back(forLoop);
        };
        if (t._subClass == Token::SubClass::If) {
            Node::PNode_t elseBranch = std::make_shared<Node>(Node::Type::StatementBlock);
            Node::PNode_t condition = parseExpr();
            checkExpr(condition);
            expect(Token::SubClass::Then);
            expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Begin);
            _lexicalAnalyzer->nextToken();
            Node::PNode_t thenBranch = parseStatement();
            expect(Token::SubClass::End);
            _lexicalAnalyzer->nextToken();
            // I deserve to burn in hell
            if (_lexicalAnalyzer->currentToken()._subClass == Token::SubClass::Else) {
                expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Begin);
                _lexicalAnalyzer->nextToken();
                elseBranch = parseStatement();
                expect(Token::SubClass::End);
                _lexicalAnalyzer->nextToken();
            };
            Node::PNode_t ifStatement = std::make_shared<If>(t, condition, thenBranch, elseBranch);
            ifStatement->addChild(condition);
            std::swap(ifStatement->_children.back(), ifStatement->_children.front());
            statements.push_back(ifStatement);
        };
        expect(Token::SubClass::Semicolon);
        _lexicalAnalyzer->nextToken();
    };
    return std::make_shared<StatementsBlock>(statements);
};

Node::VecPNode_t Parser::parseDeclarations(Token::SubClass separator, bool restrictedInitialization, bool isLocal, bool isParamList) {
    Node::VecPNode_t declarations;
    if (isParamList && _lexicalAnalyzer->currentToken()._subClass == Token::SubClass::RightParenthesis) {
        _lexicalAnalyzer->nextToken();
        return Node::VecPNode_t({});
    };
    expect(Token::SubClass::Identifier);

    if (isLocal)
        _symTables->push_back(std::make_shared<Node::SymTable_t>());

    do {
    paramListLabel:
        Node::Type identifierType;
        Node::VecPNode_t identifiers;
        if (separator == Token::SubClass::Colon) {
            identifiers = parseIdentifierList();
            identifierType = Node::Type::Identifier;
        }
        else if (separator == Token::SubClass::Equal) {
            identifiers.push_back(parseScalarIdentifier());
            identifiers.back().get()->_type = identifierType = Node::Type::TypeAliasIdentifier;
        };
        expect(separator);
        _lexicalAnalyzer->nextToken();
        Node::PNode_t type = std::make_shared<TypeNode>(parseType(), identifierType);
        for (auto i : identifiers) {
            isLocal ? checkDuplicity(i->_token, _symTables->back()) : checkDuplicity(i->_token);
            declarations.push_back(std::make_shared<ParentNode>(identifierType, i->_token, type));
            _symTables->back()->insert({ i->toString(), std::make_pair(type, nullptr) });
            if (separator == Token::SubClass::Equal)
                _typeAliases->insert({ i->toString(), std::make_pair(type, nullptr) });
        };

        if (!restrictedInitialization && _lexicalAnalyzer->currentToken()._subClass == Token::SubClass::Equal)
            if (identifiers.size() > 1)
                throwException(_lexicalAnalyzer->currentToken()._pos, "Can't initialize more than one variable");
            else {
                _lexicalAnalyzer->nextToken();
                Node::PNode_t value = std::make_shared<ValueNode>(parseInitialization(type));
                declarations.back()->addChild(value);
                _symTables->back()->operator[](identifiers.back()->toString()).second = value;
            };

        if (!isParamList) 
            expect(Token::SubClass::Semicolon);
        else if (_lexicalAnalyzer->currentToken()._subClass != Token::SubClass::Semicolon)
            expect(Token::SubClass::RightParenthesis);
        else {
            expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
            goto paramListLabel;
        }
    } while (_lexicalAnalyzer->nextToken()._subClass == Token::SubClass::Identifier);
    return declarations;
};

Node::PNode_t Parser::parseVarDecl(Token t) {
    return std::make_shared<Declaration>(Node::Type::VarDecl, t, parseDeclarations(Token::SubClass::Colon));
};

Node::PNode_t Parser::parseTypeDecl(Token t) {
    Node::VecPNode_t declarations = parseDeclarations(Token::SubClass::Equal, true);
    for (auto i : *_symTables->back().get())
        _typeAliases->operator[](i.first) = i.second;
    return std::make_shared<Declaration>(Node::Type::TypeDecl, t, declarations);
};

Node::PNode_t Parser::parseProcDecl(Token t) {
    return std::make_shared<Declaration>(Node::Type::ProcDecl, t, parseProcedure());
}

Node::PNode_t Parser::parseFuncDecl(Token t) {
    return std::make_shared<Declaration>(Node::Type::FuncDecl, t, parseFunction());
}

Node::PNode_t Parser::parseConstDecl(Token t) {
    Token next, identifier = _lexicalAnalyzer->currentToken();
    Node::PNode_t type, value, expr;
    Node::VecPNode_t constants;

    do {
        switch ((next = _lexicalAnalyzer->nextToken())._subClass) {
        case Token::SubClass::Colon:
            _lexicalAnalyzer->nextToken();
            type = std::make_shared<TypeNode>(parseType(), Node::Type::ConstIdentifier);
            expect(Token::SubClass::Equal);
            _lexicalAnalyzer->nextToken();
            value = std::make_shared<ValueNode>(parseInitialization(type));
            break;
        case Token::SubClass::Equal:
            _lexicalAnalyzer->nextToken();
            expr = parseConstExpr();
            type = std::make_shared<TypeNode>(defineConstType(validateAndReturnExprType(expr)), Node::Type::ConstIdentifier);
            value = std::make_shared<ValueNode>(Node::VecPNode_t({ expr }));
            break;
        default:
            throwException(next._pos, "What a terrible failure");
            break;
        };

        expect(Token::SubClass::Semicolon);
        checkDuplicity(identifier);
        constants.push_back(std::make_shared<ParentNode>(Node::Type::ConstIdentifier, identifier, type, value));
        _symTables->back()->insert({ identifier.toString(), std::make_pair(type, value) });
    } while ((identifier = _lexicalAnalyzer->nextToken())._subClass == Token::SubClass::Identifier);
    return std::make_shared<Declaration>(Node::Type::ConstDecl, t, constants);
};

Node::PNode_t Parser::parseConstExpr() {
    Token t = _lexicalAnalyzer->currentToken();
    Node::PNode_t expr = parseExpr();
    Node::Type exprType = validateAndReturnExprType(expr);
    if (!_reducibleScalarTypes.count(exprType))
        throwException(t._pos, "Scalar type expected");
    checkIfExprIsConst(expr);
    return expr;
};

void Parser::checkIfExprIsConst(Node::PNode_t expr) {
    if (expr->_type == Node::Type::BinaryOperator ||
        expr->_type == Node::Type::UnaryOperator ||
        expr->_type == Node::Type::IntConst ||
        expr->_type == Node::Type::FloatConst ||
        expr->_type == Node::Type::CharConst ||
        (findSymbol(expr->toString()) &&
         findSymbol(expr->toString())->first->_children.front()->_type == Node::Type::ConstIdentifier))
        for (auto i : expr->_children)
            checkIfExprIsConst(i);
    else
        throwException(expr->_token._pos, "Const identifier or expression expected: \"" + expr->toString() + "\"");
};

// la patte
Node::PNode_t Parser::parseFunction() {
    PVecPSymTable_t localSymTable = std::make_shared<VecPSymTable_t>();
    Node::PSymTable_t localTypeAliases = std::make_shared<Node::SymTable_t>(*_typeAliases.get());
    localSymTable->push_back(std::make_shared<Node::SymTable_t>());
    std::swap(localSymTable, _symTables);
    std::swap(localTypeAliases, _typeAliases);

    Token identifier = _lexicalAnalyzer->currentToken();
    if (_funcIdentifiersTable->count(identifier.toString()))
        throwException(identifier._pos, "Duplicate identifier \"" + identifier.toString() + "\"");
    else 
        _funcIdentifiersTable->insert(identifier.toString());
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::LeftParenthesis);
    _lexicalAnalyzer->nextToken();

    Node::PNode_t params = std::make_shared<ParameterList>(parseDeclarations(Token::SubClass::Colon, false, false, true));
    expect(Token::SubClass::Colon);
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);

    Node::PNode_t type = std::make_shared<TypeNode>(parseType(), Node::Type::Type, "return_type");
    expect(Token::SubClass::Semicolon);
    _lexicalAnalyzer->nextToken();

    Node::VecPNode_t nodes = std::vector<Node::PNode_t>({ params, type });
    if (_declKeywords.count(_lexicalAnalyzer->currentToken()._subClass))
        nodes.push_back(parseDeclaration());

    expect(Token::SubClass::Begin);
    _lexicalAnalyzer->nextToken();
    nodes.push_back(parseStatement());
    expect(Token::SubClass::End);
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Semicolon);
    _lexicalAnalyzer->nextToken();

    Node::PNode_t result = std::make_shared<Function>(identifier, nodes, params, type, _symTables);
    std::swap(localSymTable, _symTables);
    std::swap(localTypeAliases, _typeAliases);
    _symTables->back()->insert({ identifier.toString(), { type, result } });
    return result;
};

// la patte
Node::PNode_t Parser::parseProcedure() {
    PVecPSymTable_t localSymTable = std::make_shared<VecPSymTable_t>();
    Node::PSymTable_t localTypeAliases = std::make_shared<Node::SymTable_t>(*_typeAliases.get());
    localSymTable->push_back(std::make_shared<Node::SymTable_t>());
    std::swap(localSymTable, _symTables);
    std::swap(localTypeAliases, _typeAliases);

    Token identifier = _lexicalAnalyzer->currentToken();
    if (_funcIdentifiersTable->count(identifier.toString()))
        throwException(identifier._pos, "Duplicate identifier \"" + identifier.toString() + "\"");
    else
        _funcIdentifiersTable->insert(identifier.toString());
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::LeftParenthesis);
    _lexicalAnalyzer->nextToken(); 

    Node::PNode_t params = std::make_shared<ParameterList>(parseDeclarations(Token::SubClass::Colon, false, false, true));
    expect(Token::SubClass::Semicolon);
    _lexicalAnalyzer->nextToken();

    Node::VecPNode_t nodes = std::vector<Node::PNode_t>({ params });
    if (_declKeywords.count(_lexicalAnalyzer->currentToken()._subClass))
        nodes.push_back(parseDeclaration());

    expect(Token::SubClass::Begin);
    _lexicalAnalyzer->nextToken();
    nodes.push_back(parseStatement());
    expect(Token::SubClass::End);
    expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Semicolon);
    _lexicalAnalyzer->nextToken();

    std::shared_ptr<Procedure> result = std::make_shared<Procedure>(identifier, nodes, params, _symTables);
    std::swap(localSymTable, _symTables);
    std::swap(localTypeAliases, _typeAliases);
    _symTables->back()->insert({ identifier.toString(), { nullptr, result } });
    return result;
};

Node::PNode_t Parser::parseType() {
    Token t;
    Token current = _lexicalAnalyzer->currentToken();
    Token next = _lexicalAnalyzer->nextToken();
    Node::Type type = defineNodeType(current);
    Node::PNode_t left, right, result;
    Node::VecPNode_t fields;

    switch (type) {
    case Node::Type::Integer:
    case Node::Type::Float:
    case Node::Type::Char:
    case Node::Type::String:
        if (next._subClass == Token::SubClass::Subrange)
            throwException(current._pos, "Error in type definition");
        return std::make_shared<Node>(type, current);
    case Node::Type::TypeAliasIdentifier:
        return findSymbol(current._value.s, _typeAliases)->first->_children.back();
        //without sym table
        //if (next._subClass != Token::SubClass::Subrange)
        //    return std::make_shared<TypeAlias>(current);
    case Node::Type::ConstIdentifier:
        expect(findSymbol(current._value.s)->second->_children.back()->_token, Token::SubClass::IntConst);
    case Node::Type::IntConst:
        left = std::make_shared<Node>(type, current);
        if (type == Node::Type::ConstIdentifier) {
            left->addChild(findSymbol(current._value.s)->first);
            left->addChild(findSymbol(current._value.s)->second);
        };
        expect(t = next, Token::SubClass::Subrange);
        next = _lexicalAnalyzer->nextToken();
        if (next._subClass == Token::SubClass::IntConst)
            right = std::make_shared<IntConst>(next);
        else if (defineNodeType(next) == Node::Type::ConstIdentifier) {
            right = std::make_shared<Node>(Node::Type::ConstIdentifier, next);
            right->addChild(findSymbol(next._value.s)->first);
            right->addChild(findSymbol(next._value.s)->second);
        }
        //without semantics
        //else if (next._subClass == Token::SubClass::Identifier && _identifierNodeTypes.count(next._value.s) == 0)
        //    right = std::make_shared<TypeAlias>(next);
        else
            throwException(next._pos, "Error in type definition");
        result = std::make_shared<Subrange>(t, left, right);
        _lexicalAnalyzer->nextToken();
        break;
    case Node::Type::Record:
        fields = parseDeclarations(Token::SubClass::Colon, true, true);
        result = std::make_shared<Record>(current, fields, _symTables->back());
        _symTables->pop_back();
        expect(Token::SubClass::End);
        _lexicalAnalyzer->nextToken();
        break;
    case Node::Type::Array:
        result = std::make_shared<Node>(type, current);
        expect(next, Token::SubClass::LeftBracket);
        _lexicalAnalyzer->nextToken();
        result->addChild(parseType());
        expect(result->_children.back()->_token, Token::SubClass::Subrange);
        expect(Token::SubClass::RightBracket);
        expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Of);
        _lexicalAnalyzer->nextToken();
        result->addChild(std::make_shared<TypeNode>(parseType(), Node::Type::Type));
        break;
    }
        return result;
};

Node::VecPNode_t Parser::parseInitialization(Node::PNode_t typeNode) {
    Node::PNode_t result, type = typeNode->_children.back();
    Node::VecPNode_t values, nodes;
    std::map<Node::PNode_t, bool> initialized;
    uint64_t lowerBound, upperBound;

    Node::Type currentType = type->_type;
    switch (currentType) {
    case Node::Type::Integer:
    case Node::Type::IntConst:
    case Node::Type::Subrange:
    case Node::Type::Float:
    case Node::Type::FloatConst:
    case Node::Type::String:
    case Node::Type::Char:
    case Node::Type::CharConst:
        checkExprType(result = parseExpr(), _ordinalInitializers.at(currentType));
        values.push_back(result);
        break;
    case Node::Type::Array:
        lowerBound = std::dynamic_pointer_cast<Subrange>(type->_children.front())->_lowerBound;
        upperBound = std::dynamic_pointer_cast<Subrange>(type->_children.front())->_upperBound;
        expect(Token::SubClass::LeftParenthesis);
        for (uint64_t i = lowerBound; i <= upperBound; ++i) {
            _lexicalAnalyzer->nextToken();
            for (auto j : parseInitialization(type->_children.back()))
                nodes.push_back(j);
            if (i < upperBound)
                expect(Token::SubClass::Comma);
        };
        expect(Token::SubClass::RightParenthesis);
        values.push_back(std::make_shared<ValueNode>(nodes, "array"));
        _lexicalAnalyzer->nextToken();
        break;
    case Node::Type::Record:
        expect(Token::SubClass::LeftParenthesis);
        for (auto i : type->_children) {
            expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Identifier);
            if (!std::strcmp(_lexicalAnalyzer->currentToken()._value.s, i->_token._value.s)) {
                expect(_lexicalAnalyzer->nextToken(), Token::SubClass::Colon);
                _lexicalAnalyzer->nextToken();
                values.push_back(std::make_shared<ParentNode>(Node::Type::Identifier, i->_token, parseInitialization(i->_children.back())));
                initialized.insert({i, true});
            }
            else {
                for (auto j : type->_children)
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

Node::Type Parser::defineNodeType(Token t) {
    if (t._subClass == Token::SubClass::Identifier)
        if (_identifierNodeTypes.count(t._value.s) && !findSymbol(t._value.s))
            return _identifierNodeTypes.at(t._value.s);
        //without sym table
        //else return Node::Type::TypeAliasIdentifier;
        else if (findSymbol(t._value.s) && 
                 std::dynamic_pointer_cast<TypeNode>(findSymbol(t._value.s)->first)->isConst())
            return Node::Type::ConstIdentifier;
        else if (findSymbol(t._value.s, _typeAliases) && 
                 std::dynamic_pointer_cast<TypeNode>(findSymbol(t._value.s, _typeAliases)->first)->isTypeAlias())
            return Node::Type::TypeAliasIdentifier;
    if (_subClassNodeTypes.count(t._subClass))
        return _subClassNodeTypes.at(t._subClass);
    throwException(t._pos, "Error in type definition");
};

Node::PNode_t Parser::defineConstType(Node::Type type) {
    switch (type) {
    case Node::Type::Float:
        return std::make_shared<NamedNode>(Node::Type::FloatConst, "real");
    case Node::Type::Char:
        //return t.toString().length() > 1 ?
            //std::make_shared<NamedNode>(Node::Type::StringLiteral, "string") :
            std::make_shared<NamedNode>(Node::Type::CharConst, "char");
    case Node::Type::Integer:
        return std::make_shared<NamedNode>(Node::Type::IntConst, "integer");
    default:
        break;
    }
};

Node::PNode_t Parser::defineConstType(Token t) {
    switch (t._vtype) {
    case Token::ValueType::Double:
        return std::make_shared<NamedNode>(Node::Type::FloatConst, "real");
    case Token::ValueType::String:
        return t.toString().length() > 1 ?
               std::make_shared<NamedNode>(Node::Type::StringLiteral, "string") :
               std::make_shared<NamedNode>(Node::Type::CharConst, "char");
    case Token::ValueType::ULL:
        return std::make_shared<NamedNode>(Node::Type::IntConst, "integer");
    default:
        throwException(t._pos, "Error in const definition");
    }
};

Parser::PNodePair_t* Parser::findSymbol(std::string name) {
    for (auto it = _symTables->rbegin(); it != _symTables->rend(); ++it)
        if (it->get()->count(name))
            return &(it->get()->at(name));
    return nullptr;
};

Parser::PNodePair_t* Parser::findSymbol(std::string name, Node::PSymTable_t symTable) {
    if (symTable->count(name))
        return &(symTable->at(name));
    return nullptr;
};

Node::PNode_t Parser::parseScalarIdentifier() {
    expect(Token::SubClass::Identifier);
    Node::PNode_t node(new Identifier(_lexicalAnalyzer->currentToken()));
    _lexicalAnalyzer->nextToken();
    return node;
};

Node::VecPNode_t Parser::parseIdentifierList() {
    Node::VecPNode_t identifiers;
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
        left = std::make_shared<BinaryOperator>(t, left, (this->*pf)());
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
        return std::make_shared<UnaryOperator>(t, parseExpr());
    switch (t._subClass) {
    case Token::SubClass::Identifier:
        return parseIdentifier(t);
        break;
    case Token::SubClass::IntConst:
        return std::make_shared<IntConst>(t);
        break;
    case Token::SubClass::FloatConst:
        return std::make_shared<FloatConst>(t);
        break;
    case Token::SubClass::StringLiteral:
        if (t._raw.length() == 3)
            return std::make_shared<CharConst>(t);
        else
            return std::make_shared<StringLiteral>(t);        
        break;
    case Token::SubClass::LeftParenthesis:
        e = parseExpr();
        expect(Token::SubClass::RightParenthesis);
        _lexicalAnalyzer->nextToken();
        return e;
        break;
    case Token::SubClass::For:
    case Token::SubClass::To:
    case Token::SubClass::DownTo:
    case Token::SubClass::Do:
    case Token::SubClass::If:
    case Token::SubClass::Then:
    case Token::SubClass::Else:
        return std::make_shared<ReservedWord>(t);
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
    Node::VecPNode_t args;
    Node::PNode_t identifier = std::make_shared<Identifier>(t);
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
                identifier = std::make_shared<RecordAccess>(identifier, parseScalarIdentifier());
                break;
            case Token::SubClass::LeftBracket:
                identifier = std::make_shared<ArrayIndex>(identifier, parseExpr());
                expect(Token::SubClass::RightBracket);
                _lexicalAnalyzer->nextToken();
                break;
            case Token::SubClass::LeftParenthesis:
                if (identifier->_type != Node::Type::Identifier)
                    throwException(t._pos, "Illegal function call");
                identifier = std::make_shared<FunctionCall>(identifier, parseArgs());
                expect(Token::SubClass::RightParenthesis);
                _lexicalAnalyzer->nextToken();
                break;
            default:
                break;
        }
    }
    return identifier;
};

Node::VecPNode_t Parser::parseArgs() {
    Node::VecPNode_t args; 
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
    int offset;
    //for (auto i : *_symTables.get())
    //    for (auto j : *i.get()) {
    //        if (!std::dynamic_pointer_cast<TypeNode>(j.second.first)->isTypeAlias()) {
    //            offset = AsmCode::getTypeSize(j.second.first->_children.front());
    //            AsmCode::_offset += offset;
    //            AsmCode::_offsetMap[j.first] = { offset, AsmCode::_offset };
    //        }
    //    };
    //AsmCode::PAsmCommand setOffset = std::make_shared<AsmCommand>(AsmCommands::Enter, std::vector<std::string>({ std::to_string(AsmCode::_offset), "1" }));
    //AsmCode::addCommand(setOffset);
    //AsmCode::generateStatements(_root->_children.back());
    //AsmCode::generate(std::ofstream("code.asm"));
};

template<typename T>
void Parser::open(T filename) {
    _lexicalAnalyzer = std::make_shared<LexicalAnalyzer>(filename);
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

void Parser::checkExpr(Node::PNode_t expr) {
    if (expr->_type == Node::Type::Identifier) {
        if (expr->toString() != "write" && expr->toString() != "writeln")
            if (!findSymbol(expr->toString()))
                throwException(expr->_token._pos, "Identifier not found: \"" + expr->toString() + "\"");
            else if (!_funcIdentifiersTable->count(expr->toString())) {
                if (std::dynamic_pointer_cast<TypeNode>(findSymbol(expr->_token._value.s)->first)->isConst())
                    throwException(expr->_token._pos, "Can't modify constant values: \"" + expr->toString() + "\"");
                else if (std::dynamic_pointer_cast<TypeNode>(findSymbol(expr->_token._value.s)->first)->isTypeAlias())
                    throwException(expr->_token._pos, "Can't modify type aliases: \"" + expr->toString() + "\"");
            };
    };
    
    for (auto i : expr->_children)
        checkExpr(i);
};

Node::Type Parser::validateAndReturnExprType(Node::PNode_t expr) {
    if (expr->_type == Node::Type::BinaryOperator) {
        Node::PNode_t left = expr->_children.front();
        Node::PNode_t right = expr->_children.back();
        Node::Type leftType = validateAndReturnExprType(left);
        Node::Type rightType = validateAndReturnExprType(right);
        if (leftType == Node::Type::Char || rightType == Node::Type::Char)
            throwException(expr->_token._pos, "Can't apply operator \"" + expr->toString() + "\" to char");
        if (leftType == Node::Type::Array || rightType == Node::Type::Array)
            throwException(expr->_token._pos, "Can't apply operator \"" + expr->toString() + "\" to array");
        if (leftType == Node::Type::Record || rightType == Node::Type::Record)
            throwException(expr->_token._pos, "Can't apply operator \"" + expr->toString() + "\" to record");
        if (leftType == Node::Type::FunctionCall || rightType == Node::Type::FunctionCall)
            throwException(expr->_token._pos, "Can't use procedures in expressions");
        if (expr->_token._subClass == Token::SubClass::Add ||
            expr->_token._subClass == Token::SubClass::Sub ||
            expr->_token._subClass == Token::SubClass::Mult) {
            if (leftType == rightType)
                return leftType;
            else if ((leftType == Node::Type::Integer && rightType == Node::Type::Float) ||
                (leftType == Node::Type::Float && rightType == Node::Type::Integer))
                return Node::Type::Float;
        }
        else if (expr->_token._subClass == Token::SubClass::SHL ||
            expr->_token._subClass == Token::SubClass::SHR ||
            expr->_token._subClass == Token::SubClass::And ||
            expr->_token._subClass == Token::SubClass::Or ||
            expr->_token._subClass == Token::SubClass::Mod ||
            expr->_token._subClass == Token::SubClass::Div) {
            if ((leftType == Node::Type::Float) || (rightType == Node::Type::Integer) ||
                (leftType == Node::Type::Char) || (rightType == Node::Type::Char))
                throwException(expr->_token._pos, "Can't apply operator \"" + expr->toString() + "\" to other than integers");
            else if (expr->_token._subClass == Token::SubClass::Div)
                return Node::Type::Float;
            else
                return Node::Type::Integer;
        }
        else if (expr->_token._subClass == Token::SubClass::Equal ||
                 expr->_token._subClass == Token::SubClass::Less ||
                 expr->_token._subClass == Token::SubClass::More ||
                 expr->_token._subClass == Token::SubClass::LEQ ||
                 expr->_token._subClass == Token::SubClass::MEQ ||
                 expr->_token._subClass == Token::SubClass::NEQ) 
            if (leftType == Node::Type::Char || rightType == Node::Type::Char)
                throwException(expr->_token._pos, "Can't apply operator \"" + expr->toString() + "\" to chars");
            else
                return Node::Type::Integer;
    }
    else if (expr->_type == Node::Type::UnaryOperator)
        return validateAndReturnExprType(expr->_children.front());
    else if (expr->_type == Node::Type::FunctionCall) {
        if (!findSymbol(expr->_children.front()->toString()))
            throwException(expr->_children.front()->_token._pos, "Identifier not found: \"" + expr->_children.front()->toString() + "\"");
        else if (!_funcIdentifiersTable->count(expr->_children.front()->toString()))
            throwException(expr->_children.front()->_token._pos, "Identifier's not a function or a procedure: \"" + expr->_children.front()->toString() + "\"");
        if (findSymbol(expr->_children.front()->toString())->first) {
            Function* f = std::static_pointer_cast<Function>(findSymbol(expr->_children.front()->toString())->second).get();
            Node::VecPNode_t params = std::static_pointer_cast<ParameterList>(f->_paramList)->getParams();
            if (params.size() != expr->_children.size() - 1)
                throwException(expr->_children.front()->_token._pos, "Wrong amount of arguments in function call \"" + expr->_children.front()->toString() + "\"");
            //for (size_t i = 1; i < expr->_children.size(); ++i) {
            //    if (findSymbol(expr->_children[i]->toString())) /////////// validate
            //        validateNodeTypes(params[i - 1], findSymbol(expr->_children[i]->toString())->first->_children.front(), expr->_children[i]->_token._pos);
            //    else
            //        validateNodeTypes(params[i - 1], expr->_children[i], expr->_children[i]->_token._pos);
            //}
            return findSymbol(expr->_children.front()->toString())->first->_children.front()->_type;
        }
        else
            return Node::Type::FunctionCall;
    }
    else if (expr->_type == Node::Type::Identifier) {
        if (_funcIdentifiersTable->count(expr->toString()))
            throwException(expr->_token._pos, "Improper call of a function or a procedure: \"" + expr->toString() + "\"");
        return findSymbol(expr->toString())->first->_children.front()->_type;
    }
    else if (expr->_type == Node::Type::IntConst)
        return Node::Type::Integer;
    else if (expr->_type == Node::Type::FloatConst)
        return Node::Type::Float;
    else if (expr->_type == Node::Type::CharConst)
        return Node::Type::Char;
    else return expr->_type;
};

void Parser::validateNodeTypes(Node::PNode_t leftTypeNode, Node::PNode_t rightTypeNode, const Token::Position_t pos) {
    if (leftTypeNode->_type == rightTypeNode->_type) {
        if (leftTypeNode->_type == Node::Type::Subrange)
            if ((std::static_pointer_cast<Subrange>(leftTypeNode)->_lowerBound != 
                std::static_pointer_cast<Subrange>(rightTypeNode)->_lowerBound) ||
                (std::static_pointer_cast<Subrange>(leftTypeNode)->_upperBound !=
                 std::static_pointer_cast<Subrange>(rightTypeNode)->_upperBound))
            throwException(pos, "Incompatible types");
        if (leftTypeNode->_children.size() == rightTypeNode->_children.size())
            for (size_t i = 0; i < leftTypeNode->_children.size(); ++i)
                validateNodeTypes(leftTypeNode->_children[i], rightTypeNode->_children[i], pos);
        else
            throwException(pos, "Incompatible types");
    }
    else
        throwException(pos, "Incompatible types");
};

void Parser::validateAssignment(Node::PNode_t left, Node::PNode_t right) {
    checkExpr(left);
    checkExpr(right);
    Node::Type leftType = validateAndReturnExprType(left);
    Node::Type rightType = validateAndReturnExprType(right);
    if ((_reducibleScalarTypes.count(leftType) && !_reducibleScalarTypes.count(rightType)) ||
        (!_reducibleScalarTypes.count(leftType) && _reducibleScalarTypes.count(rightType)))
        throwException(right->_token._pos, "Can't assign operand of this type");
    else if (!_reducibleScalarTypes.count(leftType) && !_reducibleScalarTypes.count(rightType))
        validateNodeTypes(findSymbol(left->toString())->first, findSymbol(right->toString())->first, right->_token._pos);
};

void Parser::checkDuplicity(Token t) {
    if (findSymbol(t.toString())) 
        throwException(t._pos, "Duplicate identifier \"" + t.toString() + "\"");
};

void Parser::checkDuplicity(Token t, Node::PSymTable_t symTable) {
    if (findSymbol(t.toString(), symTable))
        throwException(t._pos, "Duplicate identifier \"" + t.toString() + "\"");
};

void Parser::throwException(Token::Position_t pos, std::string msg) {
    std::stringstream ss;
    ss << "(" << pos.first << ", " << pos.second << "): " << msg.c_str();
    throw std::exception(ss.str().c_str());
};