from grarser import grarser
import os

grammar_file_name = os.path.join(os.path.dirname(__file__), "Grammar.txt") 

gr = grarser.get_grammar_from_file(grammar_file_name)
firsts = grarser.find_firsts(gr)

cpp_strs = []
for item in firsts.items():
    nonterm, terms = item
    s = "{\"" + nonterm.symbol + "\", "
    term_strs = {f"Type::{term.symbol.lower()}" for term in terms}
    s += "{" + ", ".join(term_strs) + "}"
    s += "}"
    cpp_strs.append(s)
s = "{" + ",\n".join(cpp_strs) + "};"
with open("FIRSTS.txt", "w") as f:
    f.write(s)