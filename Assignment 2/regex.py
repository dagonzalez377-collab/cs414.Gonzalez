import re

pattern = r"^[A-Za-z_][A-Za-z0-9_]*$"
tests = ["myVar", "_temp2", "2fast"]

for t in tests:
    print(t, "->", bool(re.match(pattern, t)))


pattern = r"^(\(\d{3}\) \d{3}-\d{4}|\d{3}-\d{3}-\d{4})$"
tests = ["(256) 555-1234", "256-555-1234", "2565551234"]

for t in tests:
    print(t, "->", bool(re.match(pattern, t)))


pattern = r"^[+-]?\d+(\.\d+)?$"
tests = ["3.14", "-42", "+7.0"]

for t in tests:
    print(t, "->", bool(re.match(pattern, t)))


pattern = r"^(?:([01])[01]\1|([01])([01])\3\2)$"
tests = ["101", "000", "0110", "0100"]

for t in tests:
    print(t, "->", bool(re.match(pattern, t)))
