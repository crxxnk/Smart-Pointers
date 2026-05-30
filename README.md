# Smart-Pointers

A lightweight, standard-compliant, header-only implementation of custom smart pointers (`unique_ptr` and `shared_ptr`) written in modern C++ (C++17/20). This project focuses on first-principles systems programming, featuring optimized single-allocation mechanics for `make_shared`, strict memory alignment handling, type-erased control blocks, and custom allocator support via template allocator rebinding.

## 🚀 Key Features

* **`unique_ptr<T>`**: Zero-overhead RAII wrapper for exclusive resource ownership, featuring support for array specializations (`T[]`) and customizable deleters.
* **Polymorphic Control Blocks**: A robust `shared_ptr` architecture that utilizes dynamic type erasure to isolate pointer metadata from standard operations.
* **Optimized `make_shared`**: Implements single-allocation memory packing to guarantee spatial cache locality and eliminate redundant kernel heap requests.
* **Allocator Awareness**: Full integration with `std::allocator_traits` to redirect internal control block metadata mapping into custom memory pools or arenas.
* **Thread-Safe Reference Counting**: Powered by lock-free, atomic reference tracking (`std::atomic_size_t`) to eliminate race conditions across multi-threaded environments.

---

## 📐 Memory Architecture Topologies

The library implements two distinct memory layouts underneath the hood based on construction semantics to balance flexibility and performance:

### 1. Split Allocation Layout (`shared_ptr<T>(new T)`)
When a raw heap pointer is passed explicitly, ownership is transferred to an internal `object_owner` control block. This creates two separate allocations on the heap:
* **The Object Block**: A standalone chunk of memory allocated by the user via standard `new` containing the managed data.
* **The Control Block**: A separate chunk of memory allocated internally by the smart pointer constructor containing the `strong_ref` and `weak_ref` atomic counters.
* **The Parent Instance**: The `shared_ptr` on the stack frame maintains two distinct pointers: one to the object block and one to the control block.

### 2. Contiguous Combined Memory Layout (`make_shared<T>`)
To maximize CPU cache locality and drop down to a single OS kernel allocation request, `make_shared` packs the control block headers and the object storage into one unified block of memory:
* **Single Allocation**: A single chunk of bytes is requested from the heap, large enough to fit both the control block metadata and the object itself back-to-back.
* **Header and Alignment Padding**: The control block tracking headers live at the absolute beginning of the block, followed immediately by any necessary compiler-calculated alignment padding.
* **Object Placement**: The actual managed object storage lives sequentially inside the exact same block, eliminating pointer-chasing overhead across scattered heap memory regions.

---

## 🛠️ Low-Level Implementations Highlighted

### Type Erasure via Core Abstract Interfaces
To hide customized behavior from the user-facing `shared_ptr<T>` class template, the management layer relies on an abstract base interface called `control_block`. This base class handles generic counter increments and decrements, while specialized lifecycles are resolved dynamically through derived polymorphic structs:
* `object_owner`: Manages standard raw heap allocations and invokes default or custom scalar deleters.
* `object_owner_alloc`: Captures custom memory pool structures and redirects metadata cleanup back through custom allocator traits instead of the global heap.
* `make_shared_control_block`: Embedded directly within single-allocation routines, utilizing strict internal `alignas(T)` storage buffers to preserve native object alignment properties.

### Allocator Rebinding Mechanics
The implementation utilizes `std::allocator_traits::rebind_alloc` to dynamically morph incoming allocators typed for generic objects into engine blocks capable of stamping out exact control block structures:
```cpp
using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<object_owner_alloc<Ptr, Deleter, Allocator>>;
```
Strict Resource Protection & Strong Exceptions
Constructors taking raw pointers are hardened against memory leaks. If allocating the underlying tracking control block fails `cpp(std::bad_alloc)`, the implementation automatically intercepts the failure, destroys the raw pointer to prevent dynamic leaks, and rethrows to ensure incomplete objects never enter the program state:

```cpp
try {
    cb = new object_owner<Ptr>(_Ptr, std::default_delete<Ptr>{});
} catch (...) { 
    delete _Ptr; // Prevent resource leaks if control block allocation fails
    throw;       // Abort construction completely
}
```
💻 Usage Examples
1. Exclusive Ownership with Unique Pointers
```cpp
#include "unique_ptr.hpp"
#include <iostream>

struct Vector3D {
    float x, y, z;
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
};

int main() {
    // Basic instantiation
    auto u1 = iosp::make_unique<Vector3D>(1.0f, 2.0f, 3.0f);
    std::cout << "Vector: " << u1->x << ", " << u1->y << "\n";

    // Safe ownership transfer via move semantics
    iosp::unique_ptr<Vector3D> u2 = std::move(u1); 
    assert(u1.get() == nullptr); 
}
```
2. Shared Ownership & Cache-Optimized Creation

```cpp
#include "shared_ptr.hpp"
#include <cassert>

int main() {
    // Single allocation construction
    auto s1 = iosp::make_shared<int>(42);
    assert(s1.use_count() == 1);

    {
        iosp::shared_ptr<int> s2 = s1; // Implicit atomic counter increment
        assert(s1.use_count() == 2);
        assert(*s2 == 42);
    } // s2 leaves scope; counter automatically decrements

    assert(s1.use_count() == 1);
} // Last counter dropped; memory block freed cleanly
```
🔧 Building and Requirements
Compiler: C++17 compliant compiler or newer (GCC 9+, Clang 10+, MSVC 2019+).

Configuration: This is a header-only library. Simply copy the iosp files directly into your project's include path.

# Compilation flags recommendation for validating memory layout integrity
``` Bash
g++ -std=c++17 -Wall -Wextra main.cpp -o pointer_test
```

*Note: This repository represents a deep-dive into first-principles C++ systems programming. While the entire codebase is written manually, the technical write-up and memory architecture diagrams were structured with the assistance of AI to ensure optimal clarity and readability.*