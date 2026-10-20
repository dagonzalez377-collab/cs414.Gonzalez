# Assignment 06

## Part 1: C++ BST with std::variant (cpp/)

BST using `std::variant<Empty, unique_ptr<Node>>` instead of a null-pointer
based tree, matching the OCaml `binary_tree` type.

Build/run:
```
cd cpp
cmake -S . -B build
cmake --build build
./build/bst_demo
./build/bst_tests
```

See cpp/COMPARISON.md for the writeup.

## Part 2: OCaml zipper list (ocaml/)

Parametric zipper type with empty, is_empty, move_left, move_right,
push_front, push_back, front, back, focus.

Build/run:
```
cd ocaml
dune build
dune exec bin/main.exe
dune test
```
