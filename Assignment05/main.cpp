// Driver / test harness for the tokenizer and parser.
//
// Usage:
//   ./shell_parser                    runs the test files and prints pass/fail
//   ./shell_parser set $x = $a        tokenizes and parses one command

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"
#include "tokenizer.h"

using namespace shell;

static std::string tokensToString(const std::vector<Token>& tokens) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i) oss << ", ";
        oss << "Token(" << tokenTypeName(tokens[i].type) << ", '" << tokens[i].lexeme
            << "', pos=" << tokens[i].pos << ")";
    }
    oss << "]";
    return oss.str();
}

static std::string symbolsToString(const SymbolTable& symbols) {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& entry : symbols) {
        if (!first) oss << ", ";
        first = false;
        oss << entry.first << ": '" << entry.second << "'";
    }
    oss << "}";
    return oss.str();
}

static bool runOne(const std::string& command, SymbolTable& symbols, bool showTokens = true) {
    std::cout << "$ " << command << "\n";
    try {
        std::vector<Token> tokens = tokenize(command);
        if (showTokens) {
            std::cout << "  tokens: " << tokensToString(tokens) << "\n";
        }
        Parser parser(tokens, symbols);
        Command ast = parser.parseCommand();
        std::cout << "  AST:    " << ast.toString() << "\n";
        std::cout << "  vars:   " << symbolsToString(symbols) << "\n";
        return true;
    } catch (const LexError& e) {
        std::cout << "  ERROR:  " << e.what() << "\n";
        return false;
    } catch (const ParseError& e) {
        std::cout << "  ERROR:  " << e.what() << "\n";
        return false;
    }
}

static std::vector<std::string> loadCommands(const std::string& path, bool addEmpty) {
    std::vector<std::string> commands;
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string trimmed = line.substr(start, end - start + 1);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        commands.push_back(trimmed);
    }
    if (addEmpty) commands.push_back("");
    return commands;
}

// persistSymbols=true means all commands in the file share one symbol
// table, like a real shell session where "set" sticks around.
static int runSuite(const std::string& path, bool expectSuccess, bool addEmpty, bool persistSymbols) {
    std::cout << "\n=== " << path << " (expect " << (expectSuccess ? "success" : "error")
               << ") ===\n";
    int passed = 0, failed = 0;
    SymbolTable sharedSymbols;
    for (const auto& cmd : loadCommands(path, addEmpty)) {
        SymbolTable localSymbols;
        SymbolTable& symbols = persistSymbols ? sharedSymbols : localSymbols;
        bool ok = runOne(cmd, symbols);
        if (ok == expectSuccess) {
            ++passed;
        } else {
            ++failed;
            std::cout << "  ^^^ UNEXPECTED RESULT\n";
        }
    }
    std::cout << "--- " << path << ": " << passed << " passed, " << failed << " failed ---\n";
    return failed;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        std::string command;
        for (int i = 1; i < argc; ++i) {
            if (i > 1) command += " ";
            command += argv[i];
        }
        SymbolTable symbols;
        runOne(command, symbols);
        return 0;
    }

    int totalFailed = 0;
    totalFailed += runSuite("tests/valid_commands.txt", true, false, false);
    totalFailed += runSuite("tests/invalid_commands.txt", false, true, false);
    totalFailed += runSuite("tests/variable_commands.txt", true, false, true);
    totalFailed += runSuite("tests/invalid_variable_commands.txt", false, false, false);

    std::cout << "\n";
    if (totalFailed == 0) {
        std::cout << "ALL TESTS PASSED\n";
        return 0;
    }
    std::cout << totalFailed << " TEST(S) FAILED\n";
    return 1;
}
