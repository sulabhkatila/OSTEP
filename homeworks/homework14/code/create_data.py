import os
import subprocess
import sys

ITERATIONS = 10

if __name__ == "__main__":
    path = sys.argv[1]
    file_name = sys.argv[2]
    with open(file_name, "w") as f:
        print("Creating data...")

        save_stdout = os.dup(1)
        os.dup2(f.fileno(), 1)

        for i in range(1, 10):
            number_of_pages = 2**i
            f.write(f"{number_of_pages},")
            f.flush()
            subprocess.call([path, str(number_of_pages), str(ITERATIONS)])

        os.dup2(save_stdout, 1)
        os.close(save_stdout)

        print("Data created.")
