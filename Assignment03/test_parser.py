# Quick sanity check - runs a few example commands through the
# tokenizer + parser and prints what comes out.

from tokenizer import tokenize
from parser import parse

valid_commands = [
    "ls",
    "ls /",
    "ls docs/reports",
    "cd",
    "cd /",
    "cd home/data",
    "cat resultsx.doc",
    "print budgetqq.xls",
    "exec programa.exe",
]

invalid_commands = [
    "delete resultsx.doc",   # not a real command
    "cat docs/reports",      # cat needs a filename, not a path
    "cat",                   # missing filename
    "cat short.tx",          # bad extension (needs 3 letters)
    "cd /toolongfoldername", # folder name over 8 chars
    "cd home2",              # digits not allowed
]

print("=== valid commands ===")
for cmd in valid_commands:
    print(f"$ {cmd}")
    print("  tokens:", tokenize(cmd))
    print("  AST:   ", parse(cmd))

print("\n=== invalid commands (should all error) ===")
for cmd in invalid_commands:
    print(f"$ {cmd}")
    try:
        print("  AST:   ", parse(cmd))
    except Exception as e:
        print("  ERROR: ", e)
