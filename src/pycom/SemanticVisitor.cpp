#include <pycom/semanalyzer/SemanticVisitor.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>

#include <stdexcept>
#include <iostream>
#include <map>

SemanticVisitor::SemanticVisitor()
{
    std::map<std::string, Symbol> builtins_data{
        {"bool", SymbolFunction("bool", {Symbol()}, SymbolBool())}};

    SymbolTable builtins(builtins_data);
    SymbolTable globals({});

    this->tableStack.push(builtins);
    this->tableStack.push(globals);
};

void SemanticVisitor::visitLeaf(Leaf *acceptor)
{
    if (acceptor->token.getType() == Type::id)
    {
        this->tableStack.addId(acceptor->token.getValue(), Symbol());
    }
};

void SemanticVisitor::visitFormalParamsNode(FormalParamsNode *acceptor)
{
    for (auto param : acceptor->params)
    {
        param->accept(this);
    }
};

void SemanticVisitor::visitActualParamsNode(ActualParamsNode *acceptor)
{
    for (auto param : acceptor->params)
    {
        param->accept(this);
    }
};

void SemanticVisitor::visitVariableNode(VariableNode *acceptor)
{
    for (auto param : acceptor->chain)
    {
        param->accept(this);
    }
};

// Сделать проверку вызовов функции
void SemanticVisitor::visitCallNode(CallNode *acceptor)
{
    acceptor->callable->accept(this);
    acceptor->params->accept(this);
};

// Сделать проверку операндов на соответствие типов
void SemanticVisitor::visitBinaryNode(BinaryNode *acceptor)
{
    acceptor->left->accept(this);
    acceptor->right->accept(this);
};

// Сделать проверку операнда на соответствие типов
void SemanticVisitor::visitUnaryNode(UnaryNode *acceptor)
{
    acceptor->operand->accept(this);
};

// Сделать проверку левой части
void SemanticVisitor::visitAssignmentNode(AssignmentNode *acceptor)
{
    acceptor->left->accept(this);
    acceptor->right->accept(this);
};

void SemanticVisitor::visitReturnNode(ReturnNode *acceptor)
{
    acceptor->return_value->accept(this);
};

void SemanticVisitor::visitBlockNode(BlockNode *acceptor)
{
    for (auto child : acceptor->children)
    {
        child->accept(this);
    }
};

// СДЕЛАТЬ СЕЙЧАС! Нужно добавление новой таблицы в стек
void SemanticVisitor::visitFunctionNode(FunctionNode *acceptor)
{
    acceptor->id->accept(this);
    acceptor->formal_params->accept(this);
    // Здесь будет добавление функции в таблицу
    // Здесь будет добавление новой таблицы в стек
    acceptor->body->accept(this);
    // Здесь будет удаление таблицы в стек
};

void SemanticVisitor::visitElseNode(ElseNode *acceptor)
{
    acceptor->body->accept(this);
};

void SemanticVisitor::visitElifNode(ElifNode *acceptor)
{
    acceptor->condition->accept(this);
    acceptor->body->accept(this);
    if (acceptor->next_elif != nullptr)
    {
        acceptor->next_elif->accept(this);
    }
    else if (acceptor->next_else != nullptr)
    {
        acceptor->next_else->accept(this);
    }
};

void SemanticVisitor::visitIfNode(IfNode *acceptor)
{
    acceptor->condition->accept(this);
    acceptor->body->accept(this);
    if (acceptor->next_elif != nullptr)
    {
        acceptor->next_elif->accept(this);
    }
    else if (acceptor->next_else != nullptr)
    {
        acceptor->next_else->accept(this);
    }
};

void SemanticVisitor::visitWhileNode(WhileNode *acceptor)
{
    acceptor->condition->accept(this);
    acceptor->body->accept(this);
};

// Сделать добавление итератора в таблицу символов
void SemanticVisitor::visitForNode(ForNode *acceptor)
{
    acceptor->iterator->accept(this);
    // Здесь будет добавление итератора в таблицу
    acceptor->condition->accept(this);
    acceptor->body->accept(this);
};

void SemanticVisitor::visitProgramNode(ProgramNode *acceptor)
{
    for (auto child : acceptor->children)
    {
        child->accept(this);
    }
};

bool SemanticVisitor::getResult() const
{
    return result;
}

void SemanticVisitor::reset()
{
    // Раскрутить стек
}
