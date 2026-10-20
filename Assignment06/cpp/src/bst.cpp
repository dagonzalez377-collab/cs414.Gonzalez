#include "bst.hpp"

Tree insert(Tree tree, int value) {
    if (std::holds_alternative<Empty>(tree)) {
        auto node = std::make_unique<Node>();
        node->value = value;
        node->left = Empty{};
        node->right = Empty{};
        return node;
    }

    auto& node = std::get<std::unique_ptr<Node>>(tree);
    if (value < node->value) {
        node->left = insert(std::move(node->left), value);
    } else if (value > node->value) {
        node->right = insert(std::move(node->right), value);
    }
    return std::move(node);
}

bool contains(const Tree& tree, int value) {
    if (std::holds_alternative<Empty>(tree)) {
        return false;
    }

    const auto& node = std::get<std::unique_ptr<Node>>(tree);
    if (value == node->value) return true;
    if (value < node->value) return contains(node->left, value);
    return contains(node->right, value);
}

void inorder(const Tree& tree, std::vector<int>& out) {
    if (std::holds_alternative<Empty>(tree)) return;

    const auto& node = std::get<std::unique_ptr<Node>>(tree);
    inorder(node->left, out);
    out.push_back(node->value);
    inorder(node->right, out);
}

void preorder(const Tree& tree, std::vector<int>& out) {
    if (std::holds_alternative<Empty>(tree)) return;

    const auto& node = std::get<std::unique_ptr<Node>>(tree);
    out.push_back(node->value);
    preorder(node->left, out);
    preorder(node->right, out);
}

void postorder(const Tree& tree, std::vector<int>& out) {
    if (std::holds_alternative<Empty>(tree)) return;

    const auto& node = std::get<std::unique_ptr<Node>>(tree);
    postorder(node->left, out);
    postorder(node->right, out);
    out.push_back(node->value);
}
