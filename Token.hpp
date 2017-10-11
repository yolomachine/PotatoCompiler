#pragma once
#include "FiniteAutomata.hpp"
#include <utility>
#include <string>

class Token {

    typedef std::pair<int, int> Position_t;

    enum class Class {
        ReservedWord,
        Identifier,
        Operator,
        Separator,
        Constant
    };

    union Value {
        char* s;
        double d;
        unsigned long long ull;
    };

    public:
        Token() {};
        Token(FiniteAutomata::States state, Position_t pos, std::string raw, std::string value) : _pos(pos), _raw(raw) {
            switch (state) {
            case FiniteAutomata::States::Decimal:
                _value.ull = std::stoull(value);
                break;

            default:
                break;
            };
        }

        ~Token() {};

    private:
        Class _class;
        Value _value;
        Position_t _pos;
        std::string _raw;
};