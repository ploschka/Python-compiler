#include <pycom/parser/Parser.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <stdexcept>
#include <map>
#include <set>

std::map<std::string, std::set<Type>> FIRSTS = {{"for_stmt",      {Type::forkw}},
                                                {"primary",       {Type::lpr,    Type::number,  Type::id,     Type::string,  Type::dot}},
                                                {"return_stmt",   {Type::returnkw}},
                                                {"arguments",     {Type::plus,   Type::inv,     Type::number, Type::minus,   Type::string, Type::dot,      Type::id,       Type::lpr}},
                                                {"func_call",     {Type::lpr}},
                                                {"factor",        {Type::plus,   Type::inv,     Type::number, Type::minus,   Type::string, Type::dot,      Type::id,       Type::lpr}},
                                                {"block",         {Type::plus,   Type::inv,     Type::number, Type::breakkw, Type::minus,  Type::passkw,   Type::string,   Type::dot,    Type::id,      Type::continuekw, Type::lpr,    Type::newline, Type::returnkw}},
                                                {"atom",          {Type::number, Type::string,  Type::id}},
                                                {"if_stmt",       {Type::ifkw}},
                                                {"while_stmt",    {Type::whilekw}},
                                                {"elif_stmt",     {Type::elifkw}},
                                                {"statement",     {Type::plus,   Type::inv,     Type::minus,  Type::passkw,  Type::forkw,  Type::returnkw, Type::defkw,    Type::number, Type::breakkw, Type::string,     Type::dot,    Type::id,      Type::continuekw, Type::lpr,        Type::ifkw, Type::whilekw}},
                                                {"statements",    {Type::plus,   Type::inv,     Type::minus,  Type::passkw,  Type::forkw,  Type::returnkw, Type::defkw,    Type::number, Type::breakkw, Type::string,     Type::dot,    Type::id,      Type::continuekw, Type::lpr,        Type::ifkw, Type::whilekw}},
                                                {"file",          {Type::eof,    Type::plus,    Type::inv,    Type::minus,   Type::passkw, Type::forkw,    Type::returnkw, Type::defkw,  Type::number,  Type::breakkw,    Type::string, Type::dot,     Type::id,         Type::continuekw, Type::lpr,  Type::ifkw, Type::whilekw}},
                                                {"function_def",  {Type::defkw}},
                                                {"simple_stmt",   {Type::plus,   Type::inv,     Type::number, Type::breakkw, Type::minus,  Type::passkw,   Type::string,   Type::dot,    Type::id,      Type::continuekw, Type::lpr,    Type::returnkw}},
                                                {"params",        {Type::id}},
                                                {"expression",    {Type::plus,   Type::inv,     Type::number, Type::minus,   Type::string, Type::dot,      Type::id,       Type::lpr}},
                                                {"term",          {Type::plus,   Type::inv,     Type::number, Type::minus,   Type::string, Type::dot,      Type::id,       Type::lpr}},
                                                {"else_block",    {Type::elsekw}},
                                                {"compound_stmt", {Type::forkw,  Type::whilekw, Type::defkw,  Type::ifkw}},
                                                {"sum",           {Type::plus,   Type::inv,     Type::number, Type::minus,   Type::string, Type::dot,      Type::id,       Type::lpr}},
                                                {"simple_stmts",  {Type::plus,   Type::inv,     Type::number, Type::breakkw, Type::minus,  Type::passkw,   Type::string,   Type::dot,    Type::id,      Type::continuekw, Type::lpr,    Type::returnkw}},
                                                {"assignment",    {Type::id}}};

void Parser::setLexer(LexerInterface *lexer) {
    this->lexer = lexer;
}

AST *Parser::getAST() {
    // TODO: Реализовать
    return nullptr;
}


bool Parser::is_token_in_firsts(std::string grammar_node) {
    // Возвращает True, если текущий токен кода можно получить, спускаясь по узлу грамматика grammar_node
    return FIRSTS[grammar_node].find(this->token.getType()) != FIRSTS[grammar_node].end();
}

bool Parser::token_matches_any(std::vector<Type> types) {
    // Возвращает True, если текущий токен кода имеет тип, имеющийся в types
    for (Type type: types) {
        if (this->get_token().getType() == type) {
            return true;
        }
    }
    return false;
}

void Parser::error(std::string message) {
    throw std::runtime_error(message);
}

void Parser::next_token() {
    if (this->future_tokens.empty()) {
        this->token = this->lexer->getToken();
    } else {
        // Забираем токен из списка будущих токенов
        this->token = this->future_tokens[0];
        this->future_tokens.erase(this->future_tokens.begin());
    }
}

Token Parser::forward(int k) {
    while (this->future_tokens.size() < k) {
        this->future_tokens.push_back(this->lexer->getToken());
    }
    return this->future_tokens[k - 1];
}

Token Parser::get_token() {
    return this->token;
}

Token Parser::check_get_next(Type type) {
    // Проверяет, что текущий токен имеет тип type, возвращает его и сразу получает следующий токен
    if (this->token.getType() != type) {
        this->error("Неожиданный символ");
    }
    Token tmp = this->token;
    this->next_token();
    return tmp;
}

ProgramNode * Parser::file() {
    /*
    file:
        | statements EOF
        | EOF
     */
    ProgramNode* file = new ProgramNode();
    if (this->is_token_in_firsts("statements")) {
        this->statements(file);
    }
    file->add_child(new Leaf(this->check_get_next(Type::eof)));
    return file;
};

void Parser::statements(BlockNode *parent_block) {
    /*
    statements:
        | statement statements
        | statement
     */
    while (this->is_token_in_firsts("statements")) {
        this->statement(parent_block);
    }
};

void Parser::statement(BlockNode *parent_block) {
    /*
    statement:
        | compound_stmt
        | simple_stmts
     */
    if (this->is_token_in_firsts("compound_stmt")) {
        this->compound_stmt(parent_block);
    } else if (this->is_token_in_firsts("simple_stmts")) {
        this->simple_stmt(parent_block);
    } else {
        this->error("statement");
    }
};

void Parser::simple_stmts(BlockNode *parent_block) {
    /*
    simple_stmts:
        | simple_stmt NEWLINE simple_stmts
        | simple_stmt NEWLINE
     */
    while (this->is_token_in_firsts("simple_stmts")) {
        this->simple_stmt(parent_block);
        this->check_get_next(Type::newline);
    }
};

void Parser::simple_stmt(BlockNode *parent_block) {
    /*
    simple_stmt:
        | assignment
        | expression
        | return_stmt
        | PASS
        | BREAKKW
        | CONTINUEKW
     */
    if (this->is_token_in_firsts("assignment") && this->forward(1).getType() == Type::assign) {
        parent_block->add_child(this->assignment());
    } else if (this->is_token_in_firsts("expression")) {
        parent_block->add_child(this->expression());
    } else if (this->is_token_in_firsts("return_stmt")) {
        parent_block->add_child(this->return_stmt());
    } else if (this->token_matches_any({Type::passkw, Type::breakkw, Type::continuekw})) {
        parent_block->add_child(new Leaf(this->get_token()));
        this->next_token();
    } else {
        this->error("simple_stmt");
    }
};

AssignmentNode * Parser::assignment() {
    /*
    assignment:
        | ID ASSIGN expression
     */
    Leaf* left = new Leaf(this->check_get_next(Type::id));
    this->check_get_next(Type::assign);
    ExpressionNode* right = this->expression();
    return new AssignmentNode(left, right);
};

ReturnNode * Parser::return_stmt() {
    /*
    return_stmt:
        | RETURNKW
        | RETURNKW expression
     */
    this->check_get_next(Type::returnkw);
    if (this->is_token_in_firsts("expression")) {
        return new ReturnNode(this->expression());
    } else {
        Token None = Token("None", Type::id);
        return new ReturnNode(new Leaf(None));
    }

};

ExpressionNode * Parser::expression() {
    /*
    expression:
        | disjunction
     */
    return this->disjunction();
};

ExpressionNode * Parser::disjunction() {
    /*
    disjunction:
        | conjunction OR disjunction
        | conjunction
     */
    ExpressionNode* left = this->conjunction();
    if (this->get_token().getType() == Type::orop) {
        Token op = this->check_get_next(Type::orop);
        ExpressionNode* right = this->disjunction();
        return new BinaryNode(left, new Leaf(op), right);
    } else {
        return left;
    }
};

ExpressionNode * Parser::conjunction() {
    /*
    conjunction:
        | inversion AND conjunction
        | inversion
     */
    ExpressionNode* left = this->inversion();
    if (this->get_token().getType() == Type::andop) {
        Token op = this->check_get_next(Type::andop);
        ExpressionNode* right = this->conjunction();
        return new BinaryNode(left, new Leaf(op), right);
    } else {
        return left;
    }
};

ExpressionNode * Parser::inversion() {
    /*
    inversion:
        | NOT inversion
        | sum
     */
    if (this->get_token().getType() == Type::notop) {
        Token op = this->check_get_next(Type::notop);
        ExpressionNode* operand = this->inversion();
        return new UnaryNode(new Leaf(op), operand);
    } else {
        return this->sum();
    }
};

ExpressionNode * Parser::sum() {
    /*
    sum:
        | term PLUS sum
        | term MINUS sum
        | term
     */
    ExpressionNode* left = this->term();
    if (this->token_matches_any({Type::plus, Type::minus})) {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode * right = this->sum();
        return new BinaryNode(left, new Leaf(op), right);
    } else {
        return left;
    }
};

ExpressionNode * Parser::term() {
    /*
    term:
        | factor STAR term
        | factor DIV term
        | factor IDIV term
        | factor MOD term
        | factor
     */
    ExpressionNode* left = this->factor();
    if (this->token_matches_any({Type::star, Type::div, Type::idiv, Type::mod})) {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode * right = this->term();
        return new BinaryNode(left, new Leaf(op), right);
    } else {
        return left;
    }
};

ExpressionNode * Parser::factor() {
    /*
    factor:
        | PLUS factor
        | MINUS factor
        | INV factor
        | primary
     */
    if (this->token_matches_any({Type::plus, Type::minus, Type::inv})) {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode* operand = this->factor();
        return new UnaryNode(new Leaf(op), operand);
    } else {
        return this->primary();
    }
};

ExpressionNode * Parser::primary() {
    /*
    primary:
        | atom primary
        | atom
        | DOT ID primary
        | DOT ID
        | func_call
     */
    // Парсим цепочки типа ID.ID.ID.ID, пока не дойдём до вызова функции (например ID.func()), либо до конца цепочки
    VariableNode* variable = new VariableNode({});
    while (this->is_token_in_firsts("primary") && !this->is_token_in_firsts("func_call")) {
        if (this->is_token_in_firsts("atom")) {
            variable->add_to_chain(this->atom());
        } else {
            this->check_get_next(Type::dot);
            Token tk = this->check_get_next(Type::id);
            variable->add_to_chain(new Leaf(tk));
        }
    }
    if (this->is_token_in_firsts("func_call")) {
        ActualParamsNode* params = this->func_call();
        return new CallNode(variable, params);
    } else {
        return variable;
    }
};

ActualParamsNode * Parser::func_call() {
    /*
    func_call:
        | LPR arguments RPR
        | LPR RPR
     */
    this->check_get_next(Type::lpr);
    if (this->is_token_in_firsts("arguments")) {
        ActualParamsNode * params =  this->arguments();
        this->check_get_next(Type::rpr);
        return params;
    } else {
        this->check_get_next(Type::rpr);
        return new ActualParamsNode({});
    }
};

Leaf * Parser::atom() {
    /*
    atom:
        | ID
        | STRING
        | NUMBER
     */
    if (this->token_matches_any({Type::id, Type::string, Type::number})) {
        Leaf* leaf = new Leaf(this->get_token());
        this->next_token();
        return leaf;
    } else {
        this->error("atom");
    }
};

ActualParamsNode * Parser::arguments() {
    /*
    arguments:
        | expression COMMA arguments
        | expression
     */
    ActualParamsNode* params = new ActualParamsNode({});
    while (this->is_token_in_firsts("arguments")) {
        params->add_child(this->expression());
        if (this->get_token().getType() == Type::comma) {
            this->next_token();
        }
    }
    return params;
};

void Parser::compound_stmt(BlockNode *parent_block) {
    /*
    compound_stmt:
        | function_def
        | if_stmt
        | for_stmt
        | while_stmt
     */
    if (this->is_token_in_firsts("function_def")) {
        parent_block->add_child(this->function_def());
    } else if (this->is_token_in_firsts("if_stmt")) {
        parent_block->add_child(this->if_stmt());
    } else if (this->is_token_in_firsts("for_stmt")) {
        parent_block->add_child(this->for_stmt());
    } else if (this->is_token_in_firsts("while_stmt")) {
        parent_block->add_child(this->while_stmt());
    } else {
        this->error("compound_stmt");
    }
};

FormalParamsNode * Parser::params() {
    /*
    params:
        | ID COMMA params
        | ID
     */
    FormalParamsNode* params = new FormalParamsNode({});
    while (this->is_token_in_firsts("params")) {
        Leaf* leaf = new Leaf(this->check_get_next(Type::id));
        params->add_child(leaf);
        if (this->get_token().getType() == Type::comma) {
            this->next_token();
        }
    }
    return params;
};

FunctionNode * Parser::function_def() {
    /*
    function_def:
        | DEFKW ID LPR params RPR COLON block
        | DEFKW ID LPR RPR COLON block
     */
    this->check_get_next(Type::defkw);
    Leaf* id = new Leaf(this->check_get_next(Type::id));
    this->check_get_next(Type::lpr);
    FormalParamsNode* params;
    if (this->is_token_in_firsts("params")) {
        params = this->params();
    } else {
        params = new FormalParamsNode({});
    }
    this->check_get_next(Type::rpr);
    this->check_get_next(Type::colon);
    BlockNode* body = this->block();
    return new FunctionNode(id, params, body);
};

IfNode * Parser::if_stmt() {
    /*
    if_stmt:
        | IFKW expression COLON block elif_stmt
        | IFKW expression COLON block else_block
        | IFKW expression COLON block
     */
    this->check_get_next(Type::ifkw);
    ExpressionNode* condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode* body = this->block();
    IfNode* if_node = new IfNode(condition, body);
    if (this->is_token_in_firsts("elif_stmt")) {
        if_node->next_elif = this->elif_stmt();
    } else if (this->is_token_in_firsts("else_block")) {
        if_node->next_else = this->else_block();
    }
    return if_node;
};

ElifNode * Parser::elif_stmt() {
    /*
    elif_stmt:
        | ELIFKW expression COLON block elif_stmt
        | ELIFKW expression COLON block else_block
        | ELIFKW expression COLON block
     */
    this->check_get_next(Type::elifkw);
    ExpressionNode* condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode* body = this->block();
    ElifNode* elif_node = new ElifNode(condition, body);
    if (this->is_token_in_firsts("elif_stmt")) {
        elif_node->next_elif = this->elif_stmt();
    } else if (this->is_token_in_firsts("else_block")) {
        elif_node->next_else = this->else_block();
    }
    return elif_node;
};

ElseNode * Parser::else_block() {
    /*
    else_block:
        | ELSEKW COLON block
     */
    this->check_get_next(Type::elsekw);
    this->check_get_next(Type::colon);
    BlockNode* body = this->block();
    return new ElseNode(body);
};

WhileNode * Parser::while_stmt() {
    /*
    while_stmt:
        | WHILEKW expression COLON block
     */
    this->check_get_next(Type::whilekw);
    ExpressionNode* condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode* body = this->block();
    return new WhileNode(condition, body);
};

ForNode * Parser::for_stmt() {
    /*
    for_stmt:
        | FORKW ID IN expression COLON block
     */
    this->check_get_next(Type::forkw);
    Token ID = this->check_get_next(Type::id);
    this->check_get_next(Type::in);
    ExpressionNode* condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode* body = this->block();
    return new ForNode(new Leaf(ID), condition, body);
};

BlockNode * Parser::block() {
    /*
    block:
        | NEWLINE INDENT statements DEDENT
        | simple_stmts
     */
    BlockNode* block = new BlockNode();
    if (this->get_token().getType() == Type::newline) {
        this->check_get_next(Type::newline);
        this->check_get_next(Type::indent);
        this->statements(block);
        this->check_get_next(Type::dedent);
    } else {
        this->simple_stmts(block);
    }
    return block;
};
