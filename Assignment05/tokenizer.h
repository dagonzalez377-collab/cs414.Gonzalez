// Tokenizer for the shell command language.
// Assumption: using '/' for the path separator and root folder.

#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace shell {

enum class TokenType {
    LS,
    CD,
    CAT,
    PRINT,
    EXEC,
    SET,
    ECHO,
    FILENAME,
    FOLDERNAME,
    VARIABLE,
    SLASH,
    ASSIGN,
    PLUS,
    MINUS,
    STAR,
    LPAREN,
    RPAREN,
    END
};

const char* tokenTypeName(TokenType t);

struct Token {
    TokenType type;
    std::string lexeme;
    size_t pos;
};

class LexError : public std::runtime_error {
public:
    LexError(const std::string& message, size_t pos);
    size_t pos;
};

class Tokenizer {
public:
    explicit Tokenizer(const std::string& text);

    std::vector<Token> tokenize();

private:
    std::string text_;
    size_t pos_ = 0;

    void skipWhitespace();
    Token readWord();
    Token readVariable();

    static bool isKeyword(const std::string& word, TokenType& outType);
    static bool isFilename(const std::string& word);
};

std::vector<Token> tokenize(const std::string& text);

}  // namespace shell
