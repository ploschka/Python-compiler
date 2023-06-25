#include "gtest/gtest.h"
#include <pycom/parser/Parser.hpp>
#include <pycom/token/Token.hpp>
#include <fstream>
#include <pycom/lexer/Lexer.hpp>
#include <pycom/example_dependencies/NodeToVectorVisitor.h>


std::pair<std::vector<std::string>, std::vector<std::string>> get_node_vectors(
        std::string test_code_filename, ProgramNode *target_root) {
    /*
     * Синтаксические деревья, полученные из файла test_code_filename и корня target_root
     * превращает в вектор строк
     */
    std::ifstream input(test_code_filename);
    auto lexer = new Lexer;
    auto parser = new Parser;
    lexer->open(input);
    parser->setLexer(lexer);
    auto test_root = parser->getAST();

    auto test_visitor = new NodeToVectorVisitor();
    auto target_visitor = new NodeToVectorVisitor();
    test_root->accept(test_visitor);
    target_root->accept(target_visitor);
    std::vector<std::string> test_nodes = test_visitor->nodes;
    std::vector<std::string> target_nodes = target_visitor->nodes;

    return {
        test_nodes,
        target_nodes
    };
}


TEST(ExpressionTest, Assignment) {
    std::string test_code_file = "scripts/test_Assign.py";
    ProgramNode *target_root = new ProgramNode(
        {
            new AssignmentNode(
                new Leaf(Token("a", Type::id)),
                new VariableNode({
                    new Leaf(Token("3",
                    Type::number))
                })
            ),
            new Leaf(Token("", Type::eof))
        }
    );
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, Plus) {
    std::string test_code_file = "scripts/test_Plus.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new VariableNode({new Leaf(Token("a", Type::id))}),
            new Leaf(Token("", Type::plus)),
            new VariableNode({new Leaf(Token("10", Type::number))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, ManyPlus) {
    std::string test_code_file = "scripts/test_ManyPlus.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new BinaryNode(
                new BinaryNode(
                    new VariableNode({new Leaf(Token("1", Type::number))}),
                    new Leaf(Token("", Type::plus)),
                    new VariableNode({new Leaf(Token("2", Type::number))})
                ),
                new Leaf(Token("", Type::plus)),
                new VariableNode({new Leaf(Token("3", Type::number))})
            ),
            new Leaf(Token("", Type::plus)),
            new VariableNode({new Leaf(Token("g", Type::id))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, PlusMinus) {
    std::string test_code_file = "scripts/test_PlusMinus.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new BinaryNode(
                new BinaryNode(
                    new BinaryNode(
                        new VariableNode({new Leaf(Token("1", Type::number))}),
                        new Leaf(Token("", Type::plus)),
                        new VariableNode({new Leaf(Token("2", Type::number))})
                    ),
                    new Leaf(Token("", Type::minus)),
                    new VariableNode({new Leaf(Token("3", Type::number))})
                ),
                new Leaf(Token("", Type::plus)),
                new VariableNode({new Leaf(Token("4", Type::number))})
            ),
            new Leaf(Token("", Type::minus)),
            new VariableNode({new Leaf(Token("5", Type::number))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, Mul) {
    std::string test_code_file = "scripts/test_Mul.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new VariableNode({new Leaf(Token("a", Type::id))}),
            new Leaf(Token("", Type::star)),
            new VariableNode({new Leaf(Token("b", Type::id))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, MulWithDiv) {
    std::string test_code_file = "scripts/test_MulWithDiv.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new BinaryNode(
                new VariableNode({new Leaf(Token("a", Type::id))}),
                new Leaf(Token("", Type::star)),
                new VariableNode({new Leaf(Token("b", Type::id))})
            ),
            new Leaf(Token("", Type::div)),
            new VariableNode({new Leaf(Token("100", Type::number))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, And) {
    std::string test_code_file = "scripts/test_And.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new VariableNode({new Leaf(Token("a", Type::id))}),
            new Leaf(Token("", Type::andop)),
            new VariableNode({new Leaf(Token("b", Type::id))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, Or) {
    std::string test_code_file = "scripts/test_Or.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new VariableNode({new Leaf(Token("True", Type::id))}),
            new Leaf(Token("", Type::orop)),
            new VariableNode({new Leaf(Token("False", Type::id))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, Not) {
    std::string test_code_file = "scripts/test_Not.py";
    ProgramNode *target_root = new ProgramNode({
        new UnaryNode(
            new Leaf(Token("", Type::notop)),
            new BinaryNode(
                new VariableNode({new Leaf(Token("1", Type::number))}),
                new Leaf(Token("", Type::plus)),
                new VariableNode({new Leaf(Token("2", Type::number))})
            )
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, LogicPriority) {
    std::string test_code_file = "scripts/test_LogicPriority.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new BinaryNode(
                new BinaryNode(
                    new VariableNode({new Leaf(Token("A", Type::id))}),
                    new Leaf(Token("", Type::andop)),
                    new VariableNode({new Leaf(Token("B", Type::id))})
                ),
                new Leaf(Token("", Type::orop)),
                new VariableNode({new Leaf(Token("C", Type::id))})
            ),
            new Leaf(Token("", Type::orop)),
            new BinaryNode(
                new UnaryNode(
                    new Leaf(Token("", Type::notop)),
                    new VariableNode({new Leaf(Token("D", Type::id))})
                ),
                new Leaf(Token("", Type::andop)),
                new VariableNode({new Leaf(Token("F", Type::id))})
            )
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, ComparisonEqualPriority) {
    std::string test_code_file = "scripts/test_ComparisonEqualPriority.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new BinaryNode(
                new BinaryNode(
                    new VariableNode({new Leaf(Token("1", Type::number))}),
                    new Leaf(Token("", Type::less)),
                    new VariableNode({new Leaf(Token("x", Type::id))})
                ),
                new Leaf(Token("", Type::equal)),
                new VariableNode({new Leaf(Token("10", Type::number))})
            ),
            new Leaf(Token("", Type::grequal)),
            new VariableNode({new Leaf(Token("100", Type::number))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, UnaryOp) {
    std::string test_code_file = "scripts/test_UnaryOp.py";
    ProgramNode *target_root = new ProgramNode({
        new UnaryNode(
            new Leaf(Token("", Type::minus)),
            new VariableNode({new Leaf(Token("10", Type::number))})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ExpressionTest, ArithmOpPriority) {
    std::string test_code_file = "scripts/test_ArithmOpPriority.py";
    ProgramNode *target_root = new ProgramNode({
        new BinaryNode(
            new BinaryNode(
                new BinaryNode(
                    new UnaryNode(
                        new Leaf(Token("", Type::minus)),
                        new VariableNode({new Leaf(Token("10", Type::number))})
                    ),
                    new Leaf(Token("", Type::div)),
                    new VariableNode({new Leaf(Token("0", Type::number))})
                ),
                new Leaf(Token("", Type::plus)),
                new UnaryNode(
                    new Leaf(Token("", Type::plus)),
                    new VariableNode({new Leaf(Token("x", Type::id))})
                )
            ),
            new Leaf(Token("", Type::minus)),
            new BinaryNode(
                new UnaryNode(
                    new Leaf(Token("", Type::minus)),
                    new VariableNode({new Leaf(Token("5", Type::number))})
                ),
                new Leaf(Token("", Type::star)),
                new VariableNode({new Leaf(Token("10", Type::number))})
            )
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(IfTest, If) {
    std::string test_code_file = "scripts/test_If.py";
    ProgramNode *target_root = new ProgramNode({
        new IfNode(
            new VariableNode({new Leaf(Token("True", Type::id))}),
            new BlockNode({
                new Leaf(Token("", Type::breakkw))
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(IfTest, IfElse) {
    std::string test_code_file = "scripts/test_IfElse.py";
    auto if_node = new IfNode(
        new VariableNode({new Leaf(Token("True", Type::id))}),
        new BlockNode({
            new Leaf(Token("", Type::breakkw))
        })
    );
    auto else_node = new ElseNode(
        new BlockNode({
            new Leaf(Token("", Type::breakkw))
        })
    );
    if_node->next_else = else_node;
    ProgramNode *target_root = new ProgramNode({
        if_node,
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(IfTest, IfElifElse) {
    std::string test_code_file = "scripts/test_IfElifElse.py";
    
    auto if_node = new IfNode(
        new VariableNode({new Leaf(Token("True", Type::id))}),
        new BlockNode({
            new Leaf(Token("", Type::breakkw))
        })
    );
    auto elif_node = new ElifNode(
        new VariableNode({new Leaf(Token("True", Type::id))}),
        new BlockNode({
            new Leaf(Token("", Type::breakkw))
        })
    );
    auto else_node = new ElseNode(
        new BlockNode({
            new Leaf(Token("", Type::breakkw))
        })
    );
    if_node->next_elif = elif_node;
    elif_node->next_else = else_node;
    ProgramNode *target_root = new ProgramNode({
        if_node,
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(WhileTest, While) {
    std::string test_code_file = "scripts/test_While.py";
    ProgramNode *target_root = new ProgramNode({
        new WhileNode(
            new VariableNode({new Leaf(Token("True", Type::id))}),
            new BlockNode({
                new Leaf(Token("", Type::breakkw))
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ForTest, For) {
    std::string test_code_file = "scripts/test_For.py";
    ProgramNode *target_root = new ProgramNode({
        new ForNode(
            new Leaf(Token("i", Type::id)),
            new CallNode(
                new VariableNode({new Leaf(Token("range", Type::id))}),
                new ActualParamsNode({
                    new VariableNode({new Leaf(Token("100", Type::number))})
                })
            ),
            new BlockNode({
                new Leaf(Token("", Type::breakkw))
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(FunctionDefTest, DefinitionNoParams) {
    std::string test_code_file = "scripts/test_DefinitionNoParams.py";
    ProgramNode *target_root = new ProgramNode({
        new FunctionNode(
            new Leaf(Token("func", Type::id)),
            new FormalParamsNode({}),
            new BlockNode({
                new Leaf(Token("", Type::breakkw))
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(FunctionDefTest, DefinitionWithParams) {
    std::string test_code_file = "scripts/test_DefinitionWithParams.py";
    ProgramNode *target_root = new ProgramNode({
        new FunctionNode(
            new Leaf(Token("func", Type::id)),
            new FormalParamsNode({
                new Leaf(Token("a", Type::id)),
                new Leaf(Token("b", Type::id))
            }),
            new BlockNode({
                new Leaf(Token("", Type::breakkw))
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(FunctionCallTest, CallNoParams) {
    std::string test_code_file = "scripts/test_CallNoParams.py";
    ProgramNode *target_root = new ProgramNode({
        new CallNode(
            new VariableNode({new Leaf(Token("print", Type::id))}),
            new ActualParamsNode({})
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(FunctionCallTest, CallWithParams) {
    std::string test_code_file = "scripts/test_CallWithParams.py";
    ProgramNode *target_root = new ProgramNode({
        new CallNode(
            new VariableNode({new Leaf(Token("print", Type::id))}),
            new ActualParamsNode({
                new VariableNode({new Leaf(Token("1", Type::number))}),
                new VariableNode({new Leaf(Token("2", Type::number))}),
                new VariableNode({new Leaf(Token("3", Type::number))})
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ReturnTest, ReturnNothing) {
    std::string test_code_file = "scripts/test_ReturnNothing.py";
    ProgramNode *target_root = new ProgramNode({
        new FunctionNode(
            new Leaf(Token("func", Type::id)),
            new FormalParamsNode({}),
            new BlockNode({
                new ReturnNode(
                    new Leaf(Token("None", Type::id))
                )
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

TEST(ReturnTest, ReturnSomething) {
    std::string test_code_file = "scripts/test_ReturnSomething.py";
    ProgramNode *target_root = new ProgramNode({
        new FunctionNode(
            new Leaf(Token("func", Type::id)),
            new FormalParamsNode({}),
            new BlockNode({
                new ReturnNode(
                    new BinaryNode(
                        new VariableNode({new Leaf(Token("1", Type::number))}),
                        new Leaf(Token("", Type::plus)),
                        new VariableNode({new Leaf(Token("2", Type::number))})
                    )
                )
            })
        ),
        new Leaf(Token("", Type::eof))
    });
    auto nodes = get_node_vectors(test_code_file, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}