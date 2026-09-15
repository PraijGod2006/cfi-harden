# cfi-harden

`cfi-harden` is an out-of-tree LLVM new-pass-manager plugin implementing coarse-grained, forward-edge, type-based CFI for indirect calls. It builds a module-local set of address-taken functions for each canonical LLVM function signature, then inserts a call to a small C runtime immediately before each indirect call.

## Toolchain

The supported baseline is LLVM/Clang 18 or newer on Linux x86-64. The project does not hardcode an LLVM version: CMake uses `find_package(LLVM CONFIG REQUIRED)` and reports the discovered version. LLVM 18 is the recommended starting point because it provides the required pass-plugin API and opaque-pointer IR used by this implementation.

Install a matching toolchain on Debian/Ubuntu with:

```sh
sudo apt update
sudo apt install clang-18 llvm-18 llvm-18-dev lld-18 cmake ninja-build build-essential
```

On Windows, use WSL2 for the documented Linux pipeline, or install LLVM from the official Windows installer and provide `LLVM_DIR` to CMake. The integration scripts target POSIX shells.

## Build

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The plugin is `build/cfi-harden.so` and is loaded with:

```sh
clang -emit-llvm -S -c input.c -o input.ll
opt -load-pass-plugin=build/cfi-harden.so -passes=cfi-harden -S input.ll -o hardened.ll
```

Virtual/vtable dispatch is out of scope. It is treated as an indirect call only when LLVM exposes it as a `CallBase`; this project does not add class-hierarchy or vtable policy.

## Team structure

The four member folders separate architecture, detection, generation, and integration. See `docs/commit-instructions.txt` for the real per-member workflow and `docs/completion-plan.md` for known follow-up work.
