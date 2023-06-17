#pragma once

#include <pycom/interface/ASTNodeInterface.hpp>
#include <pycom/interface/VisitableNodeInterface.hpp>
#include <pycom/token/Token.hpp>
#include <vector>

class BaseASTNode : public ASTNodeInterface, public VisitableNodeInterface {
};


// ============= Expressions =============
class ExpressionNode : public BaseASTNode {
};  // Абстрактный класс выражения

class Leaf : public ExpressionNode {
public:
    Token token;
    Leaf(Token token);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitLeaf(this);};
};


// ============= Params =============
class FormalParamsNode : public BaseASTNode {
public:
    std::vector<Leaf*> params;
    FormalParamsNode(std::vector<Leaf*> params);
    void add_child(Leaf* param);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitFormalParamsNode(this);};
};

class ActualParamsNode : public BaseASTNode {
public:
    std::vector<ExpressionNode*> params;
    ActualParamsNode(std::vector<ExpressionNode*> params);
    void add_child(ExpressionNode* param);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitActualParamsNode(this);};
};


// ============= Expressions =============
class VariableNode: public ExpressionNode {
public:
    std::vector<Leaf*> chain;  // Цепочка обращений. Например: ID.ID.ID, либо ID, либо NUMBER
    VariableNode(std::vector<Leaf*> chain);
    void add_to_chain(Leaf * leaf);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitVariableNode(this);};
};

class CallNode : public ExpressionNode {
public:
    VariableNode* callable;
    ActualParamsNode* params;
    CallNode(VariableNode* callable, ActualParamsNode* params);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitCallNode(this);};
};
class BinaryNode : public ExpressionNode {
public:
    ExpressionNode* left;
    Leaf* op;
    ExpressionNode* right;
    BinaryNode(ExpressionNode* left, Leaf *op, ExpressionNode* right);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitBinaryNode(this);};
};

class UnaryNode : public ExpressionNode {
public:
    Leaf* op;
    ExpressionNode* operand;
    UnaryNode(Leaf *op, ExpressionNode* operand);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitUnaryNode(this);};
};

class AssignmentNode : public BaseASTNode {
public:
    Leaf* left;
    ExpressionNode* right;
    AssignmentNode(Leaf* left, ExpressionNode* right);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitAssignmentNode(this);};
};

class ReturnNode : public BaseASTNode {
public:
    ExpressionNode* return_value;
    ReturnNode(ExpressionNode* return_value);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitReturnNode(this);};
};


// ============= Block =============

class BlockNode : public BaseASTNode {
public:
    std::vector<BaseASTNode*> children;
    BlockNode(std::vector<BaseASTNode*> children);
    BlockNode();

    void add_child(BaseASTNode* child);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitBlockNode(this);};
};

class ProgramNode : public BlockNode {
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitProgramNode(this);};
};

// ============= Compound statements =============

class FunctionNode : public BaseASTNode {
public:
    Leaf* id;
    FormalParamsNode* formal_params;
    BlockNode* body;
    FunctionNode(Leaf* id, FormalParamsNode* formal_params, BlockNode* body);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitFunctionNode(this);};
};

class ElseNode : public BaseASTNode {
public:
    BlockNode* body;
    ElseNode(BlockNode *body);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitElseNode(this);};
};

class ElifNode : public BaseASTNode {
public:
    ExpressionNode* condition;
    BlockNode* body;
    ElifNode* next_elif;
    ElseNode* next_else;
    ElifNode(ExpressionNode* condition, BlockNode* body);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitElifNode(this);};
};

class IfNode : public BaseASTNode {
public:
    ExpressionNode* condition;
    BlockNode* body;
    ElifNode* next_elif;
    ElseNode* next_else;
    IfNode(ExpressionNode* condition, BlockNode* body);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitIfNode(this);};
};


class WhileNode : public BaseASTNode {
public:
    ExpressionNode* condition;
    BlockNode* body;
    WhileNode(ExpressionNode* condition, BlockNode* body);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitWhileNode(this);};
};

class ForNode : public BaseASTNode {
public:
    Leaf* iterator;
    ExpressionNode* condition;
    BlockNode* body;
    ForNode(Leaf* iterator, ExpressionNode* condition, BlockNode* body);
    void accept(NodeVisitorInterface *_visitor) {_visitor->visitForNode(this);};
};

