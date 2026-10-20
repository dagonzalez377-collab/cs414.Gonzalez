# Transactional Key-Value Store: C++ vs. OCaml

Two implementations of the same small transactional key-value store, one
in modern C++ and one in OCaml, used to compare how each language
isolates and controls side effects, state, and mutability.

## Repository layout

```
.
├── cpp/                  C++ implementation
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── store.hpp
│   │   └── transaction.hpp
│   ├── src/
│   │   ├── main.cpp
│   │   ├── store.cpp
│   │   └── transaction.cpp
│   └── tests/
│       └── store_tests.cpp
├── ocaml/                OCaml implementation
│   ├── dune-project
│   ├── bin/
│   │   ├── dune
│   │   └── main.ml
│   ├── lib/
│   │   ├── dune
│   │   └── store.ml
│   └── test/
│       ├── dune
│       └── test_store.ml
├── sample_data.txt       Sample file produced by SAVE / consumed by LOAD
├── comparison.md         Required 500-750 word comparison
└── README.md             This file
```

## Commands

Both programs read commands from standard input, one per line:

```
SET key value
GET key
DELETE key
LIST
SAVE filename
LOAD filename
BEGIN
COMMIT
ABORT
QUIT
```

`BEGIN` / `COMMIT` / `ABORT` demonstrate the transaction behavior
required by the assignment. `BEGIN` marks a rollback point; `COMMIT`
keeps all changes made since; `ABORT` discards them, restoring the
store to its state at `BEGIN`.

### Example session

```
> SET name Ada
> SET language OCaml
> GET name
Ada
> LIST
language = OCaml
name = Ada
> DELETE language
> LIST
name = Ada
> BEGIN
> SET x 20
> SET y 30
> ABORT
> LIST
name = Ada
> SAVE data.txt
> QUIT
```

## Building and running: C++

Requires CMake and a C++20 compiler.

```
cd cpp
cmake -S . -B build
cmake --build build
```

Run the program:

```
./build/kvstore
```

Run the tests:

```
./build/store_tests
```

## Building and running: OCaml

Requires OCaml and dune.

```
cd ocaml
dune build
```

Run the program:

```
dune exec bin/main.exe
```

Run the tests:

```
dune test
```

## Sample data file

`sample_data.txt` at the repository root was produced by running `SAVE`
from the C++ program after setting `name`, `language`, and `role`. It
can be loaded by either implementation with `LOAD sample_data.txt` --
the two programs share the same on-disk format (each entry stored as
two lines: the key, then the value), so files are interchangeable
between them.
