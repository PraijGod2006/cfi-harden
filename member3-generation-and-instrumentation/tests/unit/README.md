# Generation and instrumentation unit tests

`runtime_test.c` verifies that an allowed target returns normally from `__cfi_check`. The integration negative case verifies the abort path after the full LLVM transformation is available.
