# Assignment 03 - Tokenizer + Parser

Python 3, nothing extra to install.

Files:
- `tokenizer.py` - turns a command string into a list of tokens
- `parser.py` - recursive descent parser, builds a simple AST (grammar is
  in the comment at the top of the file)
- `test_parser.py` - runs a few example commands through both and prints
  the results

Run it:
```
python3 test_parser.py
```

Assumptions:
- Used `/` for the path separator and the root folder, since that
  character didn't come through clearly on the assignment page.
- Filenames are 8 letters + `.` + 3 letters (DOS 8.3 format), folder
  names are up to 8 letters.
- If a folder/file were literally named `ls`, `cd`, `cat`, `print`, or
  `exec`, it'd get read as the command keyword instead - there's no way
  to tell those apart in this grammar.
- `ls`/`cd` with nothing after them parse with `path=None`. Per the
  spec that means "current folder" for `ls` and "root folder" for `cd`,
  but that's more of a runtime detail than something the parser needs to
  fill in.
