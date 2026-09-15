# CFI-Harden Architecture and Integration Diagram

## End-to-End Pipeline

The diagram below illustrates the 5-stage compilation and runtime pipeline, showing how source code is lowered, transformed by each member's module within the LLVM new pass manager, and linked against the CFI runtime library.

```mermaid
flowchart TD
    subgraph Frontend["Compilation Frontend"]
        Src["C / C++ Source"] --> Clang["clang -emit-llvm -S"]
        Clang --> InputIR["Initial LLVM IR (.ll)"]
    end

    subgraph Pass["CFI-Harden Pass (Member 1 Orchestrator)"]
        InputIR --> PM["LLVM New Pass Manager (opt)"]
        PM --> Orch["CFIHardenPass::run() (Member 1)"]

        subgraph Analysis["Member 2: Detection & Analysis"]
            Scanner["IRScanner"] --> Detector["IndirectCallDetector"]
            Detector --> Analyzer["SignatureAnalyzer"]
        end

        subgraph Codegen["Member 3: Target Generation & Instrumentation"]
            Analyzer --> Gen["TargetGenerator"]
            Gen --> Inst["InstrumentationEngine"]
        end

        Orch --> Scanner
        Orch --> Gen
        Orch --> Inst
    end

    subgraph Backend["Code Generation & Linking"]
        Inst --> HardenedIR["Hardened LLVM IR"]
        HardenedIR --> LLC["llc (Object Code Generation)"]
        LLC --> Obj["Hardened Object (.o)"]
        Obj --> Linker["clang / ld Linker"]
        Runtime["cfi_runtime.c (Member 3)"] --> RuntimeObj["cfi_runtime.o"]
        RuntimeObj --> Linker
        Linker --> Executable["Hardened Executable"]
    end

    subgraph RuntimeExecution["Runtime Verification"]
        Executable --> Check["__cfi_check(target, allowed_set, count)"]
        Check -->|Target in Set| Normal["Normal Execution Flow"]
        Check -->|Target NOT in Set| Abort["[CFI] Violation Error & abort()"]
    end
```

## Module Responsibility Matrix

| Stage | Responsible Member | Component | Responsibility |
|---|---|---|---|
| **1. Orchestration** | Member 1 | `CFIHardenPass` | Traverses module, registers pass with LLVM PassBuilder (`cfi-harden`), orchestrates stages. |
| **2. Scanning** | Member 2 | `IRScanner` | Walks `Module -> Function -> BasicBlock -> Instruction`. |
| **3. Detection** | Member 2 | `IndirectCallDetector` | Identifies `llvm::CallBase` without direct function target (covers both calls and invokes). |
| **4. Signature Analysis** | Member 2 | `SignatureAnalyzer` | Generates canonical signature strings from `FunctionType` (return, args, vararg). |
| **5. Target Generation** | Member 3 | `TargetGenerator` | Gathers address-taken module functions grouped by canonical signature. |
| **6. Instrumentation** | Member 3 | `InstrumentationEngine` | Emits cached global target arrays and inserts `__cfi_check` calls before indirect call sites. |
| **7. Runtime** | Member 3 | `cfi_runtime.c` | Performs runtime membership checks in constant global tables and aborts on violation. |
| **8. Integration & Testing** | Member 4 | `scripts/`, `tests/` | Builds the pass, compiles test cases, verifies security, boundary, and performance overhead. |

