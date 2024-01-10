from .grammar import Grammar
from .mytoken import MyToken, TOKEN_TERM, TOKEN_NONTERM
from collections import defaultdict


def is_terminal(s: str):
    return s.isupper()


def get_grammar_from_file(file_path: str) -> Grammar:
    file = open(file_path, "r")

    rules = []

    for full_line in file:
        line = full_line.strip()
        if line.startswith("#") or not line:
            continue
        elif line.startswith("|"):
            items = line.split()[1:]
            items = tuple([MyToken(not is_terminal(s), s) for s in items])
            rules[-1][1].add(items)
        else:
            rules.append([MyToken(TOKEN_NONTERM, line[:-1]), set()])

    terminals = set()
    nonterminals = set()

    for rule in rules:
        nonterminals.add(rule[0])
        for subrule in rule[1]:
            for token in subrule:
                if token.isTerm():
                    terminals.add(token)
                else:
                    nonterminals.add(token)

    rules = dict(rules)
    axiom = "file"

    nonterm_str = set(item.symbol for item in nonterminals)
    term_str = set(item.symbol for item in terminals)

    gr = Grammar(nonterm_str, term_str, rules, axiom)

    return gr


def find_firsts(gr: Grammar) -> dict[MyToken, set[MyToken]]:
    rules = gr.P

    # Какие первые терминалы можно получить, переходя из этого нетерминала
    firsts: dict[MyToken, set[MyToken]] = defaultdict(set)

    def dfs(curr_node: MyToken, origin: MyToken):
        if curr_node.isTerm():  # Узел грамматики - терминал
            if origin in firsts[origin]: raise Exception("Недопустимая грамматика")
            firsts[origin].add(curr_node)
        else:  # Узел грамматики
            node_rules = rules[curr_node]  # Что можно вывести из этого узла
            for node_rule in node_rules:
                dfs(node_rule[0], origin)

    for nonterm in gr.N:
        dfs(nonterm, nonterm)

    return firsts


def rules_to_numbered_rules(rules):
    numbered_rules = []
    for nonterm in rules:
        for subrule in rules[nonterm]:
            numbered_rules.append((nonterm, subrule))
    return numbered_rules

