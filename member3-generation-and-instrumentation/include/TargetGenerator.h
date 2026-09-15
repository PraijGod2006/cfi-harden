// ==============================================================================
// TargetGenerator.h - Address-Taken Target Discovery Interface (Member 3)
//
// Purpose:
//   Discovers all functions within the translation unit that have their addresses
//   taken (meaning they could legitimately be called via function pointers) and
//   organizes them into target sets keyed by canonical function signature.
// ==============================================================================

#pragma once

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace cfi {

/// \brief Discovers and categorizes valid target functions across an LLVM Module.
class TargetGenerator {
public:
  /// \brief Map from canonical signature string to list of valid target Function pointers.
  using TargetMap = std::unordered_map<std::string, std::vector<llvm::Function *>>;

  /// \brief Analyzes the module and builds the internal signature-to-targets map.
  /// \param module The module to inspect.
  /// \param analyzer The SignatureAnalyzer used to compute canonical signature keys.
  void build(llvm::Module &module, const class SignatureAnalyzer &analyzer);

  /// \brief Looks up the list of address-taken functions matching a signature.
  /// \param signature Canonical signature string key.
  /// \return Pointer to the vector of matching Functions, or nullptr if none exist.
  const std::vector<llvm::Function *> *targetsFor(const std::string &signature) const;

private:
  TargetMap targets;
};

} // namespace cfi
