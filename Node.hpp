#pragma once
#include "Token.hpp"
#include <list>
#include <vector>
#include <memory>

class Node {

    protected:
        typedef std::shared_ptr<Node> PNode_t;
        typedef std::vector<PNode_t> VecPNode_t;
        typedef std::map<std::string, std::pair<PNode_t, PNode_t>> SymTable_t;
        typedef std::shared_ptr<SymTable_t> PSymTable_t;
        typedef std::shared_ptr<std::vector<Node::PSymTable_t>> PVecPSymTable_t;

        enum class Type {
            Program,
            Function,
            Procedure,
            
            DeclarationBlock,
            Declaration,
            VarDecl,
            TypeDecl,
            ProcDecl,
            FuncDecl,
            ConstDecl,

            StatementBlock,
            Statement,

            Type,
            Value,

            Byte,
            Boolean,
            Integer,
            Float,
            Char,
            Subrange,
            CustomType,

            TypeAlias,
            TypeAliasIdentifier,

            String,
            Record,
            Array,

            ReservedWord,
            Identifier,
            ConstIdentifier,
            CustomTypeIdentifier,
            IntConst,
            FloatConst,
            CharConst,
            StringLiteral,
            BinaryOperator,
            UnaryOperator,
            RecordAccess,
            ArrayIndex,
            ParameterList,
            FunctionCall,

            Write,
            WriteLn,
            If,
            For,
            To,
            DownTo,
        };
        // list -> vec
    public:
        Node(Type type) : _type(type), _children(std::vector<PNode_t>(0)) {};
        Node(Type type, Token token) : _type(type), _token(token), _children(std::vector<PNode_t>(0)) {};
        virtual ~Node() {};

        virtual std::string toString();
        virtual void generate();

    protected:
        void addChild(PNode_t pnode);

        Type _type;
        Token _token;
        std::vector<PNode_t> _children;
        friend class Parser;
        friend class AsmCode;
        friend class Subrange;
        friend class Write;
        friend class WriteLn;
        friend class BinOp;
        friend class If;
};

class NamedNode : public Node {
    public:
        NamedNode(Type type, std::string name);
        ~NamedNode() {};

        std::string toString() override { return _name; };

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

        void generate();
};

class FloatConst : public AtomicNode {
    public:
        FloatConst(Token t);
        ~FloatConst() {};

        static void generate();
};

class Identifier : public AtomicNode {
    public:
        Identifier(Token t);
        ~Identifier() {};

        void generate();
        bool isAssignment;
};

class CharConst : public AtomicNode {
    public:
        CharConst(Token t);
        ~CharConst() {};
};

class StringLiteral : public AtomicNode {
    public:
        StringLiteral(Token t);
        ~StringLiteral() {};
};

class TypeAlias : public AtomicNode {
    public:
        TypeAlias(Token t);
        ~TypeAlias() {};
};

class ParentNode : public Node {
    public:
        ParentNode(Type type, Token token, PNode_t child);
        ParentNode(Type type, Token token, PNode_t left, PNode_t right);
        ParentNode(Type type, Token token, VecPNode_t children);
        ParentNode(Type type, VecPNode_t children);
        ParentNode(Type type, PNode_t left, VecPNode_t right);
        ~ParentNode() {};

    private:
        void addChildren(VecPNode_t children);
};

class DeclarationsBlock : public ParentNode {
public:
    DeclarationsBlock(VecPNode_t declarations);
    ~DeclarationsBlock() {};
    std::string toString() override { return _name; };

private:
    std::string _name;
};

class Declaration : public ParentNode {
    public:
        Declaration(Type type, Token keyword, PNode_t declaration);
        Declaration(Type type, Token keyword, VecPNode_t declarations);
        ~Declaration() {};
};

class StatementsBlock : public ParentNode {
public:
    StatementsBlock(VecPNode_t statements);
    ~StatementsBlock() {};
    std::string toString() override { return _name; };

private:
    std::string _name;
};


class Record : public Declaration {
    public:
        Record(Token keyword, VecPNode_t fields);
        Record(Token keyword, VecPNode_t fields, PSymTable_t symTable);
        ~Record() {};

    private:
        PSymTable_t _localSymTable;
};

class Function : public Declaration {
public:
    Function(Token name, VecPNode_t children, PNode_t params, PNode_t type, PVecPSymTable_t symTable);

private:
    PNode_t _type;
    PNode_t _paramList;
    PVecPSymTable_t _localSymTables;
};

class Procedure : public Declaration {
public:
    Procedure(Token name, VecPNode_t children, PNode_t params, PVecPSymTable_t symTable);

private:
    PNode_t _type;
    PNode_t _paramList;
    PVecPSymTable_t _localSymTables;
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

        void generate();
};

class Subrange : public ParentNode {
    public:
        Subrange(Token op, PNode_t lowerBound, PNode_t upperBound);
        ~Subrange() {};

    private:
        uint64_t _lowerBound;
        uint64_t _upperBound;
        friend class Parser;
};

class AccessNode : public ParentNode {
    public:
        AccessNode(Node::Type type, PNode_t child, std::string name);
        AccessNode(Node::Type type, VecPNode_t args, std::string name);
        AccessNode(Node::Type type, PNode_t caller, PNode_t arg, std::string name);
        AccessNode(Node::Type type, PNode_t caller, VecPNode_t args, std::string name);
        ~AccessNode() {};

        std::string toString() override { return _name; };

    private:
        std::string _name;
};

class TypeNode : public AccessNode {
    public:
        TypeNode(PNode_t child, Node::Type type);
        TypeNode(PNode_t child, Node::Type type, std::string name);
        ~TypeNode() {};

        bool isTypeAlias() { return _type == Node::Type::TypeAliasIdentifier; };
        bool isConst() { return _type == Node::Type::ConstIdentifier; };

    private:
        Node::Type _type;
};

class ValueNode : public AccessNode {
    public:
        ValueNode(VecPNode_t children);
        ValueNode(VecPNode_t children, std::string name);
        ~ValueNode() {};
};

class RecordAccess : public AccessNode {
    public:
        RecordAccess(PNode_t record, PNode_t field);
        ~RecordAccess() {};
};

class ArrayIndex : public AccessNode {
    public:
        ArrayIndex(PNode_t array, PNode_t index);
        ~ArrayIndex() {};
};

class ParameterList : public AccessNode {
    public:
        ParameterList(VecPNode_t children);
        ~ParameterList() {};

        VecPNode_t getParams() { return _paramList; };

    private:
        VecPNode_t _paramList;
};

class FunctionCall : public AccessNode {
    public:
        FunctionCall(PNode_t function, VecPNode_t args);
        ~FunctionCall() {};
};

class Write : public ParentNode {
    public:
        Write(Token token, PNode_t arg);
        ~Write() {};

        void generate();

    private:
        PNode_t _argument;
};

class WriteLn : public ParentNode {
    public:
        WriteLn(Token token, PNode_t arg);
        ~WriteLn() {};

        void generate();

    private:
        PNode_t _argument;
};

class If : public ParentNode {
    public:
        If(Token token, PNode_t condition, PNode_t thenBranch, PNode_t elseBranch);
        ~If() {};

        void generate();

    private:
        PNode_t _condition;
        PNode_t _thenBranch;
        PNode_t _elseBranch;
};

class For : public ParentNode {
    public:
        For(Token token, PNode_t initial, PNode_t to_downto, PNode_t final, PNode_t body);
        ~For() {};

        void generate();

    private:
        PNode_t _initial;
        PNode_t _to_downto;
        PNode_t _final;
        PNode_t _body;
};

class To : public AtomicNode {
    public:
        To(Token token);
        ~To() {};
};

class DownTo : public AtomicNode {
    public:
        DownTo(Token token);
        ~DownTo() {};
};

class ReservedWord : public AtomicNode {
    public:
        ReservedWord(Token token);
        ~ReservedWord() {};
};