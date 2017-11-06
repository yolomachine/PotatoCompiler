#pragma once
#include "FiniteAutomata.hpp"
#include <utility>
#include <string>
#include <map>
#include <sstream>

class Token {

    union Value {
        char* s;
        double d;
        unsigned long long ull;
    };

    enum class ValueType {
        String,
        Double,
        ULL,
    };

    enum class Class {
        ReservedWord,
        Identifier,
        Operator,
        Separator,
        Constant,
        StringLiteral,
    };

    enum class SubClass {
        Add,
        Sub,
        Mult,
        Div,
        Equal,
        Less,
        More,
        Address,
        Pointer,
        Dot,
        AddAssign,
        SubAssign,
        MultAssign,
        DivAssign,
        Assign,
        LEQ,
        MEQ,
        NEQ,
        SymDiff,
        Exp,
        Range,
        And,
        Or,
        IntDiv,
        Mod,
        Not,
        SHL,
        SHR,
        Xor,

        Colon,
        Comma,
        Semicolon,
        LeftBracket,
        RightBracket,
        LeftParenthesis,
        RightParenthesis,

        Absolute,
        Array,
        Asm,
        Begin,
        Break,
        Case,
        Const,
        Constructor,
        Continue,
        Destructor,
        Do,
        DownTo,
        Else,
        End,
        False,
        File,
        For,
        Function,
        Goto,
        If,
        Implementation,
        In,
        Inline,
        Interface,
        Label,
        Nil,
        Object,
        Of,
        Packed,
        Procedure,
        Program,
        Record,
        Repeat,
        Set,
        String,
        Then,
        To,
        True,
        Type,
        Unit,
        Until,
        Uses,
        Var,
        While,
        With,

        IntConst,
        FloatConst,
        Identifier,
        StringLiteral,
        EndOfFile,
    };

    typedef std::pair<int, int> Position_t;
    typedef std::map<std::string, std::pair<SubClass, Class>> Dict_t;

    public:
        Token(FiniteAutomata::States state, Position_t pos, std::string raw, std::string value);
        Token() {};
        ~Token() {};

    private:
        std::string toString();

        Class _class;
        SubClass _subClass;
        Value _value;
        ValueType _vtype;
        Position_t _pos;
        std::string _raw;
        static const Dict_t _dict;
        friend class LexicalAnalyzer;
        friend class Node;
        friend class Parser;
};