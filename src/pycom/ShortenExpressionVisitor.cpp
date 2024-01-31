#include <pycom/optimizer/ShortenExpressionVisitor.hpp>

bool ShortenExpressionVisitor::is_var_constant(std::string id)
{
    for (int i = this->scopes_stack.size() - 1; i >= 0; i--)
    {
        auto scope = this->scopes_stack[i];
        bool scope_has_var = scope.variables.find(id) != scope.variables.end();
        if (scope_has_var)
        {
            return scope.is_var_constant.at(id);
        }
    }
    return false;
}

ExpressionNode *ShortenExpressionVisitor::get_var(std::string id)
{
    for (int i = this->scopes_stack.size() - 1; i >= 0; i--)
    {
        auto scope = this->scopes_stack[i];
        bool scope_has_var = scope.variables.find(id) != scope.variables.end();
        if (scope_has_var)
        {
            return scope.variables.at(id);
        }
    }
    return nullptr;
}

void ShortenExpressionVisitor::set_var(std::string id, ExpressionNode *node, bool is_constant)
{
    auto scope = this->scopes_stack[this->scopes_stack.size() - 1];
    scope.variables.insert({id, node});
    scope.is_var_constant.insert({id, is_constant});
}

void ShortenExpressionVisitor::add_scope()
{
    this->scopes_stack.push_back(VariableTable());
}

void ShortenExpressionVisitor::remove_scope()
{
    this->scopes_stack.pop_back();
}

void ShortenExpressionVisitor::visitLeaf(Leaf *_acceptor)
{
    std::string id = _acceptor->token.getValue();
    Type type = _acceptor->token.getType();
    if (type == Type::id)
    {  // Если переменная
        if (this->is_var_constant(id))
        {  // Если переменная - константа
            this->shortenedExpr = this->get_var(id);
            this->is_constant = true;
        }
        else 
        {
            this->shortenedExpr = _acceptor;
            this->is_constant = false;
        }
    }
    else 
    {  // Если какое-то значение, вроде числа или строки
        this->shortenedExpr = _acceptor;
        this->is_constant = true;
    }
}

void ShortenExpressionVisitor::visitBlockNode(BlockNode *_acceptor)
{
    for (unsigned int i = 0; i < _acceptor->children.size(); i++)
    {
        auto child = _acceptor->children[i];
        child->accept(this);
        if (this->is_constant)
        {
            _acceptor->children.erase(_acceptor->children.begin() + i);
            i--;
        }
    }
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitCallNode(CallNode *_acceptor)
{
    _acceptor->params->accept(this);
    this->shortenedExpr = _acceptor;
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    _acceptor->left->accept(this);
    _acceptor->left = this->shortenedExpr;
    bool left_is_const = this->is_constant;

    _acceptor->right->accept(this);
    _acceptor->right = this->shortenedExpr;
    bool right_is_const = this->is_constant;

    if (left_is_const && right_is_const)
    {
        _acceptor->accept(this->calculator);
        std::string calc_value = this->calculator->return_result;
        Type type = this->calculator->return_type;
        this->shortenedExpr = new Leaf(Token(calc_value, type));
        this->is_constant = true;
    }
    else
    {
        this->shortenedExpr = _acceptor;
        this->is_constant = false;
    }
}

void ShortenExpressionVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    _acceptor->operand->accept(this);
    _acceptor->operand = this->shortenedExpr;
    bool value_is_const = this->is_constant;
    if (value_is_const)
    {
        _acceptor->accept(this->calculator);
        std::string calc_value = this->calculator->return_result;
        Type type = this->calculator->return_type;
        this->shortenedExpr = new Leaf(Token(calc_value, type));
        this->is_constant = true;
    }
    else
    {
        this->shortenedExpr = _acceptor;
        this->is_constant = false;
    }
}

void ShortenExpressionVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    _acceptor->right->accept(this);
    _acceptor->right = shortenedExpr;
    std::string id = _acceptor->left->token.getValue();
    this->set_var(id, this->shortenedExpr, this->is_constant);
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    _acceptor->return_value->accept(this);
    _acceptor->return_value = this->shortenedExpr;
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    this->add_scope();
    this->visitBlockNode(_acceptor);
    this->remove_scope();
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
    this->add_scope();
    _acceptor->body->accept(this);
    this->remove_scope();
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitElseNode(ElseNode *_acceptor)
{
    _acceptor->body->accept(this);
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitElifNode(ElifNode *_acceptor)
{
    _acceptor->condition->accept(this);
    _acceptor->condition = this->shortenedExpr;
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr) {
        _acceptor->next_elif->accept(this);
    }
    if (_acceptor->next_else != nullptr) {
        _acceptor->next_else->accept(this);
    }
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitIfNode(IfNode *_acceptor)
{
    _acceptor->condition->accept(this);
    _acceptor->condition = this->shortenedExpr;
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr) {
        _acceptor->next_elif->accept(this);
    }
    if (_acceptor->next_else != nullptr) {
        _acceptor->next_else->accept(this);
    }
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitWhileNode(WhileNode *_acceptor)
{
    _acceptor->condition->accept(this);
    _acceptor->condition = this->shortenedExpr;
    _acceptor->body->accept(this);
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitForNode(ForNode *_acceptor)
{
    _acceptor->body->accept(this);
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitListNode(ListNode *_acceptor)
{
    for (unsigned int i = 0; i < _acceptor->children.size(); i++)
    {
        auto param = _acceptor->children[i];
        param->accept(this);
        _acceptor->children[i] = shortenedExpr;
    }
    this->is_constant = false;
}

void ShortenExpressionVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    for (unsigned int i = 0; i < _acceptor->params.size(); i++)
    {
        auto param = _acceptor->params[i];
        param->accept(this);
        _acceptor->params[i] = shortenedExpr;
    }
    this->is_constant = false;
}
