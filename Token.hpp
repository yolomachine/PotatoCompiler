#pragma once
#include "FiniteAutomata.hpp"
#include <utility>
#include <string>
#include <map>

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
    };

    typedef std::pair<int, int> Position_t;
    typedef std::map<std::string, Class> Dict_t;

    public:
        Token(FiniteAutomata::States state, Position_t pos, std::string raw, std::string value);
        Token() {};
        ~Token() {};

    private:
        Class _class;
        Value _value;
        ValueType _vtype;
        Position_t _pos;
        std::string _raw;
        static const Dict_t _reservedWords;
};