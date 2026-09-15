# CS 414 - Assignment 03, Step 3
# Recursive descent parser for the simple shell command language.
# No parser generator used - just one function per grammar rule below.
#
# Grammar (EBNF):
#
#   command     = ls_cmd | cd_cmd | cat_cmd | print_cmd | exec_cmd ;
#   ls_cmd      = "ls" , [ path ] ;
#   cd_cmd      = "cd" , [ path ] ;
#   cat_cmd     = "cat" , FILENAME ;
#   print_cmd   = "print" , FILENAME ;
#   exec_cmd    = "exec" , FILENAME ;
#   path        = "/" , [ folder_list ] | folder_list ;
#   folder_list = FOLDERNAME , { "/" , FOLDERNAME } ;
#
# Start symbol: command
#
# Reserved words: ls, cd, cat, print, exec (can't also be used as a
# folder/file name - the tokenizer just checks for these first)

from tokenizer import tokenize, LS, CD, CAT, PRINT, EXEC, FILENAME, FOLDERNAME, SLASH, EOF


class ParseError(Exception):
    pass


class Path:
    def __init__(self, absolute, folders):
        self.absolute = absolute
        self.folders = folders

    def __repr__(self):
        kind = "absolute" if self.absolute else "relative"
        return f"Path({kind}, {self.folders})"


class Command:
    def __init__(self, name, path=None, filename=None):
        self.name = name
        self.path = path
        self.filename = filename

    def __repr__(self):
        if self.name in (LS, CD):
            return f"{self.name}(path={self.path})"
        return f"{self.name}(filename={self.filename!r})"


class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0

    def peek(self):
        return self.tokens[self.pos]

    def advance(self):
        tok = self.tokens[self.pos]
        if tok.type != EOF:
            self.pos += 1
        return tok

    def expect(self, type):
        tok = self.peek()
        if tok.type != type:
            raise ParseError(f"expected {type} but got {tok.type} ({tok.value!r})")
        return self.advance()

    # command = ls_cmd | cd_cmd | cat_cmd | print_cmd | exec_cmd ;
    def parse_command(self):
        tok = self.peek()

        if tok.type in (LS, CD):
            self.advance()
            path = self.parse_optional_path()
            self.expect(EOF)
            return Command(tok.type, path=path)

        if tok.type in (CAT, PRINT, EXEC):
            self.advance()
            fname = self.expect(FILENAME)
            self.expect(EOF)
            return Command(tok.type, filename=fname.value)

        raise ParseError(f"expected a command but got {tok.type} ({tok.value!r})")

    # [ path ]
    def parse_optional_path(self):
        if self.peek().type in (SLASH, FOLDERNAME):
            return self.parse_path()
        return None

    # path = "/" , [ folder_list ] | folder_list ;
    def parse_path(self):
        if self.peek().type == SLASH:
            self.advance()
            folders = self.parse_folder_list() if self.peek().type == FOLDERNAME else []
            return Path(True, folders)
        return Path(False, self.parse_folder_list())

    # folder_list = FOLDERNAME , { "/" , FOLDERNAME } ;
    def parse_folder_list(self):
        folders = [self.expect(FOLDERNAME).value]
        while self.peek().type == SLASH:
            self.advance()
            folders.append(self.expect(FOLDERNAME).value)
        return folders


def parse(text):
    tokens = tokenize(text)
    return Parser(tokens).parse_command()


if __name__ == "__main__":
    import sys
    line = " ".join(sys.argv[1:]) or "cd docs/reports"
    try:
        print(parse(line))
    except Exception as e:
        print("ERROR:", e)
