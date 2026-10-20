# Comparison

The CS 372 style BST uses raw pointers and nullptr to mean "no child" -
you check `if (node == nullptr)` at the start of every recursive
function and the compiler doesn't force you to remember it.

The std::variant version makes "empty" an actual case of the type
instead of a null pointer. std::visit with the Overloaded helper forces
you to handle both Empty and Node, similar to how OCaml's match has to
be exhaustive. It's more code to set up (the Overloaded trick, the
unique_ptr indirection so the type isn't infinitely recursive) but you
can't forget to handle the empty case the way you could forget a null
check in the pointer version.
