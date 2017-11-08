#pragma once
#include "Token.hpp"
#include <list>
#include <vector>
#include <memory>

class Node {

    protected:
        typedef std::shared_ptr<Node> PNode_t;

        enum class Type {
            Program,
            
            DeclarationBlock,
            Declaration,
            VarDecl,
            LabelDecl,
            TypeDecl,
            ConstDecl,

            StatementBlock,
            Statement,

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

class Name {
    public:
        Name(std::string name);
        ~Name() {};

        std::string toString();

    private:
        std::string _name;
};

class AtomicNode : public Node {
    public:
        AtomicNode(Type type, Token token);
        ~AtomicNode() {};
};

class IntConst : public AtomicNode {
    public:
        IntConst(Token t);
        ~IntConst() {};
};

class FloatConst : public AtomicNode {
    public:
        FloatConst(Token t);
        ~FloatConst() {};
};

class Identifier : public AtomicNode {
    public:
        Identifier(Token t);
        ~Identifier() {};
};

class StringLiteral : public AtomicNode {
    public:
        StringLiteral(Token t);
        ~StringLiteral() {};
};

class ParentNode : public Node {
    public:
        ParentNode(Type type, Token token, Node::PNode_t child);
        ParentNode(Type type, Token token, Node::PNode_t left, Node::PNode_t right);
        ParentNode(Type type, Token token, std::vector<Node::PNode_t> children);
        ParentNode(Type type, std::vector<Node::PNode_t> children);
        ParentNode(Type type, Node::PNode_t left, std::vector<Node::PNode_t> right);
        ~ParentNode() {};

    private:
        void addChildren(std::vector<Node::PNode_t> children);
};

class Declaration : public ParentNode {
    public:
        Declaration(Token keyword, std::vector<Node::PNode_t> declarations);
        ~Declaration() {};
};

class DeclarationsBlock : public ParentNode, public Name {
    public:
        DeclarationsBlock(std::vector<Node::PNode_t> declarations);
        ~DeclarationsBlock() {};

        std::string toString() override { return Name::toString(); };
};

class UnaryOperator : public ParentNode {
    public:
        UnaryOperator(Token op, PNode_t expr);
        ~UnaryOperator() {};
};

class BinaryOperator : public ParentNode {
    public:
        BinaryOperator(Token op, PNode_t left, PNode_t right);
        ~BinaryOperator() {};
};

class AccessNode : public ParentNode, public Name {
    public:
        AccessNode::AccessNode(Node::Type type, Node::PNode_t caller, Node::PNode_t arg, std::string name);
        AccessNode::AccessNode(Node::Type type, Node::PNode_t caller, std::vector<Node::PNode_t> args, std::string name);
        ~AccessNode() {};

        std::string toString() override { return Name::toString(); };
};

class RecordAccess : public AccessNode {
    public:
        RecordAccess(Node::PNode_t record, Node::PNode_t field);
        ~RecordAccess() {};
};

class ArrayIndex : public AccessNode {
    public:
        ArrayIndex(PNode_t array, PNode_t index);
        ~ArrayIndex() {};
};

class FunctionCall : public AccessNode {
    public:
        FunctionCall(Node::PNode_t function, std::vector<Node::PNode_t> args);
        ~FunctionCall() {};
};