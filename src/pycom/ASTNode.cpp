#include <pycom/AST/ASTNode.hpp>

Leaf::Leaf(Token token) : token(token) {}

VariableNode::VariableNode(std::vector<Leaf*> chain) : chain(chain) {}

void VariableNode::add_to_chain(Leaf *leaf) {
    this->chain.push_back(leaf);
}

ActualParamsNode::ActualParamsNode(std::vector<ExpressionNode*> params) : params(params) {}

void ActualParamsNode::add_child(ExpressionNode *param) {
    this->params.push_back(param);
}

CallNode::CallNode(VariableNode* callable, ActualParamsNode* params) : callable(callable), params(params) {}

BinaryNode::BinaryNode(ExpressionNode* left, Leaf *op, ExpressionNode* right) : left(left), op(op), right(right) {}

UnaryNode::UnaryNode(Leaf *op, ExpressionNode* operand) : op(op), operand(operand) {}

AssignmentNode::AssignmentNode(Leaf* left, ExpressionNode* right) : left(left), right(right) {}

ReturnNode::ReturnNode(ExpressionNode* return_value) : return_value(return_value) {}

BlockNode::BlockNode(std::vector<BaseASTNode*> children) : children(children) {}
BlockNode::BlockNode(): children({}) {}

FunctionNode::FunctionNode(Leaf* id, FormalParamsNode* formal_params, BlockNode* body) : id(id),
                                                                                       formal_params(formal_params),
                                                                                       body(body) {}

IfNode::IfNode(ExpressionNode* condition, BlockNode* body) : condition(condition), body(body) {
    this->next_elif = nullptr;
    this->next_else = nullptr;
}

ElifNode::ElifNode(ExpressionNode* condition, BlockNode* body) : condition(condition), body(body) {
    this->next_elif = nullptr;
    this->next_else = nullptr;
}

ElseNode::ElseNode(BlockNode *body) : body(body) {}

WhileNode::WhileNode(ExpressionNode* condition, BlockNode* body) : condition(condition), body(body) {}

ForNode::ForNode(Leaf* iterator, ExpressionNode* condition, BlockNode* body) : iterator(iterator), condition(condition),
                                                                             body(body) {}

void BlockNode::add_child(BaseASTNode* child) {
    this->children.push_back(child);
}

FormalParamsNode::FormalParamsNode(std::vector<Leaf *> params): params(params) {}

void FormalParamsNode::add_child(Leaf *param) {
    this->params.push_back(param);
}
