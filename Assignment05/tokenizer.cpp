#include "tokenizer.h"

#include <cctype>

namespace shell {

static const char SEPARATOR = '/';
static const size_t MAX_FOLDER_LEN = 8;

const char* tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::LS: return "LS";
        case TokenType::CD: return "CD";
        case TokenType::CAT: return "CAT";
        case TokenType::PRINT: return "PRINT";
        case TokenType::EXEC: return "EXEC";
        case TokenType::SET: return "SET";
        case TokenType::ECHO: return "ECHO";
        case TokenType::FILENAME: return "FILENAME";
        case TokenType::FOLDERNAME: return "FOLDERNAME";
        case TokenType::VARIABLE: return "VARIABLE";
        case TokenType::SLASH: return "SLASH";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::END: return "EOF";
    }
    return "?";
}

LexError::LexError(const std::string& message, size_t p)
    : std::runtime_error("Lexical error at position " + std::to_string(p) + ": " + message),
      pos(p) {}

Tokenizer::Tokenizer(const std::string& text) : text_(text) {}

void Tokenizer::skipWhitespace() {
    while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
        ++pos_;
    }
}

bool Tokenizer::isKeyword(const std::string& word, TokenType& outType) {
    if (word == "ls") { outType = TokenType::LS; return true; }
    if (word == "cd") { outType = TokenType::CD; return true; }
    if (word == "cat") { outType = TokenType::CAT; return true; }
    if (word == "print") { outType = TokenType::PRINT; return true; }
    if (word == "exec") { outType = TokenType::EXEC; return true; }
    if (word == "set") { outType = TokenType::SET; return true; }
    if (word == "echo") { outType = TokenType::ECHO; return true; }
    return false;
}

// 8 letters, a '.', then 3 letters (DOS 8.3 name).
bool Tokenizer::isFilename(const std::string& word) {
    if (word.size() != 12) return false;
    for (size_t i = 0; i < 8; ++i) {
        if (!std::isalpha(static_cast<unsigned char>(word[i]))) return false;
    }
    if (word[8] != '.') return false;
    for (size_t i = 9; i < 12; ++i) {
        if (!std::isalpha(static_cast<unsigned char>(word[i]))) return false;
    }
    return true;
}

Token Tokenizer::readWord() {
    size_t start = pos_;
    while (pos_ < text_.size() &&
           (std::isalpha(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '.')) {
        ++pos_;
    }
    std::string word = text_.substr(start, pos_ - start);

    TokenType kw;
    if (isKeyword(word, kw)) {
        return Token{kw, word, start};
    }

    if (isFilename(word)) {
        return Token{TokenType::FILENAME, word, start};
    }

    if (word.find('.') != std::string::npos) {
        throw LexError(
            "'" + word + "' is not a valid 8.3 filename "
            "(need exactly 8 letters, a '.', then exactly 3 letters)",
            start);
    }

    if (word.size() <= MAX_FOLDER_LEN) {
        return Token{TokenType::FOLDERNAME, word, start};
    }

    throw LexError(
        "'" + word + "' exceeds the " + std::to_string(MAX_FOLDER_LEN) +
            "-character folder name limit",
        start);
}

// A variable is '$' followed by a letter, then any run of letters/digits.
Token Tokenizer::readVariable() {
    size_t start = pos_;
    ++pos_;  // consume '$'

    if (pos_ >= text_.size() || !std::isalpha(static_cast<unsigned char>(text_[pos_]))) {
        throw LexError("'$' must be followed by a letter", start);
    }

    while (pos_ < text_.size() && std::isalnum(static_cast<unsigned char>(text_[pos_]))) {
        ++pos_;
    }

    return Token{TokenType::VARIABLE, text_.substr(start, pos_ - start), start};
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        skipWhitespace();

        if (pos_ >= text_.size()) {
            tokens.push_back(Token{TokenType::END, "", pos_});
            return tokens;
        }

        char ch = text_[pos_];
        size_t start = pos_;

        if (ch == SEPARATOR) {
            tokens.push_back(Token{TokenType::SLASH, std::string(1, ch), start});
            ++pos_;
            continue;
        }
        if (ch == '=') {
            tokens.push_back(Token{TokenType::ASSIGN, "=", start});
            ++pos_;
            continue;
        }
        if (ch == '+') {
            tokens.push_back(Token{TokenType::PLUS, "+", start});
            ++pos_;
            continue;
        }
        if (ch == '-') {
            tokens.push_back(Token{TokenType::MINUS, "-", start});
            ++pos_;
            continue;
        }
        if (ch == '*') {
            tokens.push_back(Token{TokenType::STAR, "*", start});
            ++pos_;
            continue;
        }
        if (ch == '(') {
            tokens.push_back(Token{TokenType::LPAREN, "(", start});
            ++pos_;
            continue;
        }
        if (ch == ')') {
            tokens.push_back(Token{TokenType::RPAREN, ")", start});
            ++pos_;
            continue;
        }
        if (ch == '$') {
            tokens.push_back(readVariable());
            continue;
        }
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            tokens.push_back(readWord());
            continue;
        }

        throw LexError(std::string("unexpected character '") + ch + "'", pos_);
    }
}

std::vector<Token> tokenize(const std::string& text) {
    Tokenizer t(text);
    return t.tokenize();
}

}  // namespace shell
