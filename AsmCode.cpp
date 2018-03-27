#include "AsmCode.hpp"

int AsmCode::_offset = 0;
std::vector<AsmCode::PAsmCommand> AsmCode::_commands = {};
std::map<std::string, AsmCode::PAsmConstant> AsmCode::_constants = {};
std::map<std::string, std::pair<int, int>> AsmCode::_offsetMap = {};

const AsmCode::AsmCommandsDict_t AsmCode::_asmCommands = {
    { AsmCommands::Enter,  "enter"  },
    { AsmCommands::Push,   "push"   },
    { AsmCommands::Pop,    "pop"    },
    { AsmCommands::Lea,    "lea"    },
    { AsmCommands::Mov,    "mov"    },
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
    if (node->_type == Node::Type::Identifier && std::dynamic_pointer_cast<Identifier>(node)->isAssignment) {
        std::vector<std::string> args;
        node->generate();
        for (auto i : node->_children)
            generateStatements(i);
        args = { "eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "ebx" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
        args = { "dword ptr [ebx], eax" };
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Mov, args));
    }
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
    ss << _command << " ";
    for (auto i : _args) {
        ss << i;
        if (i != _args.back())
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
    //if (_argument->_type == Node::Type::IntConst) {
        AsmCode::addConstant(std::make_shared<AsmConstant>("__@strfmti", ConstSize::DB, PrintFormat::Integer));
        args = { "offset __@strfmti" };
        bytesToClear = "8";
    //}
    // placeholder
    /*else if (_argument->_type == Node::Type::FloatConst) {
        AsmCode::addConstant(std::make_shared<AsmConstant>("__@strfmtd", ConstSize::DQ, PrintFormat::Float));
        args = { "offset __@strfmtd" };
        bytesToClear = "16";
    }*/
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    args = { "crt_printf" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Call, args));
    args = { "esp", bytesToClear };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Add, args));
};

void WriteLn::generate() {
    std::string bytesToClear;
    std::vector<std::string> args;
    //if (_argument->_type == Node::Type::IntConst) {
        AsmCode::addConstant(std::make_shared<AsmConstant>("__@strfmtiln", ConstSize::DB, PrintFormat::IntegerLn));
        args = { "offset __@strfmtiln" };
        bytesToClear = "8";
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    //}
    // placeholder
    /*else if (_argument->_type == Node::Type::FloatConst) {
        AsmCode::addConstant(std::make_shared<AsmConstant>("__@strfmtdln", ConstSize::DQ, PrintFormat::FloatLn));
        args = { "offset __@strfmtdln" };
        bytesToClear = "16";
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Push, args));
    }*/
    args = { "crt_printf" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Call, args));
    args = { "esp", bytesToClear };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Add, args));
};

void BinaryOperator::generate() {
    std::vector<std::string> args;
    args = { "ebx" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
    args = { "eax" };
    AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Pop, args));
    args = { "eax", "ebx" };
    switch (_token._subClass) {
    case Token::SubClass::Add:
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Add, args));
        break;
    case Token::SubClass::Sub:
        AsmCode::addCommand(std::make_shared<AsmCommand>(AsmCommands::Sub, args));
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