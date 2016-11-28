import sys

OK = '\033[92m'
FAIL = '\033[91m'
END = '\033[0m'

failures = 0
number_of_tests = 0
tests = []
results = []

for line in sys.stdin:
    l = line.strip()
    if l.startswith("Test"):
        number_of_tests += 1
        tests.append(l)
    if l.startswith('FAIL'):
        failures += 1
        results.append(FAIL + "FAIL" + END)
    if l.startswith('OK'):
        results.append(OK + " OK " + END)


print("Tests result:")

for test, result in zip(tests, results):
    print("[{}] {}".format(result, test))

if failures:
    print("{}/{} tests failed".format(failures, number_of_tests))
    exit(1)
else:
    print("All {} tests passed".format(number_of_tests))

