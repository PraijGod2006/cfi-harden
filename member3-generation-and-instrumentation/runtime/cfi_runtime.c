// ==============================================================================
// cfi_runtime.c - CFI Runtime Security Enforcement Implementation (Member 3)
//
// Enforcement Mechanics:
// 1. Performs a linear scan over the allowed targets table.
// 2. If the dynamic pointer matches any entry in `allowed_targets`, control flow
//    is legitimate according to the compiler's coarse-grained signature analysis.
// 3. If no match is found, control-flow hijacking or severe type mismatch has
//    occurred. We log a violation message to stderr with the offending address
//    and immediately terminate execution via abort() to prevent arbitrary execution.
// ==============================================================================

#include "cfi_runtime.h"
#include <stdio.h>
#include <stdlib.h>

/// \brief Verifies target validity against the allowed table.
/// \param target The target address about to be called.
/// \param allowed_targets Pointer to array of allowed function pointers.
/// \param count Number of elements in the allowed array.
void __cfi_check(void *target, void **allowed_targets, int32_t count) {
    // 1. Linear scan: check if target address exists in the pre-computed set
    for (int32_t i = 0; i < count; i++) {
        if (allowed_targets[i] == target) {
            // Target is authorized: return normally to allow the indirect call to execute
            return;
        }
    }

    // 2. Violation detected: print forensic details to standard error
    fprintf(stderr, "[CFI] control-flow integrity violation: "
                    "indirect call target %p is not in the allowed set\n", target);

    // 3. Terminate immediately by raising SIGABRT (exit status 134 in bash)
    abort();
}
