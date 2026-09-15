# Member 4 Viva Preparation: Integration, Testing & Performance Evaluation

## Role & Responsibilities
- Automated Build Pipeline (`scripts/build.sh`): Building CMake pass plugin and runtime objects.
- Test Execution Harness (`scripts/run_tests.sh`): Lowering C to LLVM IR, applying `opt -passes=cfi-harden`, code generation via `llc`, and final linking.
- Integration Test Suite: Positive, negative, boundary, and performance benchmarks.
- Test Reporting & Verification: Capturing logs, status codes, and overhead metrics in `test-report.md` and `issue-tracker.md`.

---

## Key Concepts & Anticipated Viva Questions

### 1. Explain the complete compilation and hardening pipeline.
- **Answer**:
  ```text
  C Source
     │  clang -emit-llvm -S
     ▼
  Un-hardened LLVM IR (.ll)
     │  opt -load-pass-plugin=build/cfi-harden.so -passes=cfi-harden -S
     ▼
  Hardened LLVM IR (.ll)
     │  llc -filetype=obj
     ▼
  Object File (.o)
     │  clang -no-pie <test.o> <cfi_runtime.o> -o <test.hardened>
     ▼
  Hardened Executable
  ```
  This cleanly decouples frontend parsing from IR transformation and backend machine code generation.

### 2. Walk through each integration test case and explain what was verified.
- **Positive Test (`callback_dispatch.c`)**:
  - Contains array of function pointers (`add_one`, `add_two`) with matching signature `int(int)`.
  - Result: Exit code 0, prints `total=23`. Proves valid indirect calls run undisturbed.
- **Negative Test (`signature_mismatch.c`)**:
  - Forces a function pointer of type `void(int)` to be called as `void(void)` via casting.
  - Result: Terminated by `abort()`, exit code 134. Outputs `[CFI] control-flow integrity violation: indirect call target 0x... is not in the allowed set`. Proves invalid forward edges are blocked before execution.
- **Boundary Test (`empty_target_set.c`)**:
  - Introduces an indirect call to an unmatched signature (`long double(long double, long double, long double)`).
  - Result: The compiler emits diagnostic warning `cfi-harden: warning: no valid targets for indirect call with signature ...` and skips instrumentation without crashing. Binary exits 0. Proves compiler robustness against degenerate call sites.
- **Performance Test (`indirect_call_loop.c`)**:
  - Executes 10,000,000 indirect calls in a tight loop.
  - Result: Baseline execution takes ~0.0101s, hardened execution takes ~0.0286s, measuring an overhead of +183.79% for an empty body call.

### 3. Why did the negative test exit with code 134?
- **Answer**: In Linux/POSIX environments, `abort()` raises the `SIGABRT` signal (signal 6). When a process is terminated by an uncaught signal $N$, its process exit status is reported by the shell as $128 + N$. Therefore, $128 + 6 = 134$.

### 4. What integration issues were diagnosed and resolved during testing?
- **Missing `zstd`**: Fixed by installing `libzstd-dev`.
- **Parser header**: Fixed `parseAssemblyString` in Member 2's unit test.
- **Opaque pointer deprecation**: Modernized pointer types in Member 3's engine.
- **Boundary test callsite**: Added an actual indirect call to `empty_target_set.c` so the pass's target-lookup warning branch was legitimately exercised.
- **Automated overhead measurement**: Added baseline timing and automated percentage calculation to `run_tests.sh`.
