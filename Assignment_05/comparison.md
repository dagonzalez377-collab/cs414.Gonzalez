# Comparison: C++ vs. OCaml Implementations

**1. Which operations in the C++ program have side effects?**
`Store::save` and `Store::load` are the only operations with external
side effects: they open files via `std::ofstream`/`std::ifstream` and
read or write disk state. `Store::set`, `Store::remove`, `Store::get`,
and `Store::list` mutate or read only the object's own private
`std::map` member, which is a contained, local effect rather than one
reaching the outside world. `Transaction`'s constructor and destructor
also have a side effect in the broader sense: they mutate the `Store`
object they're attached to (taking a backup, or restoring one).

**2. Which operations in the OCaml program have side effects?**
`Store.save` and `Store.load` again, for the same reason: they call
`open_out`/`open_in` and perform file I/O. Everything else in
`lib/store.ml` -- `set`, `delete`, `get`, `list`, and `transaction` --
is a pure function with no observable effect beyond its return value.

**3. Which OCaml functions could reasonably be considered pure?**
`set`, `delete`, `get`, `list`, and `transaction` are all pure: each
is a mathematical function from its inputs to a result, with no
mutation, I/O, or other observable effect. Calling `set` twice with
the same arguments always returns an equal store; nothing about the
original store or the environment changes.

**4. Where does mutable state exist in the C++ implementation?**
Inside `Store`, the `std::map<std::string, std::string> data_` member
is mutated in place by `set` and `remove`. `Transaction::backup_` is
also mutable in the sense that it's a `Store` object constructed once
and never changed after that -- it exists purely to be swapped back
in in `~Transaction`.

**5. Where does mutable state exist in the OCaml implementation?**
Almost nowhere in the key-value logic itself -- `StringMap.t` values
are immutable, and `set`/`delete` build new maps rather than editing
existing ones. The only place anything is mutated is the REPL loop in
`bin/main.ml`, where `loop` is called again and again with a new
`state` record each time; there's no mutable variable, just a fresh
binding carried forward through recursive calls.

**6. How does RAII help control or isolate side effects in C++?**
`Transaction`'s backup and restoration are tied to object lifetime:
construction snapshots the store, and the destructor -- which always
runs, even if an exception is thrown -- restores it unless `commit()`
was called. There's no way to "forget" to roll back a transaction;
the compiler guarantees `~Transaction` runs. The same idea protects
file handles: `std::ofstream`/`std::ifstream` close themselves when
they go out of scope, so `save`/`load` can't leak an open file handle.

**7. How does immutability help control side effects in OCaml?**
Because `set` and `delete` return new maps instead of editing the old
one, a reference to a store can never become unexpectedly stale or
change underneath you. This makes rollback close to free: keeping a
`store option` for the pre-transaction value is enough, since that old
value is guaranteed to still describe the old state no matter what
happens to the "current" store afterward.

**8. How does rollback work differently in the two implementations?**
In C++, rollback is an active operation: `~Transaction` copies the
contents of `backup_` back into the live `Store`, physically
overwriting the mutated map. In OCaml, rollback is passive: the
"original" store was never touched by the operations performed during
the transaction, so "rolling back" just means discarding the modified
store value and resuming use of the original binding -- no copying or
restoring is required.

**9. Why can the OCaml implementation retain an earlier state without
explicitly copying the entire map?**
`Map.Make` builds a persistent, balanced tree. `add` and `remove`
share the unmodified parts of the old tree with the new one, allocating
only the handful of nodes along the path that changed. The old root
is still a complete, valid map on its own -- nothing was mutated to
produce the new one -- so holding onto `original` costs only a
reference, not a deep copy.

**10. Which programming model made transaction rollback easier to
express? Explain the mechanism rather than simply stating a
preference.**
OCaml's model made rollback easier to express, because "easier" here
means fewer things that have to be gotten right. The C++ version
needs a correctly-written destructor, an explicitly managed backup
copy, and a `committed_` flag -- correctness depends on the programmer
getting that bookkeeping exactly right, even though RAII at least
guarantees the destructor will run. The OCaml version needs none of
that: because `set` and `delete` can never mutate an existing store,
simply not using the modified value is the entire rollback. The
mechanism isn't "restore state," it's "nothing was ever lost in the
first place" -- rollback in OCaml is a consequence of the data
structure's immutability rather than a procedure that has to be
implemented and verified.
