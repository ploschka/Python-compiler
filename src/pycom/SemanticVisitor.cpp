#include <pycom/semanalyzer/SemanticVisitor.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>

#include <stdexcept>
#include <iostream>
#include <map>

void SemanticVisitor::visitFormalParamsNode(FormalParamsNode *) {}
void SemanticVisitor::visitActualParamsNode(ActualParamsNode *) {}
void SemanticVisitor::visitTypeNode(TypeNode *) {}

SemanticVisitor::SemanticVisitor(block_map_t *_map) : blockmap(_map) {}

void SemanticVisitor::visitLeaf(Leaf *_acceptor)
{
    auto &token = _acceptor->token;
    if (token.getType() == Type::id)
    {
        auto symbol = symtable.top()->find(token.getValue());
        if (symbol == symtable.top()->end())
        {
            throw std::runtime_error(
                "Name " + token.getValue() + " is undefined\n" +
                "Occured at row: " +
                std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        }

        lastpos = token.getPos();
        lastrow = token.getRow();
        evaluated_type = symbol->second.type;
    }
    else
    {
        switch (token.getType())
        {
        case Type::string:
            evaluated_type = set.insert("String").first;
            break;
        case Type::number:
            if (token.getValue().find('.') == token.getValue().npos)
                evaluated_type = set.insert("Integer").first;
            else
                evaluated_type = set.insert("Float").first;
            break;

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
        throw std::runtime_error(
            "Name " + token.getValue() + " is undefined\n" +
            "Occured at row: " +
            std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
    }
    else
    {
        auto symtype = set.find(token.getValue());
        if (symbol->second.type != symtype)
        {
            throw std::runtime_error(
                "Not a callable at row: " +
                std::to_string(token.getRow()) +
                " position: " +
                std::to_string(token.getPos()) + "\n");
        }
    }

    auto func = funcs.find(token.getValue());
    if (func->second.second.size() != _acceptor->params->params.size())
    {
        throw std::runtime_error(
            "Parameter quantity mismatch occured at row: " +
            std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
    }

    auto arg_iter = func->second.second.begin();

    for (auto &par : _acceptor->params->params)
    {
        par->accept(this);
        if (evaluated_type != *arg_iter++)
        {
            throw std::runtime_error(
                "Parameter type mismatch occured at row: " +
                std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        }
    }
    evaluated_type = func->second.first;
    lastpos = token.getPos();
    lastrow = token.getRow();
}

void SemanticVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    _acceptor->left->accept(this);
    type_t a = evaluated_type;
    _acceptor->right->accept(this);
    if (a != evaluated_type)
    {
        throw std::runtime_error(
            "Type mismatch occured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
    }
    switch (_acceptor->op->token.getType())
    {
    case Type::less:
    case Type::greater:
    case Type::noteq:
    case Type::equal:
        evaluated_type = set.insert("Bool").first;
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
    if (_acceptor->type)
    {
        auto& typtok = _acceptor->type->token;
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
            throw std::runtime_error(
                "Name " + token.getValue() + " is undefined\n" +
                "Occured at row: " +
                std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
        }
    }
    _acceptor->right->accept(this);
    if (evaluated_type != symtype)
    {
        throw std::runtime_error(
            "Type mismatch occured at row: " +
            std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
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
        blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
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
                throw std::runtime_error(
                    "Name " + token.getValue() + " is already defined\n" +
                    "Defined second time at row : " +
                    std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
            }

            ++n;
            ++t;
        }

        _acceptor->body->accept(this);
        blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
        symtable.pop();
    }
    else
    {
        throw std::runtime_error(
            "Name " + token.getValue() + " is already defined\n" +
            "Defined second time at row : " +
            std::to_string(token.getRow()) + " position: " + std::to_string(token.getPos()) + "\n");
    }
}

void SemanticVisitor::visitElseNode(ElseNode *_acceptor)
{
    blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
    _acceptor->body->accept(this);
}

void SemanticVisitor::visitElifNode(ElifNode *_acceptor)
{
    _acceptor->condition->accept(this);
    if (evaluated_type != set.find("Bool"))
    {
        throw std::runtime_error(
            "Condition type is not Bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
    }

    blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
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
    if (evaluated_type != set.find("Bool"))
    {
        throw std::runtime_error(
            "Condition type is not Bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
    }

    blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
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

    if (evaluated_type != set.find("Bool"))
    {
        throw std::runtime_error(
            "Condition type is not Bool.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
    }

    blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
    _acceptor->body->accept(this);
}

void SemanticVisitor::visitForNode(ForNode *_acceptor)
{
    auto &iter = _acceptor->iterator->token;

    _acceptor->condition->accept(this);
    auto pos = evaluated_type->find("list");
    if (pos == evaluated_type->npos)
    {
        throw std::runtime_error(
            "Expression type is not list type.\nOccured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
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

    blockmap->insert({_acceptor->body, std::make_tuple(symtable.top())});
    _acceptor->body->accept(this);
}

void SemanticVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    for (auto child : _acceptor->children)
    {
        child->accept(this);
    }
}

void SemanticVisitor::visitListNode(ListNode *_acceptor)
{
    auto prev_evaluated_type = set.end();
    for (auto &i : _acceptor->children)
    {
        i->accept(this);
        if (prev_evaluated_type == set.end())
        {
            prev_evaluated_type = evaluated_type;
        }
        if (prev_evaluated_type != evaluated_type)
        {
            throw std::runtime_error(
            "List element type mismatch occured at row: " +
            std::to_string(lastrow) + " position: " + std::to_string(lastpos) + "\n");
        }
    }
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
    set.clear();
    funcs.clear();
    blockmap->clear();
    while (!symtable.empty())
    {
        symtable.pop();
    }
    this->stdinit();
}
