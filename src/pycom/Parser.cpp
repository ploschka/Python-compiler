#include <pycom/parser/Parser.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/AST/ASTNode.hpp>

#include <stdexcept>
#include <iostream>
#include <map>
#include <set>

static const std::map<std::string, std::set<Type>> FIRSTS = {
    {"comparison", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr}},
    {"if_stmt", {Type::ifkw}},
    {"block", {Type::string, Type::plus, Type::newline, Type::returnkw, Type::breakkw, Type::passkw, Type::number, Type::id, Type::inv, Type::lpr, Type::minus, Type::continuekw, Type::lsbr, Type::notop}},
    {"assignment", {Type::id}},
    {"primary", {Type::string, Type::id, Type::number, Type::lpr, Type::lsbr}},
    {"disjunction", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr, Type::notop}},
    {"return_stmt", {Type::returnkw}},
    {"while_stmt", {Type::whilekw}},
    {"term", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr}},
    {"arguments", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr, Type::notop}},
    {"statements", {Type::string, Type::plus, Type::forkw, Type::returnkw, Type::breakkw, Type::passkw, Type::number, Type::ifkw, Type::inv, Type::lpr, Type::id, Type::minus, Type::whilekw, Type::continuekw, Type::lsbr, Type::notop, Type::defkw}},
    {"params", {Type::id}},
    {"list", {Type::lsbr}},
    {"type_hint", {Type::colon}},
    {"function_def", {Type::defkw}},
    {"statement", {Type::string, Type::plus, Type::forkw, Type::returnkw, Type::breakkw, Type::passkw, Type::number, Type::ifkw, Type::inv, Type::lpr, Type::id, Type::minus, Type::whilekw, Type::continuekw, Type::lsbr, Type::notop, Type::defkw}},
    {"inversion", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr, Type::notop}},
    {"for_stmt", {Type::forkw}},
    {"simple_stmt", {Type::string, Type::plus, Type::returnkw, Type::breakkw, Type::passkw, Type::number, Type::id, Type::inv, Type::lpr, Type::minus, Type::continuekw, Type::lsbr, Type::notop}},
    {"list_values", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr, Type::notop}},
    {"factor", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr}},
    {"sum", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr}},
    {"compound_stmt", {Type::whilekw, Type::forkw, Type::defkw, Type::ifkw}},
    {"expression", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr, Type::notop}},
    {"atom", {Type::string, Type::number, Type::id}},
    {"else_block", {Type::elsekw}},
    {"file", {Type::string, Type::eof, Type::plus, Type::forkw, Type::returnkw, Type::breakkw, Type::passkw, Type::number, Type::ifkw, Type::inv, Type::lpr, Type::id, Type::minus, Type::whilekw, Type::continuekw, Type::lsbr, Type::notop, Type::defkw}},
    {"simple_stmts", {Type::string, Type::plus, Type::returnkw, Type::breakkw, Type::passkw, Type::number, Type::id, Type::inv, Type::lpr, Type::minus, Type::continuekw, Type::lsbr, Type::notop}},
    {"elif_stmt", {Type::elifkw}},
    {"conjunction", {Type::string, Type::plus, Type::id, Type::number, Type::inv, Type::lpr, Type::minus, Type::lsbr, Type::notop}},
    {"func_call", {Type::lpr}}};

Parser::Parser() : token(Token("хуй", Type::bandass)){};

void Parser::setLexer(LexerInterface *_lexer)
{
    this->lexer = _lexer;
    this->next_token();
}

AST *Parser::getAST()
{
    return new AST(this->file());
}

// Возвращает True, если текущий токен кода можно получить, спускаясь по узлу грамматика grammar_node
bool Parser::is_token_in_firsts(std::string _grammar_node)
{
    return FIRSTS.at(_grammar_node).find(this->token.getType()) != FIRSTS.at(_grammar_node).end();
}

// Возвращает True, если текущий токен кода имеет тип _type
bool Parser::token_matches(Type _type)
{
    return this->get_token().getType() == _type;
}

// Возвращает True, если текущий токен кода имеет тип, имеющийся в types
bool Parser::token_matches_any(std::vector<Type> _types)
{
    for (Type type : _types)
    {
        if (this->get_token().getType() == type)
        {
            return true;
        }
    }
    return false;
}

void Parser::error(std::string _message)
{
    std::string pos = std::to_string(this->get_token().getPos());
    std::string row = std::to_string(this->get_token().getRow());
    _message += " pos=" + pos + " row=" + row;
    std::cout << _message << std::endl;
    throw std::runtime_error(_message);
}

void Parser::next_token()
{
    if (this->future_tokens.empty())
    {
        this->token = this->lexer->getToken();
    }
    else
    {
        // Забираем токен из списка будущих токенов
        this->token = this->future_tokens[0];
        this->future_tokens.erase(this->future_tokens.begin());
    }
}

Token Parser::forward(size_t _k)
{
    while (this->future_tokens.size() < _k)
    {
        this->future_tokens.push_back(this->lexer->getToken());
    }
    return this->future_tokens[_k - 1];
}

Token Parser::get_token()
{
    return this->token;
}

// Проверяет, что текущий токен имеет тип type, возвращает его и сразу получает следующий токен
Token Parser::check_get_next(Type _type)
{
    if (this->token.getType() != _type)
    {
        this->error("Unexpected token");
    }
    Token tmp = this->token;
    this->next_token();
    return tmp;
}

/**
 * file:
 *  | statements EOF
 *  | EOF
 */
ProgramNode *Parser::file()
{
    ProgramNode *file = new ProgramNode({});
    if (this->is_token_in_firsts("statements"))
    {
        this->statements(file);
    }
    file->add_child(new Leaf(this->check_get_next(Type::eof)));
    return file;
};

/**
 * statements:
 *  | statement statements
 *  | statement
 */
void Parser::statements(BlockNode *_parent_block)
{
    while (this->is_token_in_firsts("statements"))
    {
        this->statement(_parent_block);
    }
};

/**
 *
 * statement:
 *  | compound_stmt
 *  | simple_stmts
 */
void Parser::statement(BlockNode *_parent_block)
{
    if (this->is_token_in_firsts("compound_stmt"))
    {
        this->compound_stmt(_parent_block);
    }
    else if (this->is_token_in_firsts("simple_stmts"))
    {
        this->simple_stmts(_parent_block);
    }
    else
    {
        this->error("statement");
    }
};

/**
 * simple_stmts:
 *  | simple_stmt NEWLINE simple_stmts
 *  | simple_stmt NEWLINE
 */
void Parser::simple_stmts(BlockNode *_parent_block)
{
    while (this->is_token_in_firsts("simple_stmts"))
    {
        this->simple_stmt(_parent_block);
        this->check_get_next(Type::newline);
    }
};

/**
 * simple_stmt:
 *  | assignment
 *  | expression
 *  | return_stmt
 *  | PASS
 *  | CONTINUEKW
 */
void Parser::simple_stmt(BlockNode *_parent_block)
{
    if (this->is_token_in_firsts("assignment") 
        && (this->forward(1).getType() == Type::assign || this->forward(1).getType() == Type::colon))
    {
        _parent_block->add_child(this->assignment());
    }
    else if (this->is_token_in_firsts("expression"))
    {
        _parent_block->add_child(this->expression());
    }
    else if (this->is_token_in_firsts("return_stmt"))
    {
        _parent_block->add_child(this->return_stmt());
    }
    else if (this->token_matches_any({Type::passkw, Type::breakkw, Type::continuekw}))
    {
        _parent_block->add_child(new Leaf(this->get_token()));
        this->next_token();
    }
    else
    {
        this->error("simple_stmt");
    }
};

/**
 * type_hint:
 *  | COLON ID
 */
TypeNode *Parser::type_hint()
{
    this->check_get_next(Type::colon);
    Token type_token = this->check_get_next(Type::id);
    return new TypeNode(type_token);
}

/**
 * assignment:
 *  | ID ASSIGN expression
 *  | ID type_hint ASSIGN expression
 */
AssignmentNode *Parser::assignment()
{
    Leaf *left = new Leaf(this->check_get_next(Type::id));
    TypeNode *type = nullptr;
    if (this->is_token_in_firsts("type_hint"))
    {
        type = this->type_hint();
    }
    this->check_get_next(Type::assign);
    ExpressionNode *right = this->expression();
    return new AssignmentNode(left, type, right);
};

/**
 * return_stmt:
 *  | RETURNKW
 *  | RETURNKW expression
 */
ReturnNode *Parser::return_stmt()
{
    this->check_get_next(Type::returnkw);
    if (this->is_token_in_firsts("expression"))
    {
        return new ReturnNode(this->expression());
    }
    else
    {
        Token None = Token("None", Type::id);
        return new ReturnNode(new Leaf(None));
    }
};

/**
 * expression:
 *  | disjunction
 */
ExpressionNode *Parser::expression()
{
    return this->disjunction();
};

/**
 * disjunction:
 *  | conjunction OR disjunction
 *  | conjunction
 */
ExpressionNode *Parser::disjunction()
{
    ExpressionNode *left = this->conjunction();
    if (this->token_matches(Type::orop))
    {
        Token op = this->check_get_next(Type::orop);
        ExpressionNode *right = this->conjunction();
        BinaryNode *op_node = new BinaryNode(left, new Leaf(op), right);
        while (this->token_matches(Type::orop))
        {
            op = this->check_get_next(Type::orop);
            right = this->conjunction();
            op_node = new BinaryNode(op_node, new Leaf(op), right);
        }
        return op_node;
    }
    else
    {
        return left;
    }
};

/**
 * conjunction:
 *  | inversion AND conjunction
 *  | inversion
 */
ExpressionNode *Parser::conjunction()
{
    ExpressionNode *left = this->inversion();
    if (this->token_matches(Type::andop))
    {
        Token op = this->check_get_next(Type::andop);
        ExpressionNode *right = this->inversion();
        BinaryNode *op_node = new BinaryNode(left, new Leaf(op), right);
        while (this->token_matches(Type::andop))
        {
            op = this->check_get_next(Type::andop);
            right = this->inversion();
            op_node = new BinaryNode(op_node, new Leaf(op), right);
        }
        return op_node;
    }
    else
    {
        return left;
    }
};

/**
 * inversion:
 *  | NOT inversion
 *  | comparison
 */
ExpressionNode *Parser::inversion()
{
    if (this->token_matches(Type::notop))
    {
        Token op = this->check_get_next(Type::notop);
        ExpressionNode *operand = this->inversion();
        return new UnaryNode(new Leaf(op), operand);
    }
    else
    {
        return this->comparison();
    }
};

/**
 * comparison:
 *  | sum GREATER comparison
 *  | sum LESS comparison
 *  | sum EQUAL comparison
 *  | sum NOTEQ comparison
 *  | sum GREQUAL comparison
 *  | sum LEQUAL comparison
 *  | sum
 */
ExpressionNode *Parser::comparison()
{
    ExpressionNode *left = this->sum();
    std::vector<Type> ok_ops = {Type::greater, Type::less, Type::equal, Type::noteq, Type::grequal, Type::lequal};
    if (this->token_matches_any(ok_ops))
    {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode *right = this->sum();
        BinaryNode *op_node = new BinaryNode(left, new Leaf(op), right);
        while (this->token_matches_any(ok_ops))
        {
            op = this->get_token();
            this->next_token();
            right = this->sum();
            op_node = new BinaryNode(op_node, new Leaf(op), right);
        }
        return op_node;
    }
    else
    {
        return left;
    }
}

/**
 * sum:
 *  | term PLUS sum
 *  | term MINUS sum
 *  | term
 */
ExpressionNode *Parser::sum()
{
    ExpressionNode *left = this->term();
    std::vector<Type> ok_ops = {Type::plus, Type::minus};
    if (this->token_matches_any(ok_ops))
    {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode *right = this->term();
        BinaryNode *op_node = new BinaryNode(left, new Leaf(op), right);
        while (this->token_matches_any(ok_ops))
        {
            op = this->get_token();
            this->next_token();
            right = this->term();
            op_node = new BinaryNode(op_node, new Leaf(op), right);
        }
        return op_node;
    }
    else
    {
        return left;
    }
};

/**
 * term:
 *  | factor STAR term
 *  | factor DIV term
 *  | factor IDIV term
 *  | factor MOD term
 *  | factor
 */
ExpressionNode *Parser::term()
{
    ExpressionNode *left = this->factor();
    std::vector<Type> ok_ops = {Type::star, Type::div, Type::idiv, Type::mod};
    if (this->token_matches_any(ok_ops))
    {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode *right = this->factor();
        BinaryNode *op_node = new BinaryNode(left, new Leaf(op), right);
        while (this->token_matches_any(ok_ops))
        {
            op = this->get_token();
            this->next_token();
            right = this->factor();
            op_node = new BinaryNode(op_node, new Leaf(op), right);
        }
        return op_node;
    }
    else
    {
        return left;
    }
};

/**
 * factor:
 *  | PLUS factor
 *  | MINUS factor
 *  | INV factor
 *  | primary
 */
ExpressionNode *Parser::factor()
{
    if (this->token_matches_any({Type::plus, Type::minus, Type::inv}))
    {
        Token op = this->get_token();
        this->next_token();
        ExpressionNode *operand = this->factor();
        return new UnaryNode(new Leaf(op), operand);
    }
    else
    {
        return this->primary();
    }
};

/**
 * primary:
 *  | LPR expression RPR
 *  | LPR expression RPR func_call
 *  | atom
 *  | atom func_call
 *  | list
 */
ExpressionNode *Parser::primary()
{
    if (this->token_matches(Type::lpr))
    {
        this->check_get_next(Type::lpr);
        auto expr = this->expression();
        this->check_get_next(Type::rpr);

        if (this->is_token_in_firsts("func_call"))
        {
            ActualParamsNode *params = this->func_call();
            return new CallNode(expr, params);
        }
        return expr;
    }
    else if (this->is_token_in_firsts("atom"))
    {
        auto atom = this->atom();
        if (this->is_token_in_firsts("func_call"))
        {
            ActualParamsNode *params = this->func_call();
            return new CallNode(atom, params);
        }
        return atom;
    }
    else if (this->is_token_in_firsts("list"))
    {
        return this->list();
    }
    this->error("primary");
};

/**
 * list:
 *  | LSBR list_values RSBR
 *  | LSBR RSBR
 */
ListNode *Parser::list()
{
    this->check_get_next(Type::lsbr);
    auto node = new ListNode(this->list_values());
    this->check_get_next(Type::rsbr);
    return node;
}

/**
 * list_values:
 *  | expression COMMA arguments
 *  | expression
 */
std::vector<ExpressionNode *> Parser::list_values()
{
    std::vector<ExpressionNode *> values = {};

    while (this->is_token_in_firsts("expression"))
    {
        values.push_back(this->expression());
        if (this->token_matches(Type::comma))
        {
            this->next_token();
        }
    }
    return values;
};

/**
 * func_call:
 *  | LPR arguments RPR
 *  | LPR RPR
 */
ActualParamsNode *Parser::func_call()
{
    this->check_get_next(Type::lpr);
    if (this->is_token_in_firsts("arguments"))
    {
        ActualParamsNode *params = this->arguments();
        this->check_get_next(Type::rpr);
        return params;
    }
    else
    {
        this->check_get_next(Type::rpr);
        return new ActualParamsNode({});
    }
};

/**
 * atom:
 *  | ID
 *  | STRING
 *  | NUMBER
 */
Leaf *Parser::atom()
{
    if (this->token_matches_any({Type::id, Type::string, Type::number}))
    {
        Leaf *leaf = new Leaf(this->get_token());
        this->next_token();
        return leaf;
    }
    else
    {
        this->error("atom");
        return nullptr;
    }
};

/**
 * arguments:
 *  | expression COMMA arguments
 *  | expression
 */
ActualParamsNode *Parser::arguments()
{
    ActualParamsNode *params = new ActualParamsNode({});
    while (this->is_token_in_firsts("arguments"))
    {
        params->add_child(this->expression());
        if (this->token_matches(Type::comma))
        {
            this->next_token();
        }
    }
    return params;
};

/**
 * compound_stmt:
 *  | function_def
 *  | if_stmt
 *  | for_stmt
 *  | while_stmt
 */
void Parser::compound_stmt(BlockNode *_parent_block)
{
    if (this->is_token_in_firsts("function_def"))
    {
        _parent_block->add_child(this->function_def());
    }
    else if (this->is_token_in_firsts("if_stmt"))
    {
        _parent_block->add_child(this->if_stmt());
    }
    else if (this->is_token_in_firsts("for_stmt"))
    {
        _parent_block->add_child(this->for_stmt());
    }
    else if (this->is_token_in_firsts("while_stmt"))
    {
        _parent_block->add_child(this->while_stmt());
    }
    else
    {
        this->error("compound_stmt");
    }
};

/**
 * params:
 *  | ID type_hint COMMA params
 *  | ID type_hint
 */
FormalParamsNode *Parser::params()
{
    FormalParamsNode *params = new FormalParamsNode({}, {});
    while (this->is_token_in_firsts("params"))
    {
        Leaf *leaf = new Leaf(this->check_get_next(Type::id));
        TypeNode *type = this->type_hint();
        params->add_child(leaf, type);
        if (this->token_matches(Type::comma))
        {
            this->next_token();
        }
    }
    return params;
};

/**
 * function_def
 *  | DEFKW ID LPR params RPR COLON block
 *  | DEFKW ID LPR RPR COLON block
 */
FunctionNode *Parser::function_def()
{
    this->check_get_next(Type::defkw);
    Leaf *id = new Leaf(this->check_get_next(Type::id));
    this->check_get_next(Type::lpr);
    FormalParamsNode *params;
    if (this->is_token_in_firsts("params"))
    {
        params = this->params();
    }
    else
    {
        params = new FormalParamsNode({}, {});
    }
    this->check_get_next(Type::rpr);
    this->check_get_next(Type::colon);
    BlockNode *body = this->block();
    return new FunctionNode(id, params, body);
};

/**
 * if_stmt:
 *  | IFKW expression COLON block elif_stmt
 *  | IFKW expression COLON block else_block
 *  | IFKW expression COLON block
 */
IfNode *Parser::if_stmt()
{
    this->check_get_next(Type::ifkw);
    ExpressionNode *condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode *body = this->block();
    IfNode *if_node = new IfNode(condition, body);
    if (this->is_token_in_firsts("elif_stmt"))
    {
        if_node->next_elif = this->elif_stmt();
    }
    else if (this->is_token_in_firsts("else_block"))
    {
        if_node->next_else = this->else_block();
    }
    return if_node;
};

/**
 * elif_stmt:
 *  | ELIFKW expression COLON block elif_stmt
 *  | ELIFKW expression COLON block else_block
 *  | ELIFKW expression COLON block
 */
ElifNode *Parser::elif_stmt()
{
    this->check_get_next(Type::elifkw);
    ExpressionNode *condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode *body = this->block();
    ElifNode *elif_node = new ElifNode(condition, body);
    if (this->is_token_in_firsts("elif_stmt"))
    {
        elif_node->next_elif = this->elif_stmt();
    }
    else if (this->is_token_in_firsts("else_block"))
    {
        elif_node->next_else = this->else_block();
    }
    return elif_node;
};

/**
 * else_block:
 *  | ELSEKW COLON block
 */
ElseNode *Parser::else_block()
{
    this->check_get_next(Type::elsekw);
    this->check_get_next(Type::colon);
    BlockNode *body = this->block();
    return new ElseNode(body);
};

/**
 * while_stmt:
 *  | WHILEKW expression COLON block
 */
WhileNode *Parser::while_stmt()
{
    this->check_get_next(Type::whilekw);
    ExpressionNode *condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode *body = this->block();
    return new WhileNode(condition, body);
};

/**
 * for_stmt:
 *  | FORKW ID IN expression COLON block
 */
ForNode *Parser::for_stmt()
{
    this->check_get_next(Type::forkw);
    Token ID = this->check_get_next(Type::id);
    this->check_get_next(Type::in);
    ExpressionNode *condition = this->expression();
    this->check_get_next(Type::colon);
    BlockNode *body = this->block();
    return new ForNode(new Leaf(ID), condition, body);
};

/**
 * block:
 *  | NEWLINE INDENT statements DEDENT
 *  | simple_stmts
 */
BlockNode *Parser::block()
{
    BlockNode *block = new BlockNode();
    if (this->token_matches(Type::newline))
    {
        this->check_get_next(Type::newline);
        this->check_get_next(Type::indent);
        this->statements(block);
        this->check_get_next(Type::dedent);
    }
    else
    {
        this->simple_stmts(block);
    }
    return block;
}
