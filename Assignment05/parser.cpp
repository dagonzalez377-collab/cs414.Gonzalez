#include "parser.h"

#include <sstream>

namespace shell {

ParseError::ParseError(const std::string& message) : std::runtime_error(message) {}

std::string PathNode::toString() const {
    std::ostringstream oss;
    oss << "Path(" << (isAbsolute ? "absolute" : "relative") << ", [";
    for (size_t i = 0; i < folders.size(); ++i) {
        if (i) oss << ", ";
        oss << folders[i];
    }
    oss << "])";
    return oss.str();
}

std::string ExprNode::toString() const {
    if (isVariable) return variable;
    std::ostringstream oss;
    oss << "(" << left->toString() << " " << op << " " << right->toString() << ")";
    return oss.str();
}

static const char* kindName(CommandKind k) {
    switch (k) {
        case CommandKind::LS: return "Ls";
        case CommandKind::CD: return "Cd";
        case CommandKind::CAT: return "Cat";
        case CommandKind::PRINT: return "Print";
        case CommandKind::EXEC: return "Exec";
        case CommandKind::SET: return "Set";
        case CommandKind::ECHO: return "Echo";
    }
    return "?";
}

std::string Command::toString() const {
    std::ostringstream oss;
    oss << kindName(kind) << "(";
    if (kind == CommandKind::LS || kind == CommandKind::CD) {
        oss << "path=" << (path.has_value() ? path->toString() : "None");
    } else if (kind == CommandKind::SET) {
        oss << "var='" << filename << "', expr=" << (expr ? expr->toString() : "?");
    } else if (kind == CommandKind::ECHO) {
        oss << "var='" << filename << "'";
    } else {
        oss << "filename='" << filename << "'";
    }
    oss << ")";
    return oss.str();
}

Parser::Parser(std::vector<Token> tokens, SymbolTable& symbols)
    : tokens_(std::move(tokens)), symbols_(symbols) {}

const Token& Parser::peek() const { return tokens_[pos_]; }

Token Parser::advance() {
    Token tok = tokens_[pos_];
    if (tok.type != TokenType::END) {
        ++pos_;
    }
    return tok;
}

Token Parser::expect(TokenType type) {
    const Token& tok = peek();
    if (tok.type != type) {
        std::ostringstream oss;
        oss << "expected " << tokenTypeName(type) << " but found " << tokenTypeName(tok.type)
            << " ('" << tok.lexeme << "') at position " << tok.pos;
        throw ParseError(oss.str());
    }
    return advance();
}

// Variables are "defined upon first use" - just make sure it has an entry.
void Parser::noteVariable(const std::string& name) {
    symbols_.emplace(name, "");
}

// command = ls_cmd | cd_cmd | cat_cmd | print_cmd | exec_cmd | set_cmd | echo_cmd ;
Command Parser::parseCommand() {
    const Token& tok = peek();
    Command node;

    switch (tok.type) {
        case TokenType::LS: node = lsCmd(); break;
        case TokenType::CD: node = cdCmd(); break;
        case TokenType::CAT: node = catCmd(); break;
        case TokenType::PRINT: node = printCmd(); break;
        case TokenType::EXEC: node = execCmd(); break;
        case TokenType::SET: node = setCmd(); break;
        case TokenType::ECHO: node = echoCmd(); break;
        default: {
            std::ostringstream oss;
            oss << "expected a command (ls, cd, cat, print, exec, set, echo) but found "
                << tokenTypeName(tok.type) << " ('" << tok.lexeme << "') at position " << tok.pos;
            throw ParseError(oss.str());
        }
    }

    // A well-formed command consumes every token; anything left over is an error.
    expect(TokenType::END);
    return node;
}

Command Parser::lsCmd() {
    expect(TokenType::LS);
    Command c;
    c.kind = CommandKind::LS;
    c.path = optionalPath();
    return c;
}

Command Parser::cdCmd() {
    expect(TokenType::CD);
    Command c;
    c.kind = CommandKind::CD;
    c.path = optionalPath();
    return c;
}

Command Parser::catCmd() {
    expect(TokenType::CAT);
    Token f = fileOrVar();
    Command c;
    c.kind = CommandKind::CAT;
    c.filename = f.lexeme;
    return c;
}

Command Parser::printCmd() {
    expect(TokenType::PRINT);
    Token f = fileOrVar();
    Command c;
    c.kind = CommandKind::PRINT;
    c.filename = f.lexeme;
    return c;
}

Command Parser::execCmd() {
    expect(TokenType::EXEC);
    Token f = fileOrVar();
    Command c;
    c.kind = CommandKind::EXEC;
    c.filename = f.lexeme;
    return c;
}

// set_cmd = "set" , VARIABLE , "=" , expr ;
Command Parser::setCmd() {
    expect(TokenType::SET);
    Token v = expect(TokenType::VARIABLE);
    noteVariable(v.lexeme);
    expect(TokenType::ASSIGN);
    auto e = expr();

    // We're only parsing, not evaluating, so we store the expression's
    // text as the variable's "current value" (values are strings anyway).
    symbols_[v.lexeme] = e->toString();

    Command c;
    c.kind = CommandKind::SET;
    c.filename = v.lexeme;
    c.expr = e;
    return c;
}

// echo_cmd = "echo" , VARIABLE ;
Command Parser::echoCmd() {
    expect(TokenType::ECHO);
    Token v = expect(TokenType::VARIABLE);
    noteVariable(v.lexeme);
    Command c;
    c.kind = CommandKind::ECHO;
    c.filename = v.lexeme;
    return c;
}

// [ path ]
std::optional<PathNode> Parser::optionalPath() {
    TokenType t = peek().type;
    if (t == TokenType::SLASH || t == TokenType::FOLDERNAME || t == TokenType::VARIABLE) {
        return path();
    }
    return std::nullopt;
}

// path = "/" , [ folder_list ] | folder_list ;
PathNode Parser::path() {
    if (peek().type == TokenType::SLASH) {
        advance();
        std::vector<std::string> folders;
        if (peek().type == TokenType::FOLDERNAME || peek().type == TokenType::VARIABLE) {
            folders = folderList();
        }
        return PathNode{true, folders};
    }
    return PathNode{false, folderList()};
}

// folder_list = folder_or_var , { "/" , folder_or_var } ;
std::vector<std::string> Parser::folderList() {
    std::vector<std::string> folders;
    folders.push_back(folderOrVar().lexeme);
    while (peek().type == TokenType::SLASH) {
        advance();
        folders.push_back(folderOrVar().lexeme);
    }
    return folders;
}

// folder_or_var = FOLDERNAME | VARIABLE ;
Token Parser::folderOrVar() {
    if (peek().type == TokenType::VARIABLE) {
        Token t = advance();
        noteVariable(t.lexeme);
        return t;
    }
    return expect(TokenType::FOLDERNAME);
}

// file_or_var = FILENAME | VARIABLE ;
Token Parser::fileOrVar() {
    if (peek().type == TokenType::VARIABLE) {
        Token t = advance();
        noteVariable(t.lexeme);
        return t;
    }
    return expect(TokenType::FILENAME);
}

// expr = term , { ( "+" | "-" ) , term } ;
std::shared_ptr<ExprNode> Parser::expr() {
    auto node = term();
    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token opTok = advance();
        auto parent = std::make_shared<ExprNode>();
        parent->isVariable = false;
        parent->op = (opTok.type == TokenType::PLUS) ? '+' : '-';
        parent->left = node;
        parent->right = term();
        node = parent;
    }
    return node;
}

// term = factor , { ( "*" | "/" ) , factor } ;
std::shared_ptr<ExprNode> Parser::term() {
    auto node = factor();
    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        Token opTok = advance();
        auto parent = std::make_shared<ExprNode>();
        parent->isVariable = false;
        parent->op = (opTok.type == TokenType::STAR) ? '*' : '/';
        parent->left = node;
        parent->right = factor();
        node = parent;
    }
    return node;
}

// factor = VARIABLE | "(" , expr , ")" ;
std::shared_ptr<ExprNode> Parser::factor() {
    if (peek().type == TokenType::VARIABLE) {
        Token v = advance();
        noteVariable(v.lexeme);
        auto node = std::make_shared<ExprNode>();
        node->isVariable = true;
        node->variable = v.lexeme;
        return node;
    }
    if (peek().type == TokenType::LPAREN) {
        advance();
        auto node = expr();
        expect(TokenType::RPAREN);
        return node;
    }

    std::ostringstream oss;
    oss << "expected a variable or '(' but found " << tokenTypeName(peek().type)
        << " ('" << peek().lexeme << "') at position " << peek().pos;
    throw ParseError(oss.str());
}

Command parse(const std::string& text, SymbolTable& symbols) {
    std::vector<Token> tokens = tokenize(text);
    Parser p(std::move(tokens), symbols);
    return p.parseCommand();
}

Command parse(const std::string& text) {
    SymbolTable symbols;
    return parse(text, symbols);
}

}  // namespace shell
