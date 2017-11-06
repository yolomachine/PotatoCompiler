#pragma once
#include "LexicalAnalyzer.hpp"

const LexicalAnalyzer::ClassDict_t LexicalAnalyzer::_classDict = {
    { Token::Class::ReservedWord,        "Reserved word"  },
    { Token::Class::Identifier,          "Identifier"     },
    { Token::Class::Operator,            "Operator"       },
    { Token::Class::Separator,           "Separator"      },
    { Token::Class::Constant,            "Constant"       },
    { Token::Class::StringLiteral,       "String Literal" },
};

const LexicalAnalyzer::SubClassDict_t LexicalAnalyzer::_subClassDict = {
   { Token::SubClass::Add,              "+"   },
   { Token::SubClass::Sub,              "-"   },
   { Token::SubClass::Mult,             "*"   },
   { Token::SubClass::Div,              "/"   },
   { Token::SubClass::Equal,            "="   },
   { Token::SubClass::Less,             "<"   },
   { Token::SubClass::More,             ">"   },
   { Token::SubClass::Address,          "@"   },
   { Token::SubClass::Pointer,          "^"   },
   { Token::SubClass::Dot,              "."   },
   { Token::SubClass::AddAssign,        "+="  },
   { Token::SubClass::SubAssign,        "-="  },
   { Token::SubClass::MultAssign,       "*="  },
   { Token::SubClass::DivAssign,        "/="  },
   { Token::SubClass::Assign,           ":="  },
   { Token::SubClass::LEQ,              "<="  },
   { Token::SubClass::MEQ,              ">="  },
   { Token::SubClass::SHL,              "<<"  },
   { Token::SubClass::SHR,              ">>"  },
   { Token::SubClass::NEQ,              "<>"  },
   { Token::SubClass::SymDiff,          "><"  },
   { Token::SubClass::Exp,              "**"  },
   { Token::SubClass::Range,            ".."  },
   { Token::SubClass::And,              "and" },
   { Token::SubClass::IntDiv,           "div" },
   { Token::SubClass::Mod,              "mod" },
   { Token::SubClass::Not,              "not" },
   { Token::SubClass::SHL,              "shl" },
   { Token::SubClass::SHR,              "shr" },
   { Token::SubClass::Xor,              "xor" },

   { Token::SubClass::Colon,            ":"  },
   { Token::SubClass::Comma,            ","  },
   { Token::SubClass::Semicolon,        ";"  },
   { Token::SubClass::LeftBracket,      "["  },
   { Token::SubClass::RightBracket,     "]"  },
   { Token::SubClass::LeftBracket,      "(." },
   { Token::SubClass::RightBracket,     ".)" },
   { Token::SubClass::LeftParenthesis,  "("  },
   { Token::SubClass::RightParenthesis, ")"  },

   { Token::SubClass::Absolute,         "absolute"       },
   { Token::SubClass::Array,            "array"          },
   { Token::SubClass::Asm,              "asm"            },
   { Token::SubClass::Begin,            "begin"          },
   { Token::SubClass::Break,            "break"          },
   { Token::SubClass::Case,             "case"           },
   { Token::SubClass::Const,            "const"          },
   { Token::SubClass::Constructor,      "constructor"    },
   { Token::SubClass::Continue,         "continue"       },
   { Token::SubClass::Destructor,       "destructor"     },
   { Token::SubClass::Do,               "do"             },
   { Token::SubClass::DownTo,           "downto"         },
   { Token::SubClass::Else,             "else"           },
   { Token::SubClass::End,              "end"            },
   { Token::SubClass::False,            "false"          },
   { Token::SubClass::File,             "file"           },
   { Token::SubClass::For,              "for"            },
   { Token::SubClass::Function,         "function"       },
   { Token::SubClass::Goto,             "goto"           },
   { Token::SubClass::If,               "if"             },
   { Token::SubClass::Implementation,   "implementation" },
   { Token::SubClass::In,               "in"             },
   { Token::SubClass::Inline,           "inline"         },
   { Token::SubClass::Interface,        "interface"      },
   { Token::SubClass::Label,            "label"          },
   { Token::SubClass::Nil,              "nil"            },
   { Token::SubClass::Object,           "object"         },
   { Token::SubClass::Of,               "of"             },
   { Token::SubClass::Packed,           "packed"         },
   { Token::SubClass::Procedure,        "procedure"      },
   { Token::SubClass::Program,          "program"        },
   { Token::SubClass::Record,           "record"         },
   { Token::SubClass::Repeat,           "repeat"         },
   { Token::SubClass::Set,              "set"            },
   { Token::SubClass::String,           "string"         },
   { Token::SubClass::Then,             "then"           },
   { Token::SubClass::To,               "to"             },
   { Token::SubClass::True,             "true"           },
   { Token::SubClass::Type,             "type"           },
   { Token::SubClass::Unit,             "unit"           },
   { Token::SubClass::Until,            "until"          },
   { Token::SubClass::Uses,             "uses"           },
   { Token::SubClass::Var,              "var"            },
   { Token::SubClass::While,            "while"          },
   { Token::SubClass::With,             "with"           },
   { Token::SubClass::IntConst,         "int const"      },
   { Token::SubClass::FloatConst,       "float const"    },
   { Token::SubClass::Identifier,       "identifier"     },
   { Token::SubClass::StringLiteral,    "string literal" },
   { Token::SubClass::EndOfFile,        "end of file"    },
};

LexicalAnalyzer::LexicalAnalyzer(const char* filename) : _currentState(FiniteAutomata::States::Whitespace), _row(1), _column(1) {
    open(filename);
};

Token LexicalAnalyzer::currentToken() const {
    return _currentToken;
};

Token LexicalAnalyzer::nextToken() {
    char c;
    std::string raw;
    std::string val;
    std::string code;
    FiniteAutomata::States state;
    while (!eof()) {
        if (_file.eof())
            c = 128;
        else 
            _file >> std::noskipws >> c;
        state = FiniteAutomata::states[static_cast<unsigned int>(_currentState)][tolower(abs(c)) - 1];
        switch (state) {
        case FiniteAutomata::States::NewLine: 
        case FiniteAutomata::States::CommentNewLine:
            _column = 0;
            ++_row;
            break;
        case FiniteAutomata::States::BeginComment:
        case FiniteAutomata::States::BeginMultilineComment:
            raw = val = "";
        case FiniteAutomata::States::Whitespace:
        case FiniteAutomata::States::Comment:
        case FiniteAutomata::States::MultilineComment:
        case FiniteAutomata::States::Asterisk:
            break;
        case FiniteAutomata::States::ControlString:
        case FiniteAutomata::States::StringStart:
            if (code.length()) {
                val += codeToChar(_currentState, code);
                code = "";
            }
        case FiniteAutomata::States::StringEnd:
        case FiniteAutomata::States::Percent:
        case FiniteAutomata::States::Dollar:
        case FiniteAutomata::States::Ampersand:
            raw += c;
            break;
        case FiniteAutomata::States::Identifier:
            if (raw.length() && raw.back() == '&')
                val += '&';
            raw += c;
            val += tolower(c);
            break;
        case FiniteAutomata::States::OperatorDot:
        case FiniteAutomata::States::OperatorGreater:
        case FiniteAutomata::States::OperatorLess:
        case FiniteAutomata::States::OperatorMult:
        case FiniteAutomata::States::OperatorPlus:
        case FiniteAutomata::States::Operator:
            raw += c;
            val += tolower(c);
            break;
        case FiniteAutomata::States::DecimalCharCode:
        case FiniteAutomata::States::BinCharCode:
        case FiniteAutomata::States::OctCharCode:
        case FiniteAutomata::States::HexCharCode:
            if (c > '&')
                code += c;
            raw += c;
            break;
        case FiniteAutomata::States::Decimal:
        case FiniteAutomata::States::Bin:
        case FiniteAutomata::States::Oct:
        case FiniteAutomata::States::Hex:
        case FiniteAutomata::States::Float:
        case FiniteAutomata::States::FloatingPoint:
        case FiniteAutomata::States::FloatExp:
        case FiniteAutomata::States::FloatExpSign:
        case FiniteAutomata::States::FloatEnd:
        case FiniteAutomata::States::String:
        case FiniteAutomata::States::Separator:
        case FiniteAutomata::States::Colon:
        case FiniteAutomata::States::LeftParenthesis:
        case FiniteAutomata::States::Slash:
            raw += c;
            val += c;
            break;
        case FiniteAutomata::States::LookBack:
            --_column;
            state = FiniteAutomata::States::TokenEnd;
            _currentState = FiniteAutomata::States::Decimal;
            _file.putback(c);
            _file.putback(raw.back());
            raw.pop_back();
            val.pop_back();
            goto token;
        case FiniteAutomata::States::TokenEnd:
            if (code.length()) {
                val += codeToChar(_currentState, code);
                code = "";
            }
            if (!_file.eof())
                _file.putback(c);
            goto token;
        case FiniteAutomata::States::EndOfFile:
            _currentState = state = FiniteAutomata::States::EndOfFile;
            raw = val = "EOF";
            _column += 3;
            goto token;
        default:
            throwException({ _row, _column }, state);
            break;
        }
        _file.peek();
        ++_column;
        _currentState = state;
    }
    token:
    if (eof() && !val.length())
        return _currentToken;
    Token t(_currentState, { _row, _column - raw.length() }, raw, val);
    _currentToken = t;
    _currentState = state;
    return t;

};

char LexicalAnalyzer::codeToChar(FiniteAutomata::States state, std::string code) {
    char c;
    switch (state) {
    case FiniteAutomata::States::BinCharCode:
        c = static_cast<char>(std::stoi(code, 0, 2));
        break;
    case FiniteAutomata::States::HexCharCode:
        c = static_cast<char>(std::stoi(code, 0, 16));
        break;
    case FiniteAutomata::States::OctCharCode:
        c = static_cast<char>(std::stoi(code, 0, 8));
        break;
    default:
        c = static_cast<char>(std::stoi(code));
        break;
    }
    return c;
};

void LexicalAnalyzer::log(std::ostream &os) {
    std::list<Token> tokens;
    try {
        while (!eof())
            tokens.push_back(nextToken());
    } catch (std::exception e) {
        os << e.what();
        return;
    };
    for (Token t : tokens) {
        std::stringstream pos, type, raw, val;
        pos  << "(" << t._pos.first << ", " << t._pos.second << ")";
        type << _classDict.at(t._class);
        raw  << t._raw;
        val  << t.toString();
        os << pos.str()  << std::string(std::abs(static_cast<int>(20 - pos.str().length())), ' ') 
           << type.str() << std::string(std::abs(static_cast<int>(20 - type.str().length())), ' ')
           << raw.str()  << std::string(std::abs(static_cast<int>(30 - raw.str().length())), ' ')
           << val.str()  << std::endl;
    }
};

template<typename T>
void LexicalAnalyzer::open(T filename) {
    if (_file.is_open())
        _file.close();
    _file.open(filename);
    _file.peek();
};
template void LexicalAnalyzer::open<const char*>(const char*);
template void LexicalAnalyzer::open<const std::string&>(const std::string&);

bool LexicalAnalyzer::eof() {
    return _file.eof() && (_currentState == FiniteAutomata::States::EndOfFile);
};
void LexicalAnalyzer::throwException(Token::Position_t pos, FiniteAutomata::States state) {
    std::stringstream ss;
    ss << "(" << pos.first << ", " << pos.second << "): ";
    switch (state) {
    case FiniteAutomata::States::EOLnWhileReading:
        ss << "Unexpected end of line";
        break;
    case FiniteAutomata::States::IllegalSymbol:
        ss << "Illegal symbol";
        break;
    case FiniteAutomata::States::NumberExpected:
        ss << "Number expected";
        break;
    case FiniteAutomata::States::ScaleFactorExpected:
        ss << "Exponent scale factor expected";
        break;
    case FiniteAutomata::States::UnexpectedEndOfFile:
        ss << "Unexpected end of file";
        break;
    case FiniteAutomata::States::UnexpectedSymbol:
        ss << "Unexpected symbol";
        break;
    default:
        throw std::exception("What a Terrible Failure");
        break;
    }

    throw std::exception(ss.str().c_str());
};