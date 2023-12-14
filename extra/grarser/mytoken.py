from dataclasses import dataclass

TOKEN_TERM = 0
TOKEN_NONTERM = 1

@dataclass(frozen=True)
class MyToken:
    type: int
    symbol: str
    
    def isTerm(self) -> bool:
        return self.type == TOKEN_TERM
    
    def isNonTerm(self) -> bool:
        return self.type == TOKEN_NONTERM
    
    def __str__(self) -> str:
        t = ""
        if self.isNonTerm():
            t = "NonTerm"
        else:
            t = "Term"
        return t + " " + self.symbol

    def __repr__(self):
        t = ""
        if self.isNonTerm():
            t = "NonTerm"
        else:
            t = "Term"
        return t + " " + self.symbol
