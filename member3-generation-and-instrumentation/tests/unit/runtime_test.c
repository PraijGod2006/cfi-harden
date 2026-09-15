// ==============================================================================
// runtime_test.c - Unit Test for Member 3 CFI Runtime Library
//
// Purpose:
//   Directly tests the __cfi_check runtime function in isolation, ensuring that
//   valid function addresses correctly pass without aborting.
// ==============================================================================

#include "cfi_runtime.h"
#include <assert.h>

// A dummy static target function
static void allowed_target(void) {}

int main(void) {
    // Construct a synthetic allowed targets array containing our function
    void *targets[] = {(void *)allowed_target};

    // Invoke __cfi_check: target is in the allowed set, so this must return cleanly
    __cfi_check((void *)allowed_target, targets, 1);

    // Verify target entry was not corrupted
    assert(targets[0] == (void *)allowed_target);

    return 0;
}
