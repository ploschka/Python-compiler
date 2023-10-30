#pragma once

#include <pycom/interface/ASTNodeInterface.hpp>
#include <pycom/interface/VisitableNodeInterface.hpp>
#include <pycom/token/Token.hpp>

#include <vector>

class BaseASTNode : public ASTNodeInterface, public VisitableNodeInterface
{
};

// ============= Expressions =============
class ExpressionNode : public BaseASTNode
{
}; // Абстрактный класс выражения

class Leaf : public ExpressionNode
{
public:
    Token token;
    Leaf(Token _token);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitLeaf(this); };
};

// ============= Params =============
class FormalParamsNode : public BaseASTNode
{
public:
    std::vector<Leaf *> params;
    FormalParamsNode(std::vector<Leaf *> _params);
    void add_child(Leaf *_param);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitFormalParamsNode(this); };
};

class ActualParamsNode : public BaseASTNode
{
public:
    std::vector<ExpressionNode *> params;
    ActualParamsNode(std::vector<ExpressionNode *> _params);
    void add_child(ExpressionNode *_param);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitActualParamsNode(this); };
};

// ============= Expressions =============
class VariableNode : public ExpressionNode
{
public:
    std::vector<Leaf *> chain; // Цепочка обращений. Например: ID.ID.ID, либо ID, либо NUMBER
    VariableNode(std::vector<Leaf *> _chain);
    void add_to_chain(Leaf *_leaf);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitVariableNode(this); };
};

class CallNode : public ExpressionNode
{
public:
    VariableNode *callable;
    ActualParamsNode *params;
    CallNode(VariableNode *_callable, ActualParamsNode *_params);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitCallNode(this); };
};
class BinaryNode : public ExpressionNode
{
public:
    ExpressionNode *left;
    Leaf *op;
    ExpressionNode *right;
    BinaryNode(ExpressionNode *_left, Leaf *_op, ExpressionNode *_right);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitBinaryNode(this); };
};

class UnaryNode : public ExpressionNode
{
public:
    Leaf *op;
    ExpressionNode *operand;
    UnaryNode(Leaf *_op, ExpressionNode *_operand);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitUnaryNode(this); };
};

class AssignmentNode : public BaseASTNode
{
public:
    Leaf *left;
    ExpressionNode *right;
    AssignmentNode(Leaf *_left, ExpressionNode *_right);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitAssignmentNode(this); };
};

class ReturnNode : public BaseASTNode
{
public:
    ExpressionNode *return_value;
    ReturnNode(ExpressionNode *_return_value);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitReturnNode(this); };
};

// ============= Block =============

class BlockNode : public BaseASTNode
{
public:
    std::vector<BaseASTNode *> children;
    BlockNode(std::vector<BaseASTNode *> _children);
    BlockNode();

    void add_child(BaseASTNode *_child);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitBlockNode(this); };
};

class ProgramNode : public BlockNode
{
public:
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitProgramNode(this); };
    ProgramNode(std::vector<BaseASTNode *> _children);
};

// ============= Compound statements =============

class FunctionNode : public BaseASTNode
{
public:
    Leaf *id;
    FormalParamsNode *formal_params;
    BlockNode *body;
    FunctionNode(Leaf *_id, FormalParamsNode *_formal_params, BlockNode *_body);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitFunctionNode(this); };
};

class ElseNode : public BaseASTNode
{
public:
    BlockNode *body;
    ElseNode(BlockNode *_body);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitElseNode(this); };
};

class ElifNode : public BaseASTNode
{
public:
    ExpressionNode *condition;
    BlockNode *body;
    ElifNode *next_elif;
    ElseNode *next_else;
    ElifNode(ExpressionNode *_condition, BlockNode *_body);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitElifNode(this); };
};

class IfNode : public BaseASTNode
{
public:
    ExpressionNode *condition;
    BlockNode *body;
    ElifNode *next_elif;
    ElseNode *next_else;
    IfNode(ExpressionNode *_condition, BlockNode *_body);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitIfNode(this); };
};

class WhileNode : public BaseASTNode
{
public:
    ExpressionNode *condition;
    BlockNode *body;
    WhileNode(ExpressionNode *_condition, BlockNode *_body);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitWhileNode(this); };
};

class ForNode : public BaseASTNode
{
public:
    Leaf *iterator;
    ExpressionNode *condition;
    BlockNode *body;
    ForNode(Leaf *_iterator, ExpressionNode *_condition, BlockNode *_body);
    void accept(NodeVisitorInterface *_visitor) { _visitor->visitForNode(this); };
};
