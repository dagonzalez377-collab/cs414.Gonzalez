# Assignment 05 - Extended Parser + Contours (C++)

C++17, no third-party libraries.

## Question 1 & 2: Variables in the shell parser

Extends the Assignment 03 tokenizer/parser with shell variables.

New syntax:
- `set $VAR = EXPR` - defines/updates a variable. `EXPR` is an arithmetic
  expression (`+ - * /`, with parentheses) whose terminals are other
  variables - we're only parsing expressions here, not evaluating them,
  so there's no digit tokenizing (per the assignment note).
- `echo $VAR` - references a variable.
- A variable (`$name`) can be used anywhere `ls`/`cd` takes a path or
  `cat`/`print`/`exec` takes a filename, and as a single component inside
  a path (e.g. `cd /home/$user/docs`).

The full EBNF grammar is in the comment block at the top of `parser.h`.

Symbol table: a `std::map<std::string, std::string>` (`SymbolTable` in
parser.h) mapping variable name to current value. A variable is added the
first time it's seen (parsed with an empty value), and `set` updates its
entry. Since we don't evaluate expressions, the "value" stored for `set`
is just the parsed expression's string form. The table is printed after
every command that parses successfully.

Files:
- `tokenizer.h` / `tokenizer.cpp` - lexer, now with `$variable`, `=`,
  `+`, `-`, `*`, `(`, `)`, and the `set`/`echo` keywords.
- `parser.h` / `parser.cpp` - recursive descent parser + the EBNF grammar
  (as comments) + the `SymbolTable` type.
- `main.cpp` - runs the test files in `tests/` and prints results,
  including the symbol table after each command.
- `tests/valid_commands.txt` / `tests/invalid_commands.txt` - the
  original Assignment 03 test cases (still valid under this grammar).
- `tests/variable_commands.txt` - one shell "session" exercising `set`,
  `echo`, arithmetic expressions, and variables used as paths/filenames.
- `tests/invalid_variable_commands.txt` - bad variable syntax that should
  fail to parse.

Build and run:
```
make
./shell_parser
```

Run a single command:
```
./shell_parser set $x = $a
```

## Question 3: Scope / contours

Written answers (contour diagrams for the four C++ snippets) are in
`documents/question3_contours.pdf`, as the assignment asks for a
separate document rather than code.

## Assumptions

- `/` is used for the path separator, root folder, and division - same
  character, disambiguated by grammar position (paths vs. expressions).
- Filenames are 8 letters + `.` + 3 letters (DOS 8.3), folder names are
  1-8 letters, matching Assignment 03.
- A variable name is `$` followed by a letter, then any letters/digits.
- Digit literals are not tokenized in expressions (per the assignment's
  note that this can be ignored) - a factor is a variable or a
  parenthesized expression.
