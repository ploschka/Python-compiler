#include <pycom/codegen/CodeEmittingNodeVisitor.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <map>

CodeEmittingNodeVisitor::CodeEmittingNodeVisitor(std::ostream& _stream): stream(_stream){}

void CodeEmittingNodeVisitor::write(std::string s) {
    std::map<std::string, std::string> reserved = {
        {"int", "int"},
        {"str", "std::string"},
        {"None", "void"} 
    };
    if (reserved.find(s) != reserved.end()) {
        this->stream << reserved.at(s);
    } else {
        this->stream << s;
    }
}

void CodeEmittingNodeVisitor::write(Token token) {
    if (token.getType() == Type::string) {
        write("\"");
        write(token.getValue());
        write("\"");
    } else {
        write(token.getValue());
    }
}

void CodeEmittingNodeVisitor::write(Leaf* leaf) {
    write(leaf->token);
}

void CodeEmittingNodeVisitor::visitLeaf(Leaf *_acceptor)
{
    write(_acceptor);
}

void CodeEmittingNodeVisitor::visitTypeNode(TypeNode *_acceptor)
{
    write(_acceptor->token);
}

void CodeEmittingNodeVisitor::visitFormalParamsNode(FormalParamsNode *_acceptor)
{
    int count = _acceptor->params.size();
    for (int i = 0; i < count; i++) {
        _acceptor->types[i]->accept(this);
        if (_acceptor->types[i]->is_list) {
            write("[]");
        }
        write(" ");
        _acceptor->params[i]->accept(this);
        if (i != (count - 1))
            write(", ");
    }
}
void CodeEmittingNodeVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    int count = _acceptor->params.size();
    for (int i = 0; i < count; i++) {
        _acceptor->params[i]->accept(this);
        if (i != (count - 1))
            write(", ");
    }
}
void CodeEmittingNodeVisitor::visitCallNode(CallNode *_acceptor)
{
    write(_acceptor->callable);
    write("(");
    _acceptor->params->accept(this);
    write(")");
}
void CodeEmittingNodeVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    std::map<Type, std::string> bin_op_strs = {
        {Type::orop, "||"},
        {Type::andop, "&&"},
        {Type::plus, "+"},
        {Type::minus, "-"},
        {Type::star, "*"},
        {Type::div, "/"},
        {Type::greater, ">"},
        {Type::less, "<"},
        {Type::equal, "=="},
        {Type::noteq, "!="},
        {Type::grequal, ">="},
        {Type::lequal, "<="},
        {Type::mod, "%"},
    };
    Type op = _acceptor->op->token.getType();
    write("(");
    _acceptor->left->accept(this);
    write(" ");
    write(bin_op_strs.at(op));
    write(" ");
    _acceptor->right->accept(this);
    write(")");
}
void CodeEmittingNodeVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
    std::map<Type, std::string> bin_op_strs = {
        {Type::notop, "!"},
        {Type::plus, "+"},
        {Type::minus, "-"}
    };
    Type op = _acceptor->op->token.getType();
    write(bin_op_strs.at(op));
    write("(");
    _acceptor->operand->accept(this);
    write(")");
}
void CodeEmittingNodeVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    if (_acceptor->type != nullptr) {
        _acceptor->type->accept(this);
        write(" ");
    }
    write(_acceptor->left);
    if (_acceptor->type != nullptr && _acceptor->type->is_list) {
        write("[]");
    }
    write(" = ");
    _acceptor->right->accept(this);
}
void CodeEmittingNodeVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    write("return ");
    _acceptor->return_value->accept(this);
}
void CodeEmittingNodeVisitor::visitBlockNode(BlockNode *_acceptor)
{
    write("{\n");
    for (auto child: _acceptor->children) {
        child->accept(this);
        write(";\n");
    }
    write("}");
}
void CodeEmittingNodeVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    for (auto child: _acceptor->children) {
        child->accept(this);
        write(";\n");
    }
}
void CodeEmittingNodeVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
    _acceptor->return_type->accept(this);
    write(" ");
    write(_acceptor->id);
    write("(");
    _acceptor->formal_params->accept(this);
    write(")\n");
    _acceptor->body->accept(this);
}
void CodeEmittingNodeVisitor::visitElseNode(ElseNode *_acceptor)
{
    write("else");
    _acceptor->body->accept(this);
}
void CodeEmittingNodeVisitor::visitElifNode(ElifNode *_acceptor)
{
    write("else if ");
    write("(");
    _acceptor->condition->accept(this);
    write(")\n");
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr) {
        _acceptor->next_elif->accept(this);
    }
    if (_acceptor->next_else != nullptr) {
        _acceptor->next_else->accept(this);
    }
}
void CodeEmittingNodeVisitor::visitIfNode(IfNode *_acceptor)
{
    write("if");
    write(" ");
    write("(");
    _acceptor->condition->accept(this);
    write(")\n");
    _acceptor->body->accept(this);
    if (_acceptor->next_elif != nullptr) {
        _acceptor->next_elif->accept(this);
    }
    if (_acceptor->next_else != nullptr) {
        _acceptor->next_else->accept(this);
    }
}
void CodeEmittingNodeVisitor::visitWhileNode(WhileNode *_acceptor)
{
    write("while");
    write(" ");
    write("(");
    _acceptor->condition->accept(this);
    write(")\n");
    _acceptor->body->accept(this);  
}
void CodeEmittingNodeVisitor::visitForNode(ForNode *_acceptor)
{
    write("for (auto ");
    write(_acceptor->iterator);
    write(" : ");
    _acceptor->condition->accept(this);
    write(")\n");
    _acceptor->body->accept(this);
}
void CodeEmittingNodeVisitor::visitListNode(ListNode *_acceptor)
{
    write("{");
    int count = _acceptor->children.size();
    for (int i = 0; i < count; i++) {
        _acceptor->children[i]->accept(this);
        if (i != (count - 1))
            write(", ");
    }
    write("}");
}