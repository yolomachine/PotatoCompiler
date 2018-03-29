#include "AsmCode.hpp"

int AsmCode::_offset = 0;
int AsmCode::_ifLabelCounter = 0;
std::vector<AsmCode::PAsmCommand> AsmCode::_commands = {};
std::map<std::string, AsmCode::PAsmConstant> AsmCode::_constants = {};
std::map<std::string, std::pair<int, int>> AsmCode::_offsetMap = {};

const AsmCode::AsmCommandsDict_t AsmCode::_asmCommands = {
    { AsmCommands::NoCommand, "" },
    { AsmCommands::Enter,  "enter"  },
    { AsmCommands::Push,   "push"   },
    { AsmCommands::Pop,    "pop"    },
    { AsmCommands::Lea,    "lea"    },
    { AsmCommands::Mov,    "mov"    },
    { AsmCommands::Movsx,  "movsx"  },
    { AsmCommands::Cdq,    "cdq"    },
    { AsmCommands::Jump,   "jmp"   },
    { AsmCommands::Jz,     "jz"     },
    { AsmCommands::Setge,  "setge"  },
    { AsmCommands::Setg,   "setg"   },
    { AsmCommands::Setle,  "setle"  },
    { AsmCommands::Setl,   "setl"   },
    { AsmCommands::Setne,  "setne"  },
    { AsmCommands::Sete,   "sete"   },
    { AsmCommands::Cmp,    "cmp"    },
    { AsmCommands::Test,   "test"   },
    { AsmCommands::Add,    "add"    },
    { AsmCommands::Sub,    "sub"    },
    { AsmCommands::Imul,   "imul"   },
    { AsmCommands::Idiv,   "idiv"   },
    { AsmCommands::Addsd,  "addsd"  },
    { AsmCommands::Subsd,  "subsd"  },
    { AsmCommands::Imulsd, "imulsd" },
    { AsmCommands::Idivsd, "idivsd" },
    { AsmCommands::Call,   "call"   },
    { AsmCommands::Leave,  "leave"  },
    { AsmCommands::Ret,    "ret"    },
    { AsmCommands::Exit,   "exit"   },
    { AsmCommands::End,    "end"    },
};

const AsmCode::ConstSizesDict_t AsmCode::_constSizes = {
    { ConstSize::DB, "db" },
    { ConstSize::DQ, "dq" },
};

const AsmCode::PrintFormatsDict_t AsmCode::_printFormats = {
    { PrintFormat::Float,     "37,102,32,0" },
    { PrintFormat::Integer,   "37,100,32,0" },
    { PrintFormat::FloatLn,   "37,102,32,10,0" },
    { PrintFormat::IntegerLn, "37,100,32,10,0" },
};

void AsmCode::addCommand(AsmCode::PAsmCommand command) {
    _commands.push_back(command);
};

void AsmCode::addConstant(AsmCode::PAsmConstant constant) {
    _constants[constant->_name] = constant;
};

void AsmCode::generateStatements(Node::PNode_t node) {
    if (node->_type == Node::Type::If ||
        node->_token._subClass == Token::SubClass::Assign)
        node->generate();
    else {
        for (auto i : node->_children)
            generateStatements(i);
            node->generate();
    };
}

void AsmCode::generate(std::ostream& os) {
    os << "include G:\\masm32\\include\\masm32rt.inc\n\n.xmm\n";
    if (_constants.size()) {
        os << ".const\n";
        for (auto i : _constants)
            os << i.second->print().c_str() << "\n\n";
    }

    os << ".code\n__@function0:\n";
    if (_commands.size())
        for (auto i : _commands)
            os << i->print().c_str() << "\n";

    os << "leave\n" << "ret 0\n\n"
        << "start:\n" << "call __@function0\n"
        << "exit\n" << "end start\n";
};

int AsmCode::getTypeSize(Node::PNode_t node) {
    if (node->_type == Node::Type::Integer)
        return sizeof(int);
    else if (node->_type == Node::Type::Float)
        return sizeof(double);
    return sizeof(int);
};

AsmCommand::AsmCommand(AsmCommands command, std::vector<std::string> args) : _args(args) {
    _command = AsmCode::_asmCommands.at(command);
};

std::string AsmCommand::print() {
    std::stringstream ss;
    ss << _command;
    if (_command.length())
        ss << " ";
    for (auto i = _args.begin(); i != _args.end(); ++i) {
        ss << *i;
        if (i != _args.end()-1)
            ss << ", ";
    };
    
    return ss.str();
};

AsmConstant::AsmConstant(std::string name, ConstSize size, PrintFormat format) : _name(name) {
    _size = AsmCode::_constSizes.at(size);
    _format = AsmCode::_printFormats.at(format);
};

std::string AsmConstant::print() {
    std::stringstream ss;
    ss << _name << " " << _size << " " << _format;
    return ss.str();
};

void IntConst::generate() {
    std::vector<std::string> args;
    args = { this->toString() };
    AsmCode::PAsmCommand cmd = std::make_shared<AsmCommand>(AsmCommands::Push, args);
    AsmCode::addCommand(cmd);
};

void Write::generate() {
    std::string bytesToClear;
    std::vector<std::string> args;
    AsmCode::addConstant(std::make_shared<AsmConstant>("__@strfmti", ConstSize::DB, PrintFormat::Integer));
    args = { "offset __@strfmti" };
    bytesToClear = "8";
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    args = { "crt_printf" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Call, args));
    args = { "esp", bytesToClear };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Add, args));
};

void WriteLn::generate() {
    std::string bytesToClear;
    std::vector<std::string> args;
    AsmCode::addConstant(std::make_shared<AsmConstant>("__@strfmtiln", ConstSize::DB, PrintFormat::IntegerLn));
    args = { "offset __@strfmtiln" };
    bytesToClear = "8";
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    args = { "crt_printf" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Call, args));
    args = { "esp", bytesToClear };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Add, args));
};

void BinaryOperator::generate() {
    AsmCommands cmp;
    std::vector<std::string> args;
    switch (_token._subClass) {
    case Token::SubClass::Assign:
        _children.front()->generate();
        AsmCode::generateStatements(_children.back());
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "dword ptr [ebx]", "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Mov, args));
        return;
        break;
    case Token::SubClass::Add:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Add, args));
        break;
    case Token::SubClass::Sub:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Sub, args));
        break;
    case Token::SubClass::Mult:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Imul, args));
        break;
    case Token::SubClass::Div:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Cdq, args));
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Idiv, args));
        break;
    case Token::SubClass::Less:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        cmp = AsmCommands::Setge;
        goto label;
    case Token::SubClass::LEQ:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        cmp = AsmCommands::Setg;
        goto label;
    case Token::SubClass::More:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        cmp = AsmCommands::Setle;
        goto label;
    case Token::SubClass::MEQ:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        cmp = AsmCommands::Setl;
        goto label;
    case Token::SubClass::Equal:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        cmp = AsmCommands::Setne;
        goto label;
    case Token::SubClass::NEQ:
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "eax", "ebx" };
        cmp = AsmCommands::Sete;
    label:
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Cmp, args));
        args = { "al" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(cmp, args));
        args = { "al", "1" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Sub, args));
        args = { "eax", "al" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Movsx, args));
        break;
    }
    args = { "eax" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
};

void Identifier::generate() {
    std::vector<std::string> args;
    args = { "eax", "dword ptr [ebp - " + std::to_string(AsmCode::_offsetMap[toString()].second) + "]" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Lea, args));
    if (isAssignment) {
      args = { "eax" };
      AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    }
    else {
        args = { "dword ptr [eax]" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    }
};

void If::generate() {
    ++AsmCode::_ifLabelCounter;
    std::vector<std::string> args;
    AsmCode::generateStatements(_condition);
    std::string elseLabel("else_branch");
    elseLabel += std::to_string(AsmCode::_ifLabelCounter);
    std::string endLabel("end_if");
    endLabel += std::to_string(AsmCode::_ifLabelCounter);
    args = { "eax" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
    args = { "eax", "eax" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Test, args));
    args = { elseLabel };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Jz, args));
    AsmCode::generateStatements(_thenBranch);
    args = { endLabel };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Jump, args));
    args = { elseLabel + ":" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::NoCommand, args));
    AsmCode::generateStatements(_elseBranch);
    args = { endLabel + ":" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::NoCommand, args));
};

void For::generate() {

};