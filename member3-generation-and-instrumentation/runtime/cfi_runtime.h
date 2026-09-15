// ==============================================================================
// cfi_runtime.h - CFI Runtime Library Header (Member 3)
//
// Purpose:
//   Declares the runtime verification function __cfi_check() invoked before
//   every indirect call in a hardened binary.
// ==============================================================================

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Validates that a dynamic target pointer is an element of the allowed targets array.
/// \param target The dynamically evaluated target address about to be called.
/// \param allowed_targets Pointer to array of allowed function pointers for this signature.
/// \param count The number of allowed function pointers in the array.
///
/// Behavior:
/// - If `target` matches any entry in `allowed_targets`, the function returns normally.
/// - If `target` is not found, an error is printed to stderr and abort() is called.
void __cfi_check(void *target, void **allowed_targets, int32_t count);

#ifdef __cplusplus
}
#endif
