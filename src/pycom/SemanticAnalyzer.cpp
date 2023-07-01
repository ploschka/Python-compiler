#include <pycom/semanalyzer/SemanticAnalyzer.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <stdexcept>
#include <iostream>
#include <map>


SemanticAnalyzer::SemanticAnalyzer() {
    std::map<std::string, Symbol> builtins_data {
        {"bool", SymbolFunction("bool", {Symbol()}, SymbolBool())}
    };
    
    SymbolTable builtins(builtins_data);
    SymbolTable globals({});

    this->tableStack.push(builtins);
    this->tableStack.push(globals);
};


void SemanticAnalyzer::visitLeaf(Leaf *acceptor)
{
    if (acceptor->token.getType() == Type::id) {
        this->tableStack.addId(acceptor->token.getValue(), Symbol());
    }
};


void SemanticAnalyzer::visitFormalParamsNode(FormalParamsNode *acceptor)
{
    for (auto param : acceptor->params)
    {
        param->accept(this);
    }
};


void SemanticAnalyzer::visitActualParamsNode(ActualParamsNode *acceptor)
{
    for (auto param : acceptor->params)
    {
        param->accept(this);
    }
};


void SemanticAnalyzer::visitVariableNode(VariableNode *acceptor)
{
    for (auto param : acceptor->chain)
    {
        param->accept(this);
    }
};


// Сделать проверку вызовов функции
void SemanticAnalyzer::visitCallNode(CallNode *acceptor)
{
    acceptor->callable->accept(this);
    acceptor->params->accept(this);
};


// Сделать проверку операндов на соответствие типов
void SemanticAnalyzer::visitBinaryNode(BinaryNode *acceptor)
{
    acceptor->left->accept(this);
    acceptor->right->accept(this);
};


// Сделать проверку операнда на соответствие типов
void SemanticAnalyzer::visitUnaryNode(UnaryNode *acceptor)
{
    acceptor->operand->accept(this);
};


// Сделать проверку левой части
void SemanticAnalyzer::visitAssignmentNode(AssignmentNode *acceptor)
{
    acceptor->left->accept(this);
    acceptor->right->accept(this);
};


void SemanticAnalyzer::visitReturnNode(ReturnNode *acceptor)
{
    acceptor->return_value->accept(this);
};


void SemanticAnalyzer::visitBlockNode(BlockNode *acceptor)
{
    for (auto child : acceptor->children)
    {
        child->accept(this);
    }
};


// СДЕЛАТЬ СЕЙЧАС! Нужно добавление новой таблицы в стек
void SemanticAnalyzer::visitFunctionNode(FunctionNode *acceptor)
{
    acceptor->id->accept(this);
    acceptor->formal_params->accept(this);
    // Здесь будет добавление функции в таблицу
    // Здесь будет добавление новой таблицы в стек
    acceptor->body->accept(this);
    // Здесь будет удаление таблицы в стек
};


void SemanticAnalyzer::visitElseNode(ElseNode *acceptor)
{
    acceptor->body->accept(this);
};


void SemanticAnalyzer::visitElifNode(ElifNode *acceptor)
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


void SemanticAnalyzer::visitIfNode(IfNode *acceptor)
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


void SemanticAnalyzer::visitWhileNode(WhileNode *acceptor)
{
    acceptor->condition->accept(this);
    acceptor->body->accept(this);
};


// Сделать добавление итератора в таблицу символов
void SemanticAnalyzer::visitForNode(ForNode *acceptor)
{
    acceptor->iterator->accept(this);
    // Здесь будет добавление итератора в таблицу
    acceptor->condition->accept(this);
    acceptor->body->accept(this);
};


void SemanticAnalyzer::visitProgramNode(ProgramNode* acceptor)
{
    for (auto child : acceptor->children)
    {
        child->accept(this);
    }
};
