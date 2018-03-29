#pragma once
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include "Node.hpp"

enum class ConstSize {
    DB,
    DQ,
};

enum class PrintFormat {
    Integer,
    Float,
    IntegerLn,
    FloatLn,
};

enum class AsmCommands {
    NoCommand,

    Enter,
    Push,
    Pop,
    Lea,
    Mov,
    Movsx,
    Cdq,
    Jump,
    Jz,
    
    Setge,
    Setg,
    Setle,
    Setl,
    Setne,
    Sete,
    Cmp,
    Test,

    Add,
    Sub,
    Imul,
    Idiv,
    
    Addsd,
    Subsd,
    Imulsd,
    Idivsd,
    
    Call,
    Leave,
    Ret,
    Exit,
    End,
};

class AsmCommand {
    public:
        AsmCommand(AsmCommands command, std::vector<std::string> args);
        ~AsmCommand() {};

        std::string print();

    private:
        std::string _command;
        std::vector<std::string> _args;
        friend class AsmCode;
        friend class Parser;
};

class AsmConstant {
    public:
        AsmConstant(std::string name, ConstSize size, PrintFormat format);
        ~AsmConstant() {};

        std::string print();

    private:
        std::string _name;
        std::string _size;
        std::string _format;
        friend class AsmCode;
        friend class Parser;
};

class AsmCode {
    public:
        typedef std::shared_ptr<AsmCommand> PAsmCommand;
        typedef std::shared_ptr<AsmConstant> PAsmConstant;
        typedef std::map<AsmCommands, std::string> AsmCommandsDict_t;
        typedef std::map<ConstSize, std::string> ConstSizesDict_t;
        typedef std::map<PrintFormat, std::string> PrintFormatsDict_t;

        static void addCommand(PAsmCommand);
        static void addConstant(PAsmConstant);
        static void generateStatements(Node::PNode_t node);
        static void generate(std::ostream& os);
        static int getTypeSize(Node::PNode_t);

    private:
        static int _ifLabelCounter;
        static int _offset;
        static std::map<std::string, std::pair<int, int>> _offsetMap;
        static std::vector<PAsmCommand> _commands;
        static std::map<std::string, PAsmConstant> _constants;
        static const AsmCommandsDict_t _asmCommands;
        static const ConstSizesDict_t _constSizes;
        static const PrintFormatsDict_t _printFormats;
        friend class AsmConstant;
        friend class AsmCommand;
        friend class Parser;
        friend class Node;
        friend class Identifier;
        friend class If;
};