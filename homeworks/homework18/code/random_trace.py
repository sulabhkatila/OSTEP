import random
import sys


def random_trace(n):
    res = ""
    for _ in range(n):
        res += str(random.randint(0, n)) + "\n"
    return res


if __name__ == "__main__":
    n = int(sys.argv[1])
    print(random_trace(n))
