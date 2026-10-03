# CS 414 - Assignment 03, Step 2
# Tokenizer for the simple shell command language.
#
# Assumption: the handout's separator/root character didn't copy over
# cleanly (showed up blank), so this just uses '/' for both.

import re

# token types (just using strings instead of an enum, keeps it simple)
LS = "LS"
CD = "CD"
CAT = "CAT"
PRINT = "PRINT"
EXEC = "EXEC"
FILENAME = "FILENAME"
FOLDERNAME = "FOLDERNAME"
SLASH = "SLASH"
EOF = "EOF"

KEYWORDS = {
    "ls": LS,
    "cd": CD,
    "cat": CAT,
    "print": PRINT,
    "exec": EXEC,
}

WORD_RE = re.compile(r"[A-Za-z.]+")
FILENAME_RE = re.compile(r"[A-Za-z]{8}\.[A-Za-z]{3}$")


class Token:
    def __init__(self, type, value):
        self.type = type
        self.value = value

    def __repr__(self):
        return f"({self.type}, {self.value!r})"


def tokenize(text):
    tokens = []
    i = 0
    n = len(text)

    while i < n:
        ch = text[i]

        if ch.isspace():
            i += 1
            continue

        if ch == "/":
            tokens.append(Token(SLASH, "/"))
            i += 1
            continue

        if ch.isalpha():
            match = WORD_RE.match(text, i)
            word = match.group(0)
            i = match.end()

            if word in KEYWORDS:
                tokens.append(Token(KEYWORDS[word], word))
            elif FILENAME_RE.match(word):
                tokens.append(Token(FILENAME, word))
            elif "." in word:
                raise Exception(f"'{word}' is not a valid 8.3 filename")
            elif len(word) <= 8:
                tokens.append(Token(FOLDERNAME, word))
            else:
                raise Exception(f"'{word}' is longer than 8 characters")
            continue

        raise Exception(f"unexpected character '{ch}'")

    tokens.append(Token(EOF, ""))
    return tokens


if __name__ == "__main__":
    import sys
    line = " ".join(sys.argv[1:]) or "cd docs/reports"
    for t in tokenize(line):
        print(t)
