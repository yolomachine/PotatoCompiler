#pragma once
#include "Token.hpp"
#include <list>
#include <vector>
#include <memory>

class Node {

    protected:
        typedef std::shared_ptr<Node> PNode_t;

        enum class Type {
            IntConst,
            FloatConst,
            Identifier,
            ReservedWord,
            StringLiteral,
            BinaryOperator,
            UnaryOperator,
            RecordAccess,
            ArrayIndex,
            FunctionCall,
        };

    public:
        Node(Type type) : _type(type), _children(std::list<PNode_t>(0)) {};
        Node(Type type, Token token) : _type(type), _token(token), _children(std::list<PNode_t>(0)) {};
        virtual ~Node() {};

        virtual std::string toString();

    protected:
        void addChild(PNode_t pnode);

        Type _type;
        Token _token;
        std::list<PNode_t> _children;
        friend class Parser;
};

class UnaryOperator : public Node {
    public:
        UnaryOperator(Token op, PNode_t expr);
};

class BinaryOperator : public Node {
    public:
        BinaryOperator(Token op, PNode_t left, PNode_t right);
};

class IntConst : public Node {
public:
    IntConst(Token t);
};

class FloatConst : public Node {
public:
    FloatConst(Token t);
};

class Identifier : public Node {
public:
    Identifier(Token t);
};

class StringLiteral : public Node {
public:
    StringLiteral(Token t);
};

class AccessNode : public Node {
    public:
        AccessNode::AccessNode(Node::PNode_t caller, Node::PNode_t arg, std::string name, Node::Type type);
        AccessNode::AccessNode(Node::PNode_t caller, std::vector<Node::PNode_t> args, std::string name, Node::Type type);
        std::string toString();
    private:
        std::string _name;
};

class RecordAccess : public AccessNode {
    public:
        RecordAccess(Node::PNode_t record, Node::PNode_t field);
};

class ArrayIndex : public AccessNode {
    public:
        ArrayIndex(PNode_t array, PNode_t index);
};

class FunctionCall : public AccessNode {
    public:
        FunctionCall(Node::PNode_t function, std::vector<Node::PNode_t> args);
};