import sys

failure = False

for line in sys.stdin:
    l = line.strip()
    print(l)
    if l.startswith('FAIL'):
        failure = True
if failure:
    exit(1)
