#include <pycom/AST/NodePrintVisitor.h>
#include <iostream>

std::string type_to_str(Type type)
{
    // Переводит enum Type в строку. Лучше функции не придумаешь.
    switch (type)
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

void PrintVisitor::print_line(std::string text)
{
    // Выводит text в консоль с учётом текущего отступа
    for (int i = 0; i < this->indent; i++)
    {
        std::cout << this->indent_str;
    }
    std::cout << text;
    std::cout << "\n";
}

void PrintVisitor::visitLeaf(Leaf *acceptor)
{
    std::string text = "<" + type_to_str(acceptor->token.getType()) + ", " + acceptor->token.getValue() + ">";
    this->print_line(text);
}

void PrintVisitor::visitFormalParamsNode(FormalParamsNode *acceptor)
{
    std::string text = "FormalParams";
    this->print_line(text);
    this->indent++;
    for (auto param : acceptor->params)
    {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitActualParamsNode(ActualParamsNode *acceptor)
{
    std::string text = "ActualParams";
    this->print_line(text);
    this->indent++;
    for (auto param : acceptor->params)
    {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitVariableNode(VariableNode *acceptor)
{
    std::string text = "Variable";
    this->print_line(text);
    this->indent++;
    for (auto param : acceptor->chain)
    {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitCallNode(CallNode *acceptor)
{
    std::string text = "Call";
    this->print_line(text);
    this->indent++;
    this->print_line("callable:");
    this->indent++;
    acceptor->callable->accept(this);
    this->indent--;
    this->print_line("params:");
    this->indent++;
    acceptor->params->accept(this);
    this->indent--;
    this->indent--;
}

void PrintVisitor::visitBinaryNode(BinaryNode *acceptor)
{
    std::string text = "BinaryOp (" + type_to_str(acceptor->op->token.getType()) + ")";
    this->print_line(text);
    this->indent++;
    acceptor->left->accept(this);
    acceptor->right->accept(this);
    this->indent--;
}

void PrintVisitor::visitUnaryNode(UnaryNode *acceptor)
{
    std::string text = "UnaryOp (" + acceptor->op->token.getValue() + ")";
    this->print_line(text);
    this->indent++;
    acceptor->operand->accept(this);
    this->indent--;
}

void PrintVisitor::visitAssignmentNode(AssignmentNode *acceptor)
{
    std::string text = "Assignment";
    this->print_line(text);
    this->indent++;
    acceptor->left->accept(this);
    acceptor->right->accept(this);
    this->indent--;
}

void PrintVisitor::visitReturnNode(ReturnNode *acceptor)
{
    std::string text = "Return";
    this->print_line(text);
    this->indent++;
    acceptor->return_value->accept(this);
    this->indent--;
}

void PrintVisitor::visitBlockNode(BlockNode *acceptor)
{
    std::string text = "Block";
    this->print_line(text);
    this->indent++;
    for (auto child : acceptor->children)
    {
        child->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitProgramNode(ProgramNode *acceptor)
{
    std::string text = "Program";
    this->print_line(text);
    this->indent++;
    for (auto child : acceptor->children)
    {
        child->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitFunctionNode(FunctionNode *acceptor)
{
    std::string text = "Function def";
    this->print_line(text);
    this->indent++;
    this->print_line("name:");
    this->indent++;
    acceptor->id->accept(this);
    this->indent--;
    acceptor->formal_params->accept(this);
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitElseNode(ElseNode *acceptor)
{
    std::string text = "Else";
    this->print_line(text);
    this->indent++;
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitElifNode(ElifNode *acceptor)
{
    std::string text = "Elif";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    if (acceptor->next_elif != nullptr)
    {
        acceptor->next_elif->accept(this);
    }
    else if (acceptor->next_else != nullptr)
    {
        acceptor->next_else->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitIfNode(IfNode *acceptor)
{
    std::string text = "If";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    if (acceptor->next_elif != nullptr)
    {
        acceptor->next_elif->accept(this);
    }
    else if (acceptor->next_else != nullptr)
    {
        acceptor->next_else->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitWhileNode(WhileNode *acceptor)
{
    std::string text = "While";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitForNode(ForNode *acceptor)
{
    std::string text = "For";
    this->print_line(text);
    this->indent++;
    this->print_line("iterator:");
    acceptor->iterator->accept(this);
    this->indent++;
    this->indent--;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    this->indent--;
}
