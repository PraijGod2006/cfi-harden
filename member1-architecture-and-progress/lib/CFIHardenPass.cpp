// ==============================================================================
// CFIHardenPass.cpp - Top-level LLVM Pass Orchestrator & Plugin Entry Point
//
// Member 1 Responsibility:
//   Coordinates the 5-stage CFI hardening pipeline by integrating components
//   from Member 2 (Detection & Signature Analysis) and Member 3 (Target Generation
//   & Instrumentation). Also registers the plugin with LLVM's PassBuilder.
// ==============================================================================

#include "CFIHardenPass.h"
#include "IRScanner.h"
#include "IndirectCallDetector.h"
#include "SignatureAnalyzer.h"
#include "TargetGenerator.h"
#include "InstrumentationEngine.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace cfi {

/// \brief Executes the full CFI transformation on the given Module.
///
/// Execution Sequence:
/// 1. Initialize SignatureAnalyzer to format canonical type signatures.
/// 2. Initialize IndirectCallDetector to identify all indirect call sites.
/// 3. Run TargetGenerator::build() to discover all address-taken functions in the
///    module and map them by canonical signature.
/// 4. Run InstrumentationEngine::instrument() to insert runtime checks before
///    each detected indirect call site using the generated target tables.
PreservedAnalyses CFIHardenPass::run(Module &module, ModuleAnalysisManager &) {
  // Member 2: Tooling for extracting canonical function signatures
  SignatureAnalyzer analyzer;

  // Member 2: Detector for identifying CallBase instructions without static callees
  IndirectCallDetector detector;

  // Member 3: Discover address-taken functions across the module
  TargetGenerator generator;
  generator.build(module, analyzer);

  // Member 3: Inject __cfi_check calls and target globals into the module IR
  InstrumentationEngine instrumenter;
  instrumenter.instrument(module, detector.find(module), generator, analyzer);

  // Since we modified control flow and inserted global variables and instructions,
  // no previous analysis (e.g. DominatorTree, LoopInfo) is guaranteed to be valid.
  return PreservedAnalyses::none();
}

} // namespace cfi

// ==============================================================================
// LLVM Pass Plugin Registration
//
// This boilerplate exposes the pass plugin to `opt` and `clang` when loaded
// dynamically via:
//   opt -load-pass-plugin=build/cfi-harden.so -passes=cfi-harden ...
// ==============================================================================
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,  // Expected LLVM Plugin API version
    "cfi-harden",             // Plugin name
    "0.1",                    // Plugin version
    [](PassBuilder &builder) {
      // Register a pipeline parsing callback so `opt -passes=cfi-harden` is recognized
      builder.registerPipelineParsingCallback(
        [](StringRef name, ModulePassManager &modulePassManager,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (name == "cfi-harden") {
            // Add our pass to the module-level pass manager pipeline
            modulePassManager.addPass(cfi::CFIHardenPass());
            return true;
          }
          return false;
        });
    }
  };
}
