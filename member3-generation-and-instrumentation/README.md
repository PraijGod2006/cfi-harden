# Member 3: Generation and Instrumentation

`TargetGenerator` creates a module-local map from canonical `FunctionType` text to non-declaration functions whose addresses are taken. `InstrumentationEngine` caches one private global array per signature, casts each valid function to `i8*`, and inserts `__cfi_check(actual, allowed, count)` immediately before the original call.

## Transformation pseudocode

```text
for each indirect call:
  signature = canonical(call.function_type)
  targets = address_taken_functions[signature]
  if targets is empty: warn and skip
  global = cached_target_array(signature) or create it
  actual = bitcast(call.called_operand to i8*)
  first = gep(global, 0, 0)
  insert __cfi_check(actual, first, target_count) before call
```

A representative input call is `call i32 %fp(i32 %x)`. The inserted IR is equivalent to:

```llvm
%target = bitcast ptr %fp to ptr
%targets = getelementptr inbounds [2 x ptr], ptr @__cfi_targets_0, i32 0, i32 0
call void @__cfi_check(ptr %target, ptr %targets, i32 2)
%result = call i32 %fp(i32 %x)
```

The exact emitted text must be captured from `opt` during local verification; no execution trace is claimed here because the toolchain is not installed yet.

## Known limitations

This is coarse-grained matching: functions with the same signature are interchangeable even when their application-level roles differ. Empty target sets are warned about and skipped. Virtual/vtable policy and cross-module target sets are out of scope. The runtime uses a linear scan, so large target sets and hot loops incur overhead.
