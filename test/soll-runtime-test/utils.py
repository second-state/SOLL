def print_red(x):
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    print(FAIL + x + ENDC)


def print_green(x):
    GREEN = '\033[92m'
    ENDC = '\033[0m'
    print(GREEN + x + ENDC)
