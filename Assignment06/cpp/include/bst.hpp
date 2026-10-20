#pragma once

#include <memory>
#include <variant>
#include <vector>

struct Empty {};
struct Node;

using Tree = std::variant<Empty, std::unique_ptr<Node>>;

struct Node {
    int value;
    Tree left;
    Tree right;
};

inline Tree make_empty() {
    return Tree{Empty{}};
}

Tree insert(Tree tree, int value);

void inorder(const Tree& tree, std::vector<int>& out);
void preorder(const Tree& tree, std::vector<int>& out);
void postorder(const Tree& tree, std::vector<int>& out);

bool contains(const Tree& tree, int value);
