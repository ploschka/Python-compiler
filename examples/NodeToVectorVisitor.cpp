#include "NodeToVectorVisitor.h"
#include <set>

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

void NodeToVectorVisitor::visitLeaf(Leaf *_acceptor)
{
    std::set<Type> tokens_with_values = {Type::id, Type::number, Type::string};
    std::string text;
    if (tokens_with_values.find(_acceptor->token.getType()) != tokens_with_values.end())
    {
        text = "<" + type_to_str(_acceptor->token.getType()) + ", " + _acceptor->token.getValue() + ">";
    }
    else
    {
        text = "<" + type_to_str(_acceptor->token.getType()) + ">";
    }
    this->nodes.push_back(text);
}

void NodeToVectorVisitor::visitFormalParamsNode(FormalParamsNode *_acceptor)
{
    std::string text = "FormalParams";
    this->nodes.push_back(text);
    for (auto param : _acceptor->params)
    {
        param->accept(this);
    }
}

void NodeToVectorVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    std::string text = "ActualParams";
    this->nodes.push_back(text);
    for (auto param : _acceptor->params)
    {
        param->accept(this);
    }
}

void NodeToVectorVisitor::visitVariableNode(VariableNode *_acceptor)
{
    std::string text = "Variable";
    this->nodes.push_back(text);
    for (auto param : _acceptor->chain)
    {
        param->accept(this);
    }
}

void NodeToVectorVisitor::visitCallNode(CallNode *_acceptor)
{
    std::string text = "Call";
    this->nodes.push_back(text);
    _acceptor->callable->accept(this);
    _acceptor->params->accept(this);
}

void NodeToVectorVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    std::string text = "BinaryOp (" + type_to_str(_acceptor->op->token.getType()) + ")";
    this->nodes.push_back(text);
    _acceptor->left->accept(this);
    _acceptor->right->accept(this);
}

void NodeToVectorVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    std::string text = "UnaryOp (" + type_to_str(_acceptor->op->token.getType()) + ")";
    this->nodes.push_back(text);
    _acceptor->operand->accept(this);
}

void NodeToVectorVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    std::string text = "Assignment";
    this->nodes.push_back(text);
    _acceptor->left->accept(this);
    _acceptor->right->accept(this);
}

void NodeToVectorVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    std::string text = "Return";
    this->nodes.push_back(text);
    _acceptor->return_value->accept(this);
}

void NodeToVectorVisitor::visitBlockNode(BlockNode *_acceptor)
{
    std::string text = "Block";
    this->nodes.push_back(text);
    for (auto child : _acceptor->children)
    {
        child->accept(this);
    }
}

void NodeToVectorVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    std::string text = "Program";
    this->nodes.push_back(text);
    for (auto child : _acceptor->children)
    {
        child->accept(this);
    }
}

void NodeToVectorVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
    std::string text = "Function def";
    this->nodes.push_back(text);
    _acceptor->id->accept(this);
    _acceptor->formal_params->accept(this);
    _acceptor->body->accept(this);
}

void NodeToVectorVisitor::visitElseNode(ElseNode *_acceptor)
{
    std::string text = "Else";
    this->nodes.push_back(text);
    _acceptor->body->accept(this);
}

void NodeToVectorVisitor::visitElifNode(ElifNode *_acceptor)
{
    std::string text = "Elif";
    this->nodes.push_back(text);
    _acceptor->condition->accept(this);
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr)
    {
        _acceptor->next_elif->accept(this);
    }
    else if (_acceptor->next_else != nullptr)
    {
        _acceptor->next_else->accept(this);
    }
}

void NodeToVectorVisitor::visitIfNode(IfNode *_acceptor)
{
    std::string text = "If";
    this->nodes.push_back(text);
    _acceptor->condition->accept(this);
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr)
    {
        _acceptor->next_elif->accept(this);
    }
    else if (_acceptor->next_else != nullptr)
    {
        _acceptor->next_else->accept(this);
    }
}

void NodeToVectorVisitor::visitWhileNode(WhileNode *_acceptor)
{
    std::string text = "While";
    this->nodes.push_back(text);
    _acceptor->condition->accept(this);
    _acceptor->body->accept(this);
}

void NodeToVectorVisitor::visitForNode(ForNode *_acceptor)
{
    std::string text = "For";
    this->nodes.push_back(text);
    _acceptor->iterator->accept(this);
    _acceptor->condition->accept(this);
    _acceptor->body->accept(this);
}

void NodeToVectorVisitor::visitListNode(ListNode *_acceptor) 
{
    std::string text = "List";
    this->nodes.push_back(text);
    for (auto node: _acceptor->children) 
    {
        node->accept(this);
    }
}