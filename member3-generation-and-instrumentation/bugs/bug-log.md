# Member 3: Bug Log

| Date | Defect Description | How Found | Root Cause | Fix Applied | Status |
|---|---|---|---|---|---|
| 2026-09-15 | Deprecated `PointerType::getUnqual(Type*)` in `InstrumentationEngine.cpp` | Running `cmake --build build` under LLVM 21 | Typed pointers (`i8*`) are deprecated in modern LLVM in favor of opaque pointers (`ptr`). Using `PointerType::getUnqual(i8)` produced deprecation warnings pending API removal | Replaced with `PointerType::getUnqual(context)` across variable type definitions and function declarations | Closed (Verified: compiles cleanly without warnings) |
