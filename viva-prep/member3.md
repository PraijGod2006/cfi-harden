# Member 3 Viva Preparation: Target Generation, Instrumentation & Runtime

## Role & Responsibilities
- `TargetGenerator`: Address-taken function analysis and signature indexing.
- `InstrumentationEngine`: LLVM IR rewriting, global array emission, and check call insertion.
- `cfi_runtime.c` & `cfi_runtime.h`: Lightweight C runtime check implementation.
- Unit testing in `tests/unit/runtime_test.c`.

---

## Key Concepts & Anticipated Viva Questions

### 1. How does `TargetGenerator` determine the valid target set?
- **Answer**: It iterates through every `Function` in the module and applies two strict criteria:
  1. `!function.isDeclaration()`: The function must be defined within the module (have a body), not merely declared (extern).
  2. `function.hasAddressTaken()`: The function's address must be explicitly taken anywhere in the IR (e.g., stored in a table, passed as argument). If a function's address is never taken, it cannot legitimately be invoked via an indirect pointer.
- Valid targets are indexed by their canonical signature string.

### 2. How does `InstrumentationEngine` generate and reuse target arrays?
- **Answer**: For each unique signature encountered among indirect call sites:
  - If a global array for this signature already exists in `targetGlobals` cache, it is reused.
  - Otherwise, a private constant global variable `@__cfi_targets_N` of type `[count x ptr]` is allocated with `ConstantArray` containing bitcasts of all address-taken target functions.
  - Caching prevents code bloat when multiple indirect calls share the same signature.

### 3. Exactly where and how is the check inserted?
- **Answer**: Using `IRBuilder<> builder(call)`:
  - The check instruction is inserted **immediately preceding** the original indirect call instruction.
  - The original indirect call is left completely intact.
  - Arguments passed to `__cfi_check(void *target, void **allowed_targets, int32_t count)`:
    1. The evaluated dynamic callee target (`builder.CreatePointerCast(call->getCalledOperand(), i8ptr)`).
    2. A GEP pointer to the start of the `@__cfi_targets_N` array (`GEP(..., [0, 0])`).
    3. The number of valid targets (`builder.getInt32(count)`).

### 4. How does `__cfi_check` in the runtime work?
- **Answer**:
  ```c
  void __cfi_check(void *target, void **allowed_targets, int32_t count) {
      for (int32_t i = 0; i < count; i++) {
          if (allowed_targets[i] == target) return;
      }
      fprintf(stderr, "[CFI] control-flow integrity violation: "
                       "indirect call target %p is not in the allowed set\n", target);
      abort();
  }
  ```
  It iterates through the allowed array. If a match is found, it returns immediately and normal execution proceeds. If no match is found, it writes an alert to `stderr` and calls `abort()`.

### 5. What are the known limitations and tradeoffs of this design?
- **Coarse-grained granularity**: Two functions with identical signatures (e.g. `void grant_admin()` and `void log_out()`) share the same target set. An attacker could redirect one to the other if both have address taken. Fine-grained CFI or points-to analysis would be needed to restrict this further.
- **Linear search overhead**: In hot loops, an $O(N)$ scan per call site incurs measurable CPU overhead (as demonstrated by our performance test). In a production compiler, binary search or bitmask sets (like LLVM's built-in Forward-Edge CFI) are used to achieve $O(1)$ verification.
- **Module scope**: Cross-DSO / dynamic library function pointers require Link-Time Optimization (LTO) to be visible to the module pass.

### 6. What real bugs did you fix during implementation?
- Under modern LLVM versions with opaque pointers (LLVM 15+ / LLVM 21), `PointerType::getUnqual(Type*)` is deprecated. Migrated to `PointerType::getUnqual(LLVMContext&)`, resolving compiler deprecation warnings.

