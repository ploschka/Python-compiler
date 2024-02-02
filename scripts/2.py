def one() -> int:
    return 1

a: int = 1 + 2 * 3
b: int = a + one()
c: int = 4 * b

puts(c)

