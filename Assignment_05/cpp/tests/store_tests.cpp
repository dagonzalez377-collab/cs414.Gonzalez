// Minimal, dependency-free test runner for Store and Transaction.
// Each test function returns true on success; failures print a message.

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include "store.hpp"
#include "transaction.hpp"

namespace {

int g_failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", message.c_str());
        ++g_failures;
    } else {
        std::printf("PASS: %s\n", message.c_str());
    }
}

void test_set_and_get() {
    Store store;
    store.set("name", "Ada");
    auto value = store.get("name");
    check(value.has_value() && *value == "Ada", "SET then GET returns the value");

    auto missing = store.get("nope");
    check(!missing.has_value(), "GET on a missing key returns nothing");
}

void test_overwrite() {
    Store store;
    store.set("x", "1");
    store.set("x", "2");
    auto value = store.get("x");
    check(value.has_value() && *value == "2", "SET overwrites an existing key");
}

void test_delete() {
    Store store;
    store.set("name", "Ada");
    bool removed = store.remove("name");
    check(removed, "DELETE reports success for an existing key");
    check(!store.get("name").has_value(), "DELETE removes the key");

    bool removedAgain = store.remove("name");
    check(!removedAgain, "DELETE reports failure for a missing key");
}

void test_list_sorted() {
    Store store;
    store.set("language", "OCaml");
    store.set("name", "Ada");
    auto entries = store.list();
    check(entries.size() == 2, "LIST returns all entries");
    check(entries[0].first == "language" && entries[1].first == "name",
          "LIST returns entries in sorted key order");
}

void test_save_and_load_round_trip() {
    Store store;
    store.set("name", "Ada");
    store.set("language", "OCaml");

    const std::string filename = "/tmp/kvstore_cpp_test_data.txt";
    store.save(filename);

    Store loaded;
    loaded.load(filename);

    auto name = loaded.get("name");
    auto language = loaded.get("language");
    check(name.has_value() && *name == "Ada", "LOAD restores a saved value (name)");
    check(language.has_value() && *language == "OCaml",
          "LOAD restores a saved value (language)");
    check(loaded.list().size() == 2, "LOAD restores the correct number of entries");

    std::remove(filename.c_str());
}

void test_transaction_commit() {
    Store store;
    store.set("x", "10");
    {
        Transaction tx(store);
        store.set("x", "20");
        tx.commit();
    }
    auto value = store.get("x");
    check(value.has_value() && *value == "20", "Committed transaction keeps its changes");
}

void test_transaction_abort_restores_state() {
    // This mirrors the assignment's required transaction scenario:
    //   Initial state: x = 10
    //   Begin transaction
    //   SET x 20
    //   SET y 30
    //   Abort transaction
    //   Expected final state: x = 10, y does not exist
    Store store;
    store.set("x", "10");
    {
        Transaction tx(store);
        store.set("x", "20");
        store.set("y", "30");
        // tx goes out of scope here without commit() -> rollback via RAII.
    }
    auto x = store.get("x");
    auto y = store.get("y");
    check(x.has_value() && *x == "10", "Aborted transaction restores x to its original value");
    check(!y.has_value(), "Aborted transaction removes keys introduced during the transaction");
}

}  // namespace

int main() {
    test_set_and_get();
    test_overwrite();
    test_delete();
    test_list_sorted();
    test_save_and_load_round_trip();
    test_transaction_commit();
    test_transaction_abort_restores_state();

    if (g_failures == 0) {
        std::printf("\nAll tests passed.\n");
        return 0;
    }
    std::fprintf(stderr, "\n%d test(s) failed.\n", g_failures);
    return 1;
}
