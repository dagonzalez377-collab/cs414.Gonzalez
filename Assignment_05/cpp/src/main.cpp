#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "store.hpp"
#include "transaction.hpp"

namespace {

// Splits a command line into its first token (the command) and the
// remainder of the line (trimmed), which holds the arguments.
std::pair<std::string, std::string> splitCommand(const std::string& line) {
    std::istringstream iss(line);
    std::string command;
    iss >> command;
    std::string rest;
    std::getline(iss, rest);
    // Trim a single leading space left over from the extraction above.
    size_t start = rest.find_first_not_of(' ');
    if (start == std::string::npos) {
        return {command, ""};
    }
    return {command, rest.substr(start)};
}

// Splits "key value..." into a key and the remaining value (which may
// itself contain spaces).
std::pair<std::string, std::string> splitKeyValue(const std::string& args) {
    size_t spacePos = args.find(' ');
    if (spacePos == std::string::npos) {
        return {args, ""};
    }
    return {args.substr(0, spacePos), args.substr(spacePos + 1)};
}

}  // namespace

int main() {
    Store store;
    std::optional<Transaction> activeTransaction;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        auto [command, args] = splitCommand(line);

        if (command == "SET") {
            auto [key, value] = splitKeyValue(args);
            if (key.empty()) {
                std::cout << "ERROR: SET requires a key and a value\n";
                continue;
            }
            store.set(key, value);
        } else if (command == "GET") {
            const std::string& key = args;
            auto value = store.get(key);
            if (value.has_value()) {
                std::cout << *value << '\n';
            } else {
                std::cout << "(nil)\n";
            }
        } else if (command == "DELETE") {
            const std::string& key = args;
            store.remove(key);
        } else if (command == "LIST") {
            for (const auto& [key, value] : store.list()) {
                std::cout << key << " = " << value << '\n';
            }
        } else if (command == "SAVE") {
            try {
                store.save(args);
            } catch (const std::exception& e) {
                std::cout << "ERROR: " << e.what() << '\n';
            }
        } else if (command == "LOAD") {
            try {
                store.load(args);
            } catch (const std::exception& e) {
                std::cout << "ERROR: " << e.what() << '\n';
            }
        } else if (command == "BEGIN") {
            if (activeTransaction.has_value()) {
                std::cout << "ERROR: a transaction is already in progress\n";
            } else {
                activeTransaction.emplace(store);
            }
        } else if (command == "COMMIT") {
            if (!activeTransaction.has_value()) {
                std::cout << "ERROR: no transaction in progress\n";
            } else {
                activeTransaction->commit();
                activeTransaction.reset();
            }
        } else if (command == "ABORT") {
            if (!activeTransaction.has_value()) {
                std::cout << "ERROR: no transaction in progress\n";
            } else {
                // Resetting without commit() runs ~Transaction(), which
                // restores the store to its pre-transaction state.
                activeTransaction.reset();
            }
        } else if (command == "QUIT") {
            break;
        } else {
            std::cout << "ERROR: unknown command '" << command << "'\n";
        }
    }

    return 0;
}
