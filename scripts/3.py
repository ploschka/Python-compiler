def get_by_index(arr: list[int], i: int) -> int:
    curr_i: int = 0
    for el in arr:
        if curr_i == i:
            return el
        curr_i = curr_i + 1
    return -666

arr: list[int] = [1, 2, 3]
print(get_by_index(arr, 2))
