#include <pycom/semanalyzer/SemanticVisitor.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>

#include <map>

void SemanticVisitor::visitFormalParamsNode(FormalParamsNode *) {}
void SemanticVisitor::visitActualParamsNode(ActualParamsNode *) {}
void SemanticVisitor::visitTypeNode(TypeNode *) {}

void SemanticVisitor::visitLeaf(Leaf *_acceptor)
{
    auto &token = _acceptor->token;
    lastpos = token.getPos();
    lastrow = token.getRow();

    if (token.getType() == Type::id)
    {
        auto symbol = symtable.top()->find(token.getValue());
        if (symbol == symtable.top()->end())
        {
            error("Name " + token.getValue() + " is undefined\n" +
                  "Occured at row: " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
            return;
        }
        evaluated_type = symbol->second.second;
    }
    else
    {
        switch (token.getType())
        {
        case Type::string:
            evaluated_type = set.insert(STRING_TYPE).first;
            break;
        case Type::number:
            evaluated_type = set.insert(INTEGER_TYPE).first;
            break;
        default:
            evaluated_type = set.insert(VOID_TYPE).first;
            break;
        }
    }
    _acceptor->type = evaluated_type;
}

void SemanticVisitor::visitCallNode(CallNode *_acceptor)
{
    auto &token = _acceptor->callable;
    auto symbol = symtable.top()->find(token.getValue());
    if (symbol == symtable.top()->end())
    {
        error("Name " + token.getValue() + " is undefined\n" +
              "Occured at row: " +
              std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        return;
    }
    else
    {
        auto symtype = set.find(token.getValue());
        if (symbol->second.second != symtype)
        {
            error("Not a callable at row: " +
                  std::to_string(token.getRow()) +
                  " position: " +
                  std::to_string(token.getPos()) + "\n");
            return;
        }
    }

    auto func = funcs.find(token.getValue());
    if (func->second.second.size() != _acceptor->params->params.size())
    {
        error("Parameter quantity mismatch occured at row: " +
              std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        return;
    }

    auto arg_iter = func->second.second.begin();

    for (auto &par : _acceptor->params->params)
    {
        par->accept(this);
        if (err)
            return;
        if (evaluated_type != *arg_iter++)
        {
            error(
                "Parameter type mismatch occured at row: " +
                std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
            return;
        }
    }
    evaluated_type = func->second.first;
    lastpos = token.getPos();
    lastrow = token.getRow();
    _acceptor->type = evaluated_type;
}

void SemanticVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    _acceptor->left->accept(this);
    if (err)
        return;
    type_t a = evaluated_type;
    _acceptor->right->accept(this);
    if (err)
        return;
    if (a != evaluated_type)
    {
        error("Type mismatch occured at row: " +
              std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        return;
    }
    switch (_acceptor->op->token.getType())
    {
    case Type::less:
    case Type::greater:
    case Type::noteq:
    case Type::equal:
        evaluated_type = set.insert("bool").first;
        break;
    default:
        evaluated_type = a;
        break;
    }
    _acceptor->type = evaluated_type;
}

void SemanticVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    _acceptor->operand->accept(this);
    _acceptor->type = evaluated_type;
}

void SemanticVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    auto token = _acceptor->left->token;
    type_t lefttype;
    _acceptor->right->accept(this);
    if (err)
        return;
    auto righttype = evaluated_type;

    if (_acceptor->type)
    {
        auto &typtok = _acceptor->type->token;
        if (_acceptor->type->is_list)
        {
            lefttype = set.insert(LIST_TYPE + typtok.getValue()).first;
            set.insert(typtok.getValue());
        }
        else
        {
            lefttype = set.insert(typtok.getValue()).first;
        }

        if (righttype != lefttype)
        {
            error("Type mismatch occured at row: " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
            return;
        }
    }
    else
    {
        auto symbol = symtable.top()->find(token.getValue());
        if (symbol == symtable.top()->end())
        {
            error("Name " + token.getValue() + " is undefined\n" +
                  "Occured at row: " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
            return;
        }
        lefttype = symbol->second.second;
    }
    symtable.top()->insert({token.getValue(), {token, lefttype}});
    if (righttype != lefttype)
    {
        error("Type mismatch occured at row: " +
              std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        return;
    }
    evaluated_type = lefttype;
}

void SemanticVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    _acceptor->return_value->accept(this);
}

void SemanticVisitor::visitBlockNode(BlockNode *_acceptor)
{
    for (auto child : _acceptor->children)
    {
        child->accept(this);
        if (err)
            return;
    }
}

void SemanticVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
    auto token = _acceptor->id->token;
    auto symbol = symtable.top()->find(token.getValue());
    auto symtype = set.insert(token.getValue());
    if (symbol == symtable.top()->end() && symtype.second)
    {
        symtable.top()->insert({token.getValue(), {token, symtype.first}});

        symtable.push(std::make_shared<localtable_t>(*symtable.top()));
        auto curr = funcs.insert({token.getValue(), {}}).first;
        curr->second.first = set.insert(_acceptor->return_type->token.getValue()).first;

        auto n = _acceptor->formal_params->params.begin();
        auto t = _acceptor->formal_params->types.begin();
        while (n != _acceptor->formal_params->params.end() && t != _acceptor->formal_params->types.end())
        {
            auto tt = (*n)->token;
            auto type = (*t)->token;

            type_t symtype;

            if ((*t)->is_list)
            {
                symtype = set.insert(LIST_TYPE + type.getValue()).first;
                set.insert(type.getValue());
            }
            else
            {
                symtype = set.insert(type.getValue()).first;
            }

            curr->second.second.push_back(symtype);

            auto symbol = symtable.top()->find(tt.getValue());
            if (symbol == symtable.top()->end())
            {
                symtable.top()->insert({tt.getValue(), {tt, symtype}});
            }
            else
            {
                error("Name " + tt.getValue() + " is already defined\n" +
                      "Defined second time at row : " +
                      std::to_string(tt.getRow()) + " position: " + std::to_string(tt.getPos()) + "\n");
                return;
            }

            ++n;
            ++t;
        }

        _acceptor->body->accept(this);
        symtable.pop();
    }
    else
    {
        error("Name " + token.getValue() + " is already defined\n" +
              "Defined second time at row : " +
              std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        return;
    }
}

void SemanticVisitor::visitElseNode(ElseNode *_acceptor)
{
    _acceptor->body->accept(this);
}

void SemanticVisitor::visitElifNode(ElifNode *_acceptor)
{
    _acceptor->condition->accept(this);
    if (err)
        return;
    if (evaluated_type != set.find(BOOL_TYPE))
    {
        error(
            "Condition type is not bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        return;
    }

    _acceptor->body->accept(this);

    if (_acceptor->next_elif)
    {
        _acceptor->next_elif->accept(this);
    }
    if (_acceptor->next_else)
    {
        _acceptor->next_else->accept(this);
    }
}

void SemanticVisitor::visitIfNode(IfNode *_acceptor)
{
    _acceptor->condition->accept(this);
    if (err)
        return;
    if (evaluated_type != set.find(BOOL_TYPE))
    {
        error(
            "Condition type is not bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        return;
    }

    _acceptor->body->accept(this);

    if (_acceptor->next_elif)
    {
        _acceptor->next_elif->accept(this);
    }
    if (_acceptor->next_else)
    {
        _acceptor->next_else->accept(this);
    }
}

void SemanticVisitor::visitWhileNode(WhileNode *_acceptor)
{
    _acceptor->condition->accept(this);
    if (err)
        return;

    if (evaluated_type != set.find(BOOL_TYPE))
    {
        error(
            "Condition type is not bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        return;
    }

    _acceptor->body->accept(this);
}

void SemanticVisitor::visitForNode(ForNode *_acceptor)
{
    auto &iter = _acceptor->iterator->token;

    _acceptor->condition->accept(this);
    if (err)
        return;
    auto pos = evaluated_type->find(LIST_TYPE);
    if (pos == evaluated_type->npos)
    {
        error(
            "Expression type is not list type.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        return;
    }

    pos += 4;
    auto list_el_type = evaluated_type->c_str() + pos;

    auto iter_iter = symtable.top()->find(iter.getValue());
    if (iter_iter == symtable.top()->end())
    {
        symtable.top()->insert({iter.getValue(), {iter, set.find(list_el_type)}});
    }
    else
    {
        iter_iter->second.second = set.find(list_el_type);
    }

    _acceptor->body->accept(this);
}

void SemanticVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    for (auto child : _acceptor->children)
    {
        child->accept(this);
        if (err)
            return;
    }
}

void SemanticVisitor::visitListNode(ListNode *_acceptor)
{
    auto prev_evaluated_type = set.end();
    for (auto &i : _acceptor->children)
    {
        i->accept(this);
        if (err)
            return;
        if (prev_evaluated_type == set.end())
        {
            prev_evaluated_type = evaluated_type;
        }
        if (prev_evaluated_type != evaluated_type)
        {
            error(
                "List element type mismatch occured at row: " +
                std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
            return;
        }
    }
    evaluated_type = set.insert(LIST_TYPE + *evaluated_type).first;
    _acceptor->type = evaluated_type;
}

void SemanticVisitor::stdinit()
{
    symtable.push(std::make_shared<localtable_t>());

    symbol_t _1 = {{"True", Type::id}, set.insert("bool").first};
    symbol_t _2 = {{"False", Type::id}, set.insert("bool").first};
    symbol_t _3 = {{"puts", Type::id}, set.insert("puts").first};
    symbol_t _4 = {{"putf", Type::id}, set.insert("putf").first};

    funcs.insert({"puts", {set.insert(VOID_TYPE).first, {set.insert(STRING_TYPE).first}}});
    funcs.insert({"putf", {set.insert(VOID_TYPE).first, {set.insert(INTEGER_TYPE).first}}});

    symtable.top()->insert({_1.first.getValue(), _1});
    symtable.top()->insert({_2.first.getValue(), _2});
    symtable.top()->insert({_3.first.getValue(), _3});
    symtable.top()->insert({_4.first.getValue(), _4});
}

void SemanticVisitor::reset()
{
    err = false;
    set.clear();
    funcs.clear();
    while (!symtable.empty())
    {
        symtable.pop();
    }
    this->stdinit();
}

void SemanticVisitor::setEM(ErrorManagerInterface *_em)
{
    em = _em;
}

void SemanticVisitor::error(const std::string &_str)
{
    if (em)
    {
        em->error(_str);
    }
    err = true;
}
