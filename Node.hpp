#pragma once
#include "Token.hpp"
#include <list>
#include <memory>

class Node {

    typedef std::shared_ptr<Node> PNode;

    enum class Type {
        Root,
        IntConst,
        FloatConst,
        Identifier,
        ReservedWord,
        StringLiteral,
        BinaryOperator,
        UnaryOperator,
    };

    public:
        Node(Type type) : _type(type), _children(std::list<PNode>(0)) {};
        Node(Type type, Token token) : _type(type), _token(token), _children(std::list<PNode>(0)) {};
        ~Node() {};

        void addChild(PNode pnode) {
            _children.push_back(pnode);
        };

    private:
        Type _type;
        Token _token;
        std::list<PNode> _children;
        friend class Parser;
};