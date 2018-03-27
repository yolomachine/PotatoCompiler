#include "Node.hpp"

void Node::addChild(PNode_t pnode) {
    _children.push_back(pnode);
};

std::string Node::toString() {
    return _token.toString();
};

void Node::generate() {

};

NamedNode::NamedNode(Type type, std::string name) : 
    Node(type), _name(name) {};
AtomicNode::AtomicNode(Type type, Token token) : 
    Node(type, token) {};

IntConst::IntConst(Token t) : 
    AtomicNode(Type::IntConst, t) {};
FloatConst::FloatConst(Token t) : 
    AtomicNode(Type::FloatConst, t) {};
Identifier::Identifier(Token t) : 
    AtomicNode(Type::Identifier, t), isAssignment(false) {};
CharConst::CharConst(Token t) : 
    AtomicNode(Type::CharConst, t) {};
StringLiteral::StringLiteral(Token t) : 
    AtomicNode(Type::StringLiteral, t) {};
TypeAlias::TypeAlias(Token t) : 
    AtomicNode(Type::TypeAlias, t) {};

ParentNode::ParentNode(Type type, Token token, Node::PNode_t child) : Node(type, token) {
    addChild(child);
};

ParentNode::ParentNode(Type type, Token token, Node::PNode_t left, Node::PNode_t right) : Node(type, token) {
    addChild(left);
    addChild(right);
};

ParentNode::ParentNode(Type type, Token token, Node::VecPNode_t children) : Node(type, token) {
    addChildren(children);
}

ParentNode::ParentNode(Type type, Node::VecPNode_t children) : Node(type) {
    addChildren(children);
};

ParentNode::ParentNode(Type type, Node::PNode_t left, Node::VecPNode_t right) : Node(type) {
    addChild(left);
    addChildren(right);
};

void ParentNode::addChildren(Node::VecPNode_t children) {
    for (auto i : children)
        addChild(i);
}

DeclarationsBlock::DeclarationsBlock(Node::VecPNode_t declarations) : 
    ParentNode(Type::DeclarationBlock, declarations), _name("declarations") {};
StatementsBlock::StatementsBlock(Node::VecPNode_t statements) : 
    ParentNode(Type::StatementBlock, statements), _name("statements") {};
Declaration::Declaration(Type type, Token keyword, Node::VecPNode_t declarations) : 
    ParentNode(type, keyword, declarations) {};
Declaration::Declaration(Type type, Token keyword, Node::PNode_t declaration) : 
    ParentNode(type, keyword, declaration) {};
Record::Record(Token keyword, Node::VecPNode_t fields) : 
    Declaration(Type::Record, keyword, fields) {};
Record::Record(Token keyword, Node::VecPNode_t fields, Node::PSymTable_t symTable) : 
    Declaration(Type::Record, keyword, fields), _localSymTable(symTable) {};

Function::Function(Token name, Node::VecPNode_t children, Node::PNode_t params, Node::PNode_t type, Node::PVecPSymTable_t symTables) :
    Declaration(Type::Function, name, children), _paramList(params), _type(type), _localSymTables(symTables) {};
Procedure::Procedure(Token name, Node::VecPNode_t children, Node::PNode_t params, Node::PVecPSymTable_t symTables) : 
    Declaration(Type::Procedure, name, children), _paramList(params), _localSymTables(symTables) {};

UnaryOperator::UnaryOperator(Token op, Node::PNode_t expr) : 
    ParentNode(Type::UnaryOperator, op, expr) {};
BinaryOperator::BinaryOperator(Token op, PNode_t left, PNode_t right) : 
    ParentNode(Type::UnaryOperator, op, left, right) {};
Subrange::Subrange(Token op, Node::PNode_t lowerBound, Node::PNode_t upperBound) : 
    ParentNode(Type::Subrange, op, lowerBound, upperBound) {
    _lowerBound = lowerBound->_token._value.ull;
    _upperBound = upperBound->_token._value.ull;
};

AccessNode::AccessNode(Node::Type type, Node::PNode_t child, std::string name) : 
    ParentNode(type, Node::VecPNode_t{child}), _name(name) {};
AccessNode::AccessNode(Node::Type type, Node::VecPNode_t args, std::string name) : 
    ParentNode(type, args), _name(name) {};
AccessNode::AccessNode(Node::Type type, Node::PNode_t caller, Node::PNode_t arg, std::string name) : 
    ParentNode(type, Node::VecPNode_t{caller, arg}), _name(name) {};
AccessNode::AccessNode(Node::Type type, Node::PNode_t caller, Node::VecPNode_t args, std::string name) : 
    ParentNode(type, caller, args), _name(name) {};

TypeNode::TypeNode(Node::PNode_t child, Node::Type type) : 
    AccessNode(Type::Type, child, "type"), _type(type) {};
TypeNode::TypeNode(Node::PNode_t child, Node::Type type, std::string name) :
    AccessNode(Type::Type, child, name), _type(type) {};
ValueNode::ValueNode(Node::VecPNode_t children) : 
    AccessNode(Type::Value, children, "value") {};
ValueNode::ValueNode(Node::VecPNode_t children, std::string name) : 
    AccessNode(Type::Value, children, name) {};
RecordAccess::RecordAccess(Node::PNode_t record, Node::PNode_t field) : 
    AccessNode(Type::RecordAccess, record, field, ".") {};
ArrayIndex::ArrayIndex(Node::PNode_t array, Node::PNode_t index) : 
    AccessNode(Type::ArrayIndex, array, index, "[]") {};
ParameterList::ParameterList(Node::VecPNode_t children) :
    AccessNode(Type::ParameterList, children, "parameters"), _paramList(children) {};
FunctionCall::FunctionCall(Node::PNode_t function, Node::VecPNode_t args) : 
    AccessNode(Type::FunctionCall, function, args, "()") {};

Write::Write(Token token, Node::PNode_t arg) : 
    ParentNode(Node::Type::Write, token, arg), _argument(arg) {};
WriteLn::WriteLn(Token token, Node::PNode_t arg) : 
    ParentNode(Node::Type::WriteLn, token, arg), _argument(arg) {};
