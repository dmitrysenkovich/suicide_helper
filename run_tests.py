import subprocess


TESTS_COUNT = 2


# Just to make sure everything is ok
# even if we have some race condition
def main():
    for i in range(TESTS_COUNT):
        proc = subprocess.Popen('./test')


if __name__ == '__main__':
    main()
