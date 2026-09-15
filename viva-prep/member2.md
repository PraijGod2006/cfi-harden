# Member 2 Viva Preparation: Detection & Signature Analysis

## Role & Responsibilities
- `IRScanner`: Instruction visitor pattern decoupling IR traversal from inspection.
- `IndirectCallDetector`: Accurate classification of indirect call sites.
- `SignatureAnalyzer`: Canonical string representation of LLVM function types.
- Unit testing framework in `tests/unit/detector_tests.cpp`.

---

## Key Concepts & Anticipated Viva Questions

### 1. Why check `llvm::CallBase` rather than `llvm::CallInst`?
- **Answer**: In LLVM, function calls can be initiated via standard calls (`CallInst`) or exception-handling calls (`InvokeInst`). `CallBase` is the unified parent class introduced in LLVM to abstract both. Using `CallBase` guarantees that indirect calls in C++ code using exceptions (e.g., `invoke` instructions with normal and unwind destination blocks) are hardened and not bypassed.

### 2. How do you distinguish between direct and indirect calls?
- **Answer**: By inspecting `call->getCalledFunction()`.
  - If `getCalledFunction() != nullptr`, the callee is a statically resolved function (e.g. `@printf` or `@helper`), which is immune to runtime function pointer tampering.
  - If `getCalledFunction() == nullptr`, the callee operand is dynamically evaluated at runtime (e.g. loaded from a struct, vtable, or pointer argument). This represents an indirect call and must be protected.

### 3. How do you construct a canonical signature key?
- **Answer**: `FunctionType` contains:
  1. Return type (`type->getReturnType()`)
  2. Number and ordered types of formal parameters (`type->params()`)
  3. Variadic flag (`type->isVarArg()`)
- We print the function type into a stream and append `;vararg=<0|1>`. This forms a deterministic string key (e.g., `i32 (i32);vararg=0`) used as a dictionary key to pair caller sites with callee functions.

### 4. What unit tests did you implement and why?
- **Direct call exclusion**: Ensures normal static calls are not mistakenly instrumented (which would cause unnecessary overhead and potential false positives).
- **Indirect call detection**: Confirms that standard function pointer calls are identified and assigned proper signatures.
- **Variadic boundary**: Tests `void (...)` to verify `vararg=1` handling.
- **Invoke instruction boundary**: Verifies `InvokeInst` detection so C++ exception boundaries are not left vulnerable.

### 5. What real bugs did you encounter during development?
- Missing `#include "llvm/AsmParser/Parser.h"` in `detector_tests.cpp`, which caused `parseAssemblyString` to fail during compilation. Resolved by including the appropriate LLVM AsmParser header.
