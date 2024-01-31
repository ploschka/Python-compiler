#include <pycom/semanalyzer/SemanticVisitor.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>

#include <stdexcept>
#include <iostream>
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
            em->error("Name " + token.getValue() + " is undefined\n" +
                      "Occured at row: " +
                      std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
            err = true;
            return;
        }
        evaluated_type = symbol->second.type;
    }
    else
    {
        switch (token.getType())
        {
        case Type::string:
            evaluated_type = set.insert("string").first;
            break;
        case Type::number:
            evaluated_type = set.insert("int").first;

        default:
            evaluated_type = set.insert("void").first;
            break;
        }
    }
}

void SemanticVisitor::visitCallNode(CallNode *_acceptor)
{
    auto &token = _acceptor->callable;
    auto symbol = symtable.top()->find(token.getValue());
    if (symbol == symtable.top()->end())
    {
        em->error("Name " + token.getValue() + " is undefined\n" +
                  "Occured at row: " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        err = true;
        return;
    }
    else
    {
        auto symtype = set.find(token.getValue());
        if (symbol->second.type != symtype)
        {
            em->error("Not a callable at row: " +
                      std::to_string(token.getRow()) +
                      " position: " +
                      std::to_string(token.getPos()) + "\n");
            err = true;
            return;
        }
    }

    auto func = funcs.find(token.getValue());
    if (func->second.second.size() != _acceptor->params->params.size())
    {
        em->error("Parameter quantity mismatch occured at row: " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        err = true;
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
            em->error(
                "Parameter type mismatch occured at row: " +
                std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
            err = true;
            return;
        }
    }
    evaluated_type = func->second.first;
    lastpos = token.getPos();
    lastrow = token.getRow();
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
        em->error("Type mismatch occured at row: " +
                  std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        err = true;
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
        break;
    }
}

void SemanticVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    _acceptor->operand->accept(this);
}

void SemanticVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    auto token = _acceptor->left->token;
    type_t symtype;
    if (!_acceptor->type)
    {
        auto &typtok = _acceptor->type->token;
        if (_acceptor->type->is_list)
        {
            symtype = set.insert("list " + typtok.getValue()).first;
            set.insert(typtok.getValue());
        }
        else
        {
            symtype = set.insert(typtok.getValue()).first;
        }
    }
    else
    {
        auto symbol = symtable.top()->find(token.getValue());
        if (symbol == symtable.top()->end())
        {
            em->error("Name " + token.getValue() + " is undefined\n" +
                      "Occured at row: " +
                      std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
            err = true;
            return;
        }
    }
    _acceptor->right->accept(this);
    if (err)
        return;
    if (evaluated_type != symtype)
    {
        em->error("Type mismatch occured at row: " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        err = true;
        return;
    }
    evaluated_type = symtype;
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
        symtable.push(std::make_unique<localtable_t>(*symtable.top()));
        auto curr = funcs.insert({token.getValue(), {}}).first;
        curr->second.first = set.insert(_acceptor->return_type->token.getValue()).first;

        auto n = _acceptor->formal_params->params.begin();
        auto t = _acceptor->formal_params->types.begin();
        while (n != _acceptor->formal_params->params.end() && t != _acceptor->formal_params->types.end())
        {
            auto token = (*n)->token;
            auto type = (*t)->token;

            symtype = set.insert(type.getValue());
            curr->second.second.push_back(symtype.first);

            auto symbol = symtable.top()->find(token.getValue());
            if (symbol == symtable.top()->end())
            {
                symtable.top()->insert({token.getValue(), {token, symtype.first}});
            }
            else
            {
                em->error("Name " + token.getValue() + " is already defined\n" +
                          "Defined second time at row : " +
                          std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
                err = true;
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
        em->error("Name " + token.getValue() + " is already defined\n" +
                  "Defined second time at row : " +
                  std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        err = true;
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
    if (evaluated_type != set.find("Bool"))
    {
        em->error(
            "Condition type is not Bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        err = true;
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
    if (evaluated_type != set.find("Bool"))
    {
        em->error(
            "Condition type is not Bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        err = true;
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

    if (evaluated_type != set.find("Bool"))
    {
        em->error(
            "Condition type is not Bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        err = true;
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
    auto pos = evaluated_type->find("list");
    if (pos == evaluated_type->npos)
    {
        em->error(
            "Expression type is not list type.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        err = true;
        return;
    }

    pos += 5;
    auto list_el_type = evaluated_type->c_str() + pos;

    auto iter_iter = symtable.top()->find(iter.getValue());
    if (iter_iter == symtable.top()->end())
    {
        symtable.top()->insert({iter.getValue(), {iter, set.find(list_el_type)}});
    }
    else
    {
        iter_iter->second.type = set.find(list_el_type);
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
            em->error(
                "List element type mismatch occured at row: " +
                std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
            err = true;
            return;
        }
    }
    evaluated_type = set.insert("list " + *evaluated_type).first;
}

void SemanticVisitor::stdinit()
{
    symtable.push(std::make_unique<localtable_t>());

    Symbol tr = {{"True", Type::id}, set.insert("bool").first};
    Symbol fl = {{"False", Type::id}, set.insert("bool").first};
    Symbol wr = {{"Put_Line", Type::id}, set.insert("Put_Line").first};

    funcs.insert({"Put_Line", {set.insert("void").first, {set.insert("String").first}}});

    symtable.top()->insert({tr.token.getValue(), tr});
    symtable.top()->insert({fl.token.getValue(), fl});
    symtable.top()->insert({wr.token.getValue(), wr});
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

void SemanticVisitor::setErr(ErrorManagerInterface *_em)
{
    this->em = _em;
}
