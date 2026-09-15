# Member 1 Viva Preparation: Architecture, Pipeline & Pass Orchestration

## Role & Responsibilities
- System Architecture Design: 5-stage forward-edge CFI pipeline.
- Shared Interface Specification: Defining header contracts before parallel development.
- Top-level LLVM Pass Orchestration (`CFIHardenPass.cpp` & `CFIHardenPass.h`).
- Pass Plugin Registration with LLVM New Pass Manager.
- Milestone planning and Review 1/2 tracking.

---

## Key Concepts & Anticipated Viva Questions

### 1. Why is this implemented as an LLVM `ModulePass` rather than a `FunctionPass`?
- **Answer**: Target discovery requires whole-module visibility. To determine the allowed set of targets for any given function signature, we must inspect all functions across the entire translation unit (finding address-taken functions). Furthermore, the global target arrays (`@__cfi_targets_*`) and the declaration of `@__cfi_check` must be inserted at the module level. Therefore, a `ModulePass` (`PreservedAnalyses run(Module &, ModuleAnalysisManager &)`) is strictly required.

### 2. How does the 5-stage pipeline operate?
- **Stage 1 (IR Scanning)**: Traverses all instructions across all basic blocks in all functions.
- **Stage 2 (Indirect Call Detection)**: Identifies indirect `CallBase` candidates (excluding direct calls).
- **Stage 3 (Signature Analysis)**: Derives a canonical type signature string for each indirect call.
- **Stage 4 (Target Generation)**: Collects address-taken functions in the module and indexes them by canonical signature.
- **Stage 5 (Instrumentation Engine)**: Injects global target arrays and pre-call `__cfi_check` invocations.

### 3. How is the plugin registered with the LLVM New Pass Manager?
- LLVM plugins export `llvmGetPassPluginInfo()` with `LLVM_PLUGIN_API_VERSION`.
- Inside the callback, `builder.registerPipelineParsingCallback(...)` checks for pass name `"cfi-harden"` and adds `cfi::CFIHardenPass` to the `ModulePassManager`.
- This enables running the pass via `opt -load-pass-plugin=build/cfi-harden.so -passes=cfi-harden`.

### 4. What analyses does this pass preserve?
- It returns `PreservedAnalyses::none()` because it modifies the control-flow graph (injecting calls and new instructions) and adds global variables to the module.

### 5. Why use an "interfaces-first" build methodology?
- Defining pure interface headers (`IRScanner.h`, `TargetGenerator.h`, etc.) first established a strict contract between members. Member 2 (detection) and Member 3 (instrumentation) could develop their logic in parallel without blocking each other, while Member 1 could orchestrate the top-level pass against these interfaces from day one.
