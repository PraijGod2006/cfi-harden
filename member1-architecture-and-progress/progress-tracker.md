# Member 1: Progress Tracker

| Module | Responsible Member | Planned Deliverable | Status | Verification Evidence |
|---|---|---|---|---|
| **Shared Interfaces** | Member 1 | Header contracts for all stages (`CFIHardenPass.h`, `IRScanner.h`, etc.) | **Completed** | Full build compiles cleanly; plugin registers with PassBuilder |
| **Pass Orchestration** | Member 1 | Top-level `CFIHardenPass.cpp` pipeline coordinator | **Completed** | Successfully registered under new pass manager via `cfi-harden` |
| **Architecture & Docs** | Member 1 | Architecture write-up, diagrams, milestone plan | **Completed** | Complete in `member1-architecture-and-progress/` |
| **IR Traversal & Scanning** | Member 2 | `IRScanner.cpp` (`Module -> Function -> BB -> Inst`) | **Completed** | Used by detector and verified in `cfi-unit-tests` |
| **Indirect Call Detection** | Member 2 | `IndirectCallDetector.cpp` (identifies `CallBase` without direct callee) | **Completed** | Unit test verifies direct exclusion, indirect detection, `InvokeInst` support |
| **Signature Analysis** | Member 2 | `SignatureAnalyzer.cpp` (canonical signature generation) | **Completed** | Unit test confirms stable keys and variadic argument handling |
| **Target Generation** | Member 3 | `TargetGenerator.cpp` (module-wide address-taken map) | **Completed** | Populates address-taken functions grouped by canonical type |
| **CFI Instrumentation** | Member 3 | `InstrumentationEngine.cpp` (cached target tables & check calls) | **Completed** | Injects `@__cfi_targets_*` globals and `__cfi_check` calls into IR |
| **CFI Runtime Library** | Member 3 | `cfi_runtime.c` & `cfi_runtime.h` | **Completed** | Verified in `cfi-runtime-tests` and integration test suite |
| **Integration Scripts** | Member 4 | `build.sh`, `run_tests.sh` | **Completed** | Scripts build plugin and run positive, negative, boundary, and perf tests |
| **End-to-End Test Suite** | Member 4 | Integration tests (positive, negative, boundary, perf) | **Completed** | Positive passes (0), Negative aborts (134), Boundary warns (0), Perf timed |
