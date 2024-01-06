#include <pycom/AST/ASTNode.hpp>

Leaf::Leaf(Token _token) : token(_token) {}

TypeNode::TypeNode(Token _type_token) : token(_type_token) {}

ActualParamsNode::ActualParamsNode(std::vector<ExpressionNode *> _params) : params(_params) {}

void ActualParamsNode::add_child(ExpressionNode *_param)
{
    this->params.push_back(_param);
}

CallNode::CallNode(ExpressionNode *_callable, ActualParamsNode *_params) : callable(_callable), params(_params) {}

BinaryNode::BinaryNode(ExpressionNode *_left, Leaf *_op, ExpressionNode *_right) : left(_left), op(_op), right(_right) {}

UnaryNode::UnaryNode(Leaf *_op, ExpressionNode *_operand) : op(_op), operand(_operand) {}

AssignmentNode::AssignmentNode(Leaf *_left, TypeNode *_type, ExpressionNode *_right) : left(_left), type(_type), right(_right) {}
AssignmentNode::AssignmentNode(Leaf *_left, ExpressionNode *_right) : left(_left), type(nullptr), right(_right) {}

ReturnNode::ReturnNode(ExpressionNode *_return_value) : return_value(_return_value) {}

BlockNode::BlockNode(std::vector<BaseASTNode *> _children) : children(_children) {}
BlockNode::BlockNode() : children({}) {}

FunctionNode::FunctionNode(Leaf *_id, FormalParamsNode *_formal_params, BlockNode *_body) : id(_id),
                                                                                            formal_params(_formal_params),
                                                                                            body(_body) {}

IfNode::IfNode(ExpressionNode *_condition, BlockNode *_body) : condition(_condition), body(_body)
{
    this->next_elif = nullptr;
    this->next_else = nullptr;
}

ElifNode::ElifNode(ExpressionNode *_condition, BlockNode *_body) : condition(_condition), body(_body)
{
    this->next_elif = nullptr;
    this->next_else = nullptr;
}

ElseNode::ElseNode(BlockNode *_body) : body(_body) {}

WhileNode::WhileNode(ExpressionNode *_condition, BlockNode *_body) : condition(_condition), body(_body) {}

ForNode::ForNode(Leaf *_iterator, ExpressionNode *_condition, BlockNode *_body) : iterator(_iterator), condition(_condition),
                                                                                  body(_body) {}

void BlockNode::add_child(BaseASTNode *_child)
{
    this->children.push_back(_child);
}

FormalParamsNode::FormalParamsNode(std::vector<Leaf *> _params, std::vector<TypeNode* > _types) : params(_params), types(_types) {}

void FormalParamsNode::add_child(Leaf *_param, TypeNode *_type)
{
    this->params.push_back(_param);
    this->types.push_back(_type);
}

ProgramNode::ProgramNode(std::vector<BaseASTNode *> _children) {
    this->children = _children;
}

ListNode::ListNode(std::vector<ExpressionNode *> _values): children(_values) {}
void ListNode::append(ExpressionNode *_expr) {
    this->children.push_back(_expr);
}
