#pragma once

#include <pycom/interface/ASTNodeInterface.hpp>
#include <pycom/interface/VisitableNodeInterface.hpp>
#include <pycom/token/Token.hpp>
#include <vector>

class BaseASTNode : public ASTNodeInterface, public VisitableNodeInterface {
public:
    void accept(NodeVisitorInterface *_visitor) override {};
};


// ============= Expressions =============
class ExpressionNode : public BaseASTNode {
};  // Абстрактный класс выражения

class Leaf : public ExpressionNode {
public:
    Token token;
    Leaf(Token token);
};


// ============= Params =============
class FormalParamsNode : public BaseASTNode {
public:
    std::vector<Leaf*> params;
    FormalParamsNode(std::vector<Leaf*> params);
    void add_child(Leaf* param);
};

class ActualParamsNode : public BaseASTNode {
public:
    std::vector<ExpressionNode*> params;
    ActualParamsNode(std::vector<ExpressionNode*> params);
    void add_child(ExpressionNode* param);
};


// ============= Expressions =============
class VariableNode: public ExpressionNode {
public:
    std::vector<Leaf*> chain;  // Цепочка обращений. Например: ID.ID.ID, либо ID, либо NUMBER
    VariableNode(std::vector<Leaf*> chain);
    void add_to_chain(Leaf * leaf);
};

class CallNode : public ExpressionNode {
public:
    VariableNode* callable;
    ActualParamsNode* params;
    CallNode(VariableNode* callable, ActualParamsNode* params);
};

class BinaryNode : public ExpressionNode {
public:
    ExpressionNode* left;
    Leaf* op;
    ExpressionNode* right;
    BinaryNode(ExpressionNode* left, Leaf *op, ExpressionNode* right);
};

class UnaryNode : public ExpressionNode {
public:
    Leaf* op;
    ExpressionNode* operand;
    UnaryNode(Leaf *op, ExpressionNode* operand);
};

class AssignmentNode : public BaseASTNode {
public:
    Leaf* left;
    ExpressionNode* right;
    AssignmentNode(Leaf* left, ExpressionNode* right);
};

class ReturnNode : public BaseASTNode {
public:
    ExpressionNode* return_value;
    ReturnNode(ExpressionNode* return_value);
};


// ============= Block =============

class BlockNode : public BaseASTNode {
public:
    std::vector<BaseASTNode*> children;
    BlockNode(std::vector<BaseASTNode*> children);
    BlockNode();

    void add_child(BaseASTNode* child);
};
class ProgramNode : public BlockNode {};

// ============= Compound statements =============

class FunctionNode : public BaseASTNode {
public:
    Leaf* id;
    FormalParamsNode* formal_params;
    BlockNode* body;
    FunctionNode(Leaf* id, FormalParamsNode* formal_params, BlockNode* body);
};

class ElseNode : public BaseASTNode {
public:
    BlockNode* body;
    ElseNode(BlockNode *body);
};

class ElifNode : public BaseASTNode {
public:
    ExpressionNode* condition;
    BlockNode* body;
    ElifNode* next_elif;
    ElseNode* next_else;
    ElifNode(ExpressionNode* condition, BlockNode* body);
};

class IfNode : public BaseASTNode {
public:
    ExpressionNode* condition;
    BlockNode* body;
    ElifNode* next_elif;
    ElseNode* next_else;
    IfNode(ExpressionNode* condition, BlockNode* body);
};


class WhileNode : public BaseASTNode {
public:
    ExpressionNode* condition;
    BlockNode* body;
    WhileNode(ExpressionNode* condition, BlockNode* body);
};

class ForNode : public BaseASTNode {
public:
    Leaf* iterator;
    ExpressionNode* condition;
    BlockNode* body;
    ForNode(Leaf* iterator, ExpressionNode* condition, BlockNode* body);
};

