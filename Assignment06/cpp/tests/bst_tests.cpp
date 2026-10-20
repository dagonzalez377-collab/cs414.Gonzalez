#include <cstdio>

#include "bst.hpp"

int failures = 0;

void check(bool condition, const char* message) {
    if (condition) {
        std::printf("PASS: %s\n", message);
    } else {
        std::fprintf(stderr, "FAIL: %s\n", message);
        failures++;
    }
}

int main() {
    Tree tree = make_empty();
    for (int v : {5, 3, 8, 1, 4, 7, 9}) {
        tree = insert(std::move(tree), v);
    }

    std::vector<int> in, pre, post;
    inorder(tree, in);
    preorder(tree, pre);
    postorder(tree, post);

    check(in == std::vector<int>({1, 3, 4, 5, 7, 8, 9}), "inorder is sorted");
    check(pre == std::vector<int>({5, 3, 1, 4, 8, 7, 9}), "preorder order");
    check(post == std::vector<int>({1, 4, 3, 7, 9, 8, 5}), "postorder order");
    check(contains(tree, 7), "contains finds 7");
    check(!contains(tree, 6), "contains does not find 6");

    if (failures == 0) {
        std::printf("\nAll tests passed.\n");
        return 0;
    }
    std::fprintf(stderr, "\n%d test(s) failed.\n", failures);
    return 1;
}
