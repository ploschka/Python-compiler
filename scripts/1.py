def fib(a: int, b: int, n: int) -> int:
    if n == 0:
        return a
    else:
        return fib(b, a + b, n - 1)
    

puts(fib(0, 1, 10))
