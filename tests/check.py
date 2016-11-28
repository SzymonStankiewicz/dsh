import sys

failures = 0
number_of_tests = 0

for line in sys.stdin:
    l = line.strip()
    print(l)
    if l.startswith("Test"):
        number_of_tests += 1
    if l.startswith('FAIL'):
        failures += 1


if failures:
    print("{}/{} tests failed".format(failures, number_of_tests))
    exit(1)
else:
    print("All {} tests passed".format(number_of_tests))

