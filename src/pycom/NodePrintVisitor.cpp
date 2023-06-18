#include <pycom/AST/NodePrintVisitor.h>
#include <iostream>

void PrintVisitor::print_line(std::string text) {
    // Выводит text в консоль с учётом текущего отступа
    for (int i = 0; i < this->indent; i++) {
        std::cout << this->indent_str;
    }
    std::cout << text;
    std::cout << "\n";
}

void PrintVisitor::visitLeaf(Leaf* acceptor) {
    std::string text = "<" + type_to_str(acceptor->token.getType()) + ", " + acceptor->token.getValue() + ">";
    this->print_line(text);
}

void PrintVisitor::visitFormalParamsNode(FormalParamsNode* acceptor) {
    std::string text = "FormalParams";
    this->print_line(text);
    this->indent++;
    for (auto param: acceptor->params) {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitActualParamsNode(ActualParamsNode* acceptor) {
    std::string text = "ActualParams";
    this->print_line(text);
    this->indent++;
    for (auto param: acceptor->params) {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitVariableNode(VariableNode* acceptor) {
    std::string text = "Variable";
    this->print_line(text);
    this->indent++;
    for (auto param: acceptor->chain) {
        param->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitCallNode(CallNode* acceptor) {
    std::string text = "Call";
    this->print_line(text);
    this->indent++;
    this->print_line("callable:");
    this->indent++;
    acceptor->callable->accept(this);
    this->indent--;
    this->print_line("params:");
    this->indent++;
    acceptor->params->accept(this);
    this->indent--;
    this->indent--;
}

void PrintVisitor::visitBinaryNode(BinaryNode* acceptor) {
    std::string text = "BinaryOp (" + type_to_str(acceptor->op->token.getType()) + ")";
    this->print_line(text);
    this->indent++;
    acceptor->left->accept(this);
    acceptor->right->accept(this);
    this->indent--;
}

void PrintVisitor::visitUnaryNode(UnaryNode* acceptor) {
    std::string text = "UnaryOp (" + acceptor->op->token.getValue() + ")";
    this->print_line(text);
    this->indent++;
    acceptor->operand->accept(this);
    this->indent--;
}

void PrintVisitor::visitAssignmentNode(AssignmentNode* acceptor) {
    std::string text = "Assignment";
    this->print_line(text);
    this->indent++;
    acceptor->left->accept(this);
    acceptor->right->accept(this);
    this->indent--;
}

void PrintVisitor::visitReturnNode(ReturnNode* acceptor) {
    std::string text = "Return";
    this->print_line(text);
    this->indent++;
    acceptor->return_value->accept(this);
    this->indent--;
}

void PrintVisitor::visitBlockNode(BlockNode* acceptor) {
    std::string text = "Block";
    this->print_line(text);
    this->indent++;
    for (auto child: acceptor->children) {
        child->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitProgramNode(ProgramNode* acceptor) {
    std::string text = "Program";
    this->print_line(text);
    this->indent++;
    for (auto child: acceptor->children) {
        child->accept(this);
    }
    this->indent--;
}

void PrintVisitor::visitFunctionNode(FunctionNode* acceptor) {
    std::string text = "Function def";
    this->print_line(text);
    this->indent++;
    this->print_line("name:");
    this->indent++;
    acceptor->id->accept(this);
    this->indent--;
    acceptor->formal_params->accept(this);
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitElseNode(ElseNode* acceptor) {
    std::string text = "Else";
    this->print_line(text);
    this->indent++;
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitElifNode(ElifNode* acceptor) {
    std::string text = "Elif";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitIfNode(IfNode* acceptor) {
    std::string text = "If";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitWhileNode(WhileNode* acceptor) {
    std::string text = "While";
    this->print_line(text);
    this->indent++;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    this->indent--;
}

void PrintVisitor::visitForNode(ForNode* acceptor) {
    std::string text = "For";
    this->print_line(text);
    this->indent++;
    this->print_line("iterator:");
    acceptor->iterator->accept(this);
    this->indent++;
    this->indent--;
    this->print_line("condition:");
    this->indent++;
    acceptor->condition->accept(this);
    this->indent--;
    acceptor->body->accept(this);
    this->indent--;
}
