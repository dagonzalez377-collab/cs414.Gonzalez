#include <iostream>

#include "bst.hpp"

namespace {

void print(const std::string& label, const std::vector<int>& values) {
    std::cout << label << ":";
    for (int v : values) {
        std::cout << ' ' << v;
    }
    std::cout << '\n';
}

}  // namespace

int main() {
    Tree tree = make_empty();
    for (int value : {5, 3, 8, 1, 4, 7, 9}) {
        tree = insert(std::move(tree), value);
    }

    std::vector<int> in, pre, post;
    inorder(tree, in);
    preorder(tree, pre);
    postorder(tree, post);

    print("inorder", in);
    print("preorder", pre);
    print("postorder", post);

    std::cout << "contains(7): " << std::boolalpha << contains(tree, 7) << '\n';
    std::cout << "contains(6): " << std::boolalpha << contains(tree, 6) << '\n';

    return 0;
}
