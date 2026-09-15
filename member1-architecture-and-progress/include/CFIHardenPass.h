// ==============================================================================
// CFIHardenPass.h - Top-level LLVM Pass Interface (Member 1)
//
// Purpose:
//   Defines the CFIHardenPass class using the LLVM New Pass Manager infrastructure.
//   It inherits from PassInfoMixin<CFIHardenPass> to support standard pass registration,
//   pipeline composition, and analysis management.
// ==============================================================================

#pragma once

#include "llvm/IR/PassManager.h"

namespace cfi {

/// \brief Top-level pass class that orchestrates the coarse-grained forward-edge
/// Control-Flow Integrity (CFI) hardening pipeline.
///
/// This pass operates at the Module level (ModulePass) because:
/// 1. Target discovery must inspect all functions across the entire translation unit.
/// 2. Injected global target tables (@__cfi_targets_*) and the __cfi_check
///    function declaration are placed into module scope.
class CFIHardenPass : public llvm::PassInfoMixin<CFIHardenPass> {
public:
  /// \brief Main entry point executed by the LLVM Pass Manager on a Module.
  /// \param module The LLVM Module to inspect and transform.
  /// \param manager The ModuleAnalysisManager providing access to analysis passes.
  /// \return PreservedAnalyses indicating which existing analyses remain valid.
  llvm::PreservedAnalyses run(llvm::Module &module,
                              llvm::ModuleAnalysisManager &manager);
};

} // namespace cfi
