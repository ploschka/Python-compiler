#include <pycom/optimizer/CalculateVisitor.hpp>
#include <pycom/utility/Types.hpp>

void CalculateVisitor::visitTypeNode(TypeNode *) {}
void CalculateVisitor::visitFormalParamsNode(FormalParamsNode *) {}
void CalculateVisitor::visitActualParamsNode(ActualParamsNode *) {}
void CalculateVisitor::visitCallNode(CallNode *) {}
void CalculateVisitor::visitAssignmentNode(AssignmentNode *) {}
void CalculateVisitor::visitReturnNode(ReturnNode *) {}
void CalculateVisitor::visitBlockNode(BlockNode *) {}
void CalculateVisitor::visitProgramNode(ProgramNode *) {}
void CalculateVisitor::visitFunctionNode(FunctionNode *) {}
void CalculateVisitor::visitElseNode(ElseNode *) {}
void CalculateVisitor::visitElifNode(ElifNode *) {}
void CalculateVisitor::visitIfNode(IfNode *) {}
void CalculateVisitor::visitWhileNode(WhileNode *) {}
void CalculateVisitor::visitForNode(ForNode *) {}
void CalculateVisitor::visitListNode(ListNode *) {}

bool CalculateVisitor::has_type(ExpressionNode *node, std::string type)
{
    return *node->type == type;
}

std::string CalculateVisitor::calculate_binary_string(std::string left, std::string right, Type op)
{
    if (op == Type::plus)
    {
        return left + right;
    }
    else
    {
        return "ķ̵̀ȋ̸͓̣̖̘̩̕l̵͇͚̠͕̂̽̊̚ͅl̵̛̖̦̭̳͛̏ͅ ̴͉̀͌y̷̢̞̔͠ǫ̵̟̖̀̏̒͆͠ṳ̸͇̇̾̄̂̈́r̴̖̱̮͎͋̌̃͗͘s̷̮̖̈́͐̌è̸̻͙͈̀̌l̶̙̩̎̔̎̍f̸̲̹̭͔̒̉̉͝";
    }
}

int CalculateVisitor::calculate_binary_int(int left, int right, Type op)
{
    int res = 0;
    switch (op)
    {
    case Type::orop:
        res = left || right;
        break;
    case Type::andop:
        res = left && right;
        break;
    case Type::plus:
        res = left + right;
        break;
    case Type::minus:
        res = left - right;
        break;
    case Type::star:
        res = left * right;
        break;
    case Type::div:
        res = left / right;
        break;
    case Type::greater:
        res = left > right;
        break;
    case Type::less:
        res = left < right;
        break;
    case Type::equal:
        res = left == right;
        break;
    case Type::noteq:
        res = left != right;
        break;
    case Type::grequal:
        res = left >= right;
        break;
    case Type::lequal:
        res = left <= right;
        break;
    case Type::mod:
        res = left % right;
        break;
    default:
        break;
    }
    return res;
}

int CalculateVisitor::calculate_unary_int(int value, Type op)
{
    int res = 0;
    switch (op)
    {
    case Type::notop:
        res = !value;
        break;
    case Type::minus:
        res = -value;
        break;
    case Type::plus:
        res = value;
        break;
    default:
        break;
    }
    return res;
}

void CalculateVisitor::visitLeaf(Leaf *_acceptor)
{
    this->return_result = _acceptor->token.getValue();
}

void CalculateVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    _acceptor->left->accept(this);
    std::string left_res = this->return_result;
    _acceptor->right->accept(this);
    std::string right_res = this->return_result;

    auto op = _acceptor->op->token.getType();
    if (has_type(_acceptor->left, INTEGER_TYPE))
    {
        int l = std::stoi(left_res);
        int r = std::stoi(right_res);
        int res = calculate_binary_int(l, r, op);
        this->return_result = std::to_string(res);
        this->return_type = Type::number;
    }
    else
    {
        std::string l = left_res;
        std::string r = right_res;
        std::string res = calculate_binary_string(l, r, op);
        this->return_result = res;
        this->return_type = Type::string;
    }
}

void CalculateVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    _acceptor->operand->accept(this);
    std::string value = this->return_result;

    int v = std::stoi(value);
    auto op = _acceptor->op->token.getType();
    int res = calculate_unary_int(v, op);
    this->return_result = std::to_string(res);
    this->return_type = Type::number;
}
