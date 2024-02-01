def one() -> int:
    return 1 + 9

a: int = 1 + 2 * 3
b: int = a + one()
c: int = 4 * b
d: int = 5 * a + b + c

putf(c)

