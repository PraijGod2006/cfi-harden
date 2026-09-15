// ==============================================================================
// InstrumentationEngine.h - LLVM IR Rewriting & Check Injection Interface (Member 3)
//
// Purpose:
//   Mutates the LLVM IR of the target module:
//   1. Emits constant global pointer arrays holding allowed target functions.
//   2. Declares the external __cfi_check runtime function.
//   3. Inserts runtime validation calls immediately preceding each indirect call.
// ==============================================================================

#pragma once

#include "llvm/IR/Module.h"
#include "llvm/IR/InstrTypes.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace cfi {

class TargetGenerator;
class SignatureAnalyzer;

/// \brief Inserts forward-edge CFI runtime validation checks into the LLVM IR.
class InstrumentationEngine {
public:
  /// \brief Instruments all identified indirect call sites in the module.
  /// \param module The module being modified.
  /// \param calls List of indirect call sites identified by Member 2's detector.
  /// \param generator The populated target generator from Member 3.
  /// \param analyzer The signature analyzer from Member 2.
  void instrument(llvm::Module &module,
                  const std::vector<llvm::CallBase *> &calls,
                  const TargetGenerator &generator,
                  const SignatureAnalyzer &analyzer);

private:
  /// \brief Cache of emitted global arrays indexed by canonical signature key.
  /// Reusing globals prevents emitting duplicate target arrays when multiple
  /// call sites share the exact same signature.
  std::unordered_map<std::string, llvm::GlobalVariable *> targetGlobals;
};

} // namespace cfi
