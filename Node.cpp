#include "Node.hpp"

void Node::addChild(PNode_t pnode) {
    _children.push_back(pnode);
};

std::string Node::toString() {
    return _token.toString();
};

UnaryOperator::UnaryOperator(Token op, Node::PNode_t expr) : Node(Type::UnaryOperator, op) {
    addChild(expr);
};

BinaryOperator::BinaryOperator(Token op, PNode_t left, PNode_t right) : Node(Type::BinaryOperator, op) {
    addChild(left);
    addChild(right);
};

IntConst::IntConst(Token t) : Node(Type::IntConst, t) {};
FloatConst::FloatConst(Token t) : Node(Type::FloatConst, t) {};
Identifier::Identifier(Token t) : Node(Type::Identifier, t) {};
StringLiteral::StringLiteral(Token t) : Node(Type::StringLiteral, t) {};

AccessNode::AccessNode(Node::PNode_t caller, Node::PNode_t arg, std::string name, Node::Type type) : Node(type), _name(name) {
    addChild(caller);
    addChild(arg);
};

AccessNode::AccessNode(Node::PNode_t caller, std::vector<Node::PNode_t> args, std::string name, Node::Type type) : Node(type), _name(name) {
    addChild(caller);
    for (auto i : args)
        addChild(i);
};

std::string AccessNode::toString() {
    return _name;
}

RecordAccess::RecordAccess(Node::PNode_t record, Node::PNode_t field) : AccessNode(record, field, ".", Type::RecordAccess) {};
ArrayIndex::ArrayIndex(Node::PNode_t array, Node::PNode_t index) : AccessNode(array, index, "[]", Type::ArrayIndex) {};
FunctionCall::FunctionCall(Node::PNode_t function, std::vector<Node::PNode_t> args) : AccessNode(function, args, "()", Type::FunctionCall) {};