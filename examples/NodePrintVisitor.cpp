#include "NodePrintVisitor.h"
#include <iostream>

// Переводит enum Type в строку. Лучше функции не придумаешь.
static inline std::string type_to_str(Type _type)
{
    switch (_type)
    {
    case Type::id:
        return "id";
    case Type::number:
        return "number";
    case Type::string:
        return "string";
    case Type::lpr:
        return "lpr";
    case Type::rpr:
        return "rpr";
    case Type::lsbr:
        return "lsbr";
    case Type::rsbr:
        return "rsbr";
    case Type::lbr:
        return "lbr";
    case Type::rbr:
        return "rbr";
    case Type::colon:
        return "colon";
    case Type::plus:
        return "plus";
    case Type::minus:
        return "minus";
    case Type::star:
        return "star";
    case Type::div:
        return "div";
    case Type::mod:
        return "mod";
    case Type::idiv:
        return "idiv";
    case Type::matmul:
        return "matmul";
    case Type::dot:
        return "dot";
    case Type::greater:
        return "greater";
    case Type::less:
        return "less";
    case Type::equal:
        return "equal";
    case Type::noteq:
        return "noteq";
    case Type::grequal:
        return "grequal";
    case Type::lequal:
        return "lequal";
    case Type::notop:
        return "notop";
    case Type::in:
        return "in";
    case Type::is:
        return "is";
    case Type::andop:
        return "andop";
    case Type::orop:
        return "orop";
    case Type::inv:
        return "inv";
    case Type::lshift:
        return "lshift";
    case Type::rshift:
        return "rshift";
    case Type::band:
        return "band";
    case Type::bor:
        return "bor";
    case Type::xorop:
        return "xorop";
    case Type::assign:
        return "assign";
    case Type::plusass:
        return "plusass";
    case Type::minass:
        return "minass";
    case Type::mulass:
        return "mulass";
    case Type::divass:
        return "divass";
    case Type::modass:
        return "modass";
    case Type::idivass:
        return "idivass";
    case Type::matmulass:
        return "matmulass";
    case Type::lshiftass:
        return "lshiftass";
    case Type::rshiftass:
        return "rshiftass";
    case Type::bandass:
        return "bandass";
    case Type::borass:
        return "borass";
    case Type::xorass:
        return "xorass";
    case Type::arrow:
        return "arrow";
    case Type::defkw:
        return "defkw";
    case Type::ifkw:
        return "ifkw";
    case Type::elifkw:
        return "elifkw";
    case Type::elsekw:
        return "elsekw";
    case Type::forkw:
        return "forkw";
    case Type::whilekw:
        return "whilekw";
    case Type::classkw:
        return "classkw";
    case Type::yieldkw:
        return "yieldkw";
    case Type::returnkw:
        return "returnkw";
    case Type::continuekw:
        return "continuekw";
    case Type::passkw:
        return "passkw";
    case Type::breakkw:
        return "breakkw";
    case Type::indent:
        return "indent";
    case Type::dedent:
        return "dedent";
    case Type::newline:
        return "newline";
    case Type::eof:
        return "eof";
    case Type::comma:
        return "comma";
    case Type::unexpected:
        return "unexpected";
    case Type::indenterror:
        return "indenterror";
    case Type::tabspacemix:
        return "tabspacemix";
    default:
        return "␜⌾ⴶⶪ⤡₀⍣⁰";
    }
}

void PrintVisitor::print_line(std::string _text)
{
    // Выводит text в консоль с учётом текущего отступа
    for (int i = 0; i < this->indent; i++)
    {
        std::cout << this->indent_str;
    }
    std::cout << _text;
    std::cout << "\n";
}

void PrintVisitor::visitLeaf(Leaf *_acceptor)
{
    std::string text = "<" + type_to_str(_acceptor->token.getType()) + ", " + _acceptor->token.getValue() + ">";
    this->print_line(text);
}

void PrintVisitor::visitFormalParamsNode(FormalParamsNode *_acceptor)
{
    std::string text = "FormalParams";
    this->print_line(text);
    this->indent++;
    this->print_line("Params:");
    this->indent++;
    for (auto param : _acceptor->params)
    {
        param->accept(this);
    }
    this->indent--;
    this->print_line("Types:");
    this->indent++;
    for (auto type : _acceptor->types)
    {
        type->accept(this);
    }
    this->indent--;
    this->indent--;
}

void PrintVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    std::string text = "ActualParams";
    this->print_line(text);
    this->indent++;
    for (auto param : _acceptor->params)
    {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitCallNode(CallNode *_acceptor)
{
    std::string text = "Call";
    this->print_line(text);
    this->indent++;
    this->print_line("callable:");
    this->indent++;
    this->print_line("<" + type_to_str(_acceptor->callable.getType()) + ", " + _acceptor->callable.getValue() + ">");
    this->indent--;
    this->print_line("params:");
    this->indent++;
    _acceptor->params->accept(this);
    this->indent--;
    this->indent--;
}

void PrintVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    std::string text = "BinaryOp (" + type_to_str(_acceptor->op->token.getType()) + ")";
    this->print_line(text);
    this->indent++;
    _acceptor->left->accept(this);
    _acceptor->right->accept(this);
    this->indent--;
}

void PrintVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    std::string text = "UnaryOp (" + type_to_str(_acceptor->op->token.getType()) + ")";
    this->print_line(text);
    this->indent++;
    _acceptor->operand->accept(this);
    this->indent--;
}

void PrintVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    std::string text = "Assignment";
    this->print_line(text);
    this->indent++;
    _acceptor->left->accept(this);
    if (_acceptor->type != nullptr) {
        _acceptor->type->accept(this);        
    }
    _acceptor->right->accept(this);
    this->indent--;
}

void PrintVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    std::string text = "Return";
    this->print_line(text);
    this->indent++;
    _acceptor->return_value->accept(this);
    this->indent--;
}

void PrintVisitor::visitBlockNode(BlockNode *_acceptor)
{
    std::string text = "Block";
    this->print_line(text);
    this->indent++;
    for (auto child : _acceptor->children)
    {
        child->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    std::string text = "Program";
    this->print_line(text);
    this->indent++;
    for (auto child : _acceptor->children)
    {
        child->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
    std::string text = "Function def";
    this->print_line(text);
    this->indent++;
    this->print_line("name:");
    this->indent++;
    _acceptor->id->accept(this);
    this->indent--;
    _acceptor->formal_params->accept(this);
    _acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitElseNode(ElseNode *_acceptor)
{
    std::string text = "Else";
    this->print_line(text);
    this->indent++;
    _acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitElifNode(ElifNode *_acceptor)
{
    std::string text = "Elif";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    _acceptor->condition->accept(this);
    this->indent--;
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr)
    {
        _acceptor->next_elif->accept(this);
    }
    else if (_acceptor->next_else != nullptr)
    {
        _acceptor->next_else->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitIfNode(IfNode *_acceptor)
{
    std::string text = "If";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    _acceptor->condition->accept(this);
    this->indent--;
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr)
    {
        _acceptor->next_elif->accept(this);
    }
    else if (_acceptor->next_else != nullptr)
    {
        _acceptor->next_else->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitWhileNode(WhileNode *_acceptor)
{
    std::string text = "While";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    _acceptor->condition->accept(this);
    this->indent--;
    _acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitForNode(ForNode *_acceptor)
{
    std::string text = "For";
    this->print_line(text);
    this->indent++;
    this->print_line("iterator:");
    _acceptor->iterator->accept(this);
    this->indent++;
    this->indent--;
    this->print_line("condition:");
    this->indent++;
    _acceptor->condition->accept(this);
    this->indent--;
    _acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitListNode(ListNode *_acceptor) 
{
    std::string text = "List:";
    this->print_line(text);
    this->indent++;
    for (auto node: _acceptor->children) 
    {
        node->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitTypeNode(TypeNode *_acceptor)
{
    std::string text = "Type: <" + _acceptor->token.getValue() + ">";
    this->print_line(text);
}