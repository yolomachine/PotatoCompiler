#include "Node.hpp"

void Node::addChild(PNode_t pnode) {
    _children.push_back(pnode);
};

std::string Node::toString() {
    return _token.toString();
};

AtomicNode::AtomicNode(Type type, Token token) : Node(type, token) {};
IntConst::IntConst(Token t) : AtomicNode(Type::IntConst, t) {};
FloatConst::FloatConst(Token t) : AtomicNode(Type::FloatConst, t) {};
Identifier::Identifier(Token t) : AtomicNode(Type::Identifier, t) {};
StringLiteral::StringLiteral(Token t) : AtomicNode(Type::StringLiteral, t) {};

ParentNode::ParentNode(Type type, Token token, Node::PNode_t child) : Node(type, token) {
    addChild(child);
};

ParentNode::ParentNode(Type type, Token token, Node::PNode_t left, Node::PNode_t right) : Node(type, token) {
    addChild(left);
    addChild(right);
};

ParentNode::ParentNode(Type type, Token token, std::vector<Node::PNode_t> children) : Node(type, token) {
    addChildren(children);
}

ParentNode::ParentNode(Type type, std::vector<Node::PNode_t> children) : Node(type) {
    addChildren(children);
};

ParentNode::ParentNode(Type type, Node::PNode_t left, std::vector<Node::PNode_t> right) : Node(type) {
    addChild(left);
    addChildren(right);
};

void ParentNode::addChildren(std::vector<Node::PNode_t> children) {
    for (auto i : children)
        addChild(i);
}

Declaration::Declaration(Type type, Token keyword, std::vector<Node::PNode_t> declarations) : ParentNode(type, keyword, declarations) {};
DeclarationsBlock::DeclarationsBlock(std::vector<Node::PNode_t> declarations) : ParentNode(Type::DeclarationBlock, declarations), _name("declarations") {};

UnaryOperator::UnaryOperator(Token op, Node::PNode_t expr) : ParentNode(Type::UnaryOperator, op, expr) {};
BinaryOperator::BinaryOperator(Token op, PNode_t left, PNode_t right) : ParentNode(Type::UnaryOperator, op, left, right) {};

AccessNode::AccessNode(Node::Type type, std::vector<Node::PNode_t> args, std::string name) : ParentNode(type, args), _name(name) {};
AccessNode::AccessNode(Node::Type type, Node::PNode_t caller, Node::PNode_t arg, std::string name) : ParentNode(type, std::vector<Node::PNode_t>{caller, arg}), _name(name) {};
AccessNode::AccessNode(Node::Type type, Node::PNode_t caller, std::vector<Node::PNode_t> args, std::string name) : ParentNode(type, caller, args), _name(name) {};

RecordAccess::RecordAccess(Node::PNode_t record, Node::PNode_t field) : AccessNode(Type::RecordAccess, record, field, ".") {};
ArrayIndex::ArrayIndex(Node::PNode_t array, Node::PNode_t index) : AccessNode(Type::ArrayIndex, array, index, "[]") {};
FunctionCall::FunctionCall(Node::PNode_t function, std::vector<Node::PNode_t> args) : AccessNode(Type::FunctionCall, function, args, "()") {};
Enum::Enum(std::vector<Node::PNode_t> identifiers) : AccessNode(Type::Enum, identifiers, "enum") {};