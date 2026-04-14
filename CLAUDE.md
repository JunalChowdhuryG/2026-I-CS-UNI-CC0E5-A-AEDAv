# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
make          # compile → produces ./main executable
./main        # run (demo output goes to temp.txt)
make clean    # remove object files and executable
```

Compiler: `g++`, standard: C++23 (`-std=c++2b`), flags: `-Wall -g -pthread`.

There is no test framework — demos in `containers/vector.cpp` (`DemoVector()`) serve as integration tests and write output to `temp.txt`.

## Architecture

This is a C++ data structures teaching library. New containers follow the same layered pattern:

```
util.h               ← generic algorithms: Print, ForEach (3 overloads)
general_iterator.h   ← CRTP base iterator<Container, IteratorBase>
containers/X.h       ← XNode<T> + X<T> container + concrete iterator
types.h              ← platform type aliases: Type, T1, Ref
```

### Key design decisions

- **`VectorNode<T>`** pairs every stored value with a `Ref` (`long`) reference field. Elements are always `(data, ref)` pairs — `push_back` requires both arguments; there is no single-argument push.
- **`general_iterator<Container, IteratorBase>`** uses CRTP so derived iterators are returned correctly from `operator=` and comparisons. Concrete iterators inherit this and only add `operator++`.
- **`Vector<T>::value_type`** is `T`; `operator*` on the iterator returns `T&` via `VectorNode::getDataRef()`. The iterator traverses the `VectorNode[]` array by raw pointer increment.
- **`ForEach`** has three free-function overloads in `util.h`: `(container, func)`, `(begin, end, func, ...args)`, and `(container, func, ...args)`. `Vector::ForEach` member delegates to the iterator-range overload.
- `resize()` doubles capacity when ≥ 10, otherwise adds 10.

### Adding a new container

1. Create `containers/X.h` defining `XNode<T>`, `X<T>` (with `value_type` and `Node` typedefs, `begin()`/`end()`), and a concrete iterator inheriting `general_iterator<X<T>, x_forward_iterator<X<T>>>`.
2. If the container needs a demo `.cpp`, add it to `SRCS` in the Makefile.
3. Call the demo from `main.cpp`.

### Current branch context

Branch `15-BackwardIter` — adding a backward (reverse) iterator. `general_iterator.h` is the file under active modification. The pattern mirrors `vector_forward_iterator` but `operator++` should decrement the internal pointer.
