from .mytoken import *
from .mytoken import MyToken as Tk

class Grammar:
    N: set[Tk]
    T: set[Tk]
    P: dict[Tk, set[tuple[Tk, ...]]]
    S: Tk

    def __init__(self, N: set[str], T: set[str], P: dict[Tk, set[tuple[Tk, ...]]], S: str):
        self.N = set([Tk(TOKEN_NONTERM, i) for i in N]) # Нетерминалы. Например, {"E", "T", "F"}
        self.T = set([Tk(TOKEN_TERM, i) for i in T]) # Терминалы. Например, {"a", "(", ")"}
        self.P = P # Правила. Например, { "E": {"E+T", "T"}, "T": {"T*F", "F"}, "F": {"(E)", "a"} }
        self.S = Tk(TOKEN_NONTERM, S) # Аксиома (начальный символ грамматики). Например, "E"

    def getN(self):
        return set([i.symbol for i in self.N])

    def getT(self):
        return set([i.symbol for i in self.T])

    def getS(self):
        return self.S.symbol

    def __str__(self):
        """Метод для красивого вывода грамматики с помощью print()"""
        rules = ""
        for non_term in self.P.keys():
            # rules += f"\n\t\t{non_term.symbol} -> " + " | ".join([":".join([j.symbol for j in i]) for i in self.P[non_term]])
            for subrule in self.P[non_term]:
                rules += f"\n{non_term.symbol} -> " + " ".join([j.symbol for j in subrule])
        return f"""{{
    Нетерминалы: {self.getN()}
    Терминалы: {self.getT()}
    Правила: {rules}
    Аксиома: {self.getS()}
}}"""

    def __eq__(self, other):
        """Оператор равенства грамматик"""
        if set(self.P.keys()) == set(other.P.keys()):
            for rule in self.P:
                if self.P[rule] != other.P[rule]:
                    return False
        else:
            return False
        return self.N == other.N and \
               self.T == other.T and \
               self.S == other.S
