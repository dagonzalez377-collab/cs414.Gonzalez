// Recursive descent parser for the shell command language.
//
// Grammar (EBNF):
//
//   command       = ls_cmd | cd_cmd | cat_cmd | print_cmd | exec_cmd
//                 | set_cmd | echo_cmd ;
//
//   ls_cmd        = "ls" , [ path ] ;
//   cd_cmd        = "cd" , [ path ] ;
//   cat_cmd       = "cat" , file_or_var ;
//   print_cmd     = "print" , file_or_var ;
//   exec_cmd      = "exec" , file_or_var ;
//   set_cmd       = "set" , VARIABLE , "=" , expr ;
//   echo_cmd      = "echo" , VARIABLE ;
//
//   path          = "/" , [ folder_list ] | folder_list ;
//   folder_list   = folder_or_var , { "/" , folder_or_var } ;
//   folder_or_var = FOLDERNAME | VARIABLE ;
//   file_or_var   = FILENAME | VARIABLE ;
//
//   expr          = term , { ( "+" | "-" ) , term } ;
//   term          = factor , { ( "*" | "/" ) , factor } ;
//   factor        = VARIABLE | "(" , expr , ")" ;
//
// Start symbol: command
//
// Reserved words: ls, cd, cat, print, exec, set, echo
//
// Lexical rules for tokens with variable content:
//
//   FILENAME   = letter letter letter letter letter letter letter letter "." letter letter letter ;
//   FOLDERNAME = letter , { letter } ;                (* 1 to 8 letters total *)
//   VARIABLE   = "$" , letter , { letter | digit } ;
//
// Note: we only parse expressions here, not evaluate them, so a factor is
// just a variable (or a parenthesized expression) - no number literals,
// since tokenizing digits is out of scope for this assignment.

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "tokenizer.h"

namespace shell {

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message);
};

struct PathNode {
    bool isAbsolute;
    std::vector<std::string> folders;

    std::string toString() const;
};

// A node in an arithmetic expression: either a variable (leaf) or a binary
// operation ('+', '-', '*', '/') on two sub-expressions.
struct ExprNode {
    bool isVariable;
    std::string variable;
    char op = 0;
    std::shared_ptr<ExprNode> left;
    std::shared_ptr<ExprNode> right;

    std::string toString() const;
};

enum class CommandKind { LS, CD, CAT, PRINT, EXEC, SET, ECHO };

struct Command {
    CommandKind kind;
    std::optional<PathNode> path;
    std::string filename;              // also used for the variable name in SET/ECHO
    std::shared_ptr<ExprNode> expr;    // only set for SET

    std::string toString() const;
};

// Symbol table: variable name -> current value. A std::map keeps it sorted
// so printing it is deterministic.
using SymbolTable = std::map<std::string, std::string>;

class Parser {
public:
    Parser(std::vector<Token> tokens, SymbolTable& symbols);

    Command parseCommand();
    const SymbolTable& symbols() const { return symbols_; }

private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;
    SymbolTable& symbols_;

    const Token& peek() const;
    Token advance();
    Token expect(TokenType type);
    void noteVariable(const std::string& name);

    Command lsCmd();
    Command cdCmd();
    Command catCmd();
    Command printCmd();
    Command execCmd();
    Command setCmd();
    Command echoCmd();

    std::optional<PathNode> optionalPath();
    PathNode path();
    std::vector<std::string> folderList();
    Token folderOrVar();
    Token fileOrVar();

    std::shared_ptr<ExprNode> expr();
    std::shared_ptr<ExprNode> term();
    std::shared_ptr<ExprNode> factor();
};

// Tokenize + parse one command, using (and updating) the given symbol table.
Command parse(const std::string& text, SymbolTable& symbols);

// Convenience overload for a one-off parse with no variables to carry over.
Command parse(const std::string& text);

}  // namespace shell
