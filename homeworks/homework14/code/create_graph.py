import sys

import matplotlib.pyplot as plt
import numpy as np

file = sys.argv[1]
to_save = sys.argv[2]
x, y = np.loadtxt(file, delimiter=",", unpack=True)

plt.ylim(0, np.max(y))

plt.plot(x, y)
plt.xlabel("Number of Pages")
plt.ylabel("Time (ms)")
plt.title("Time vs Number of Pages")

plt.savefig(to_save)
