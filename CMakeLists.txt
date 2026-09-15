# ==============================================================================
# CMakeLists.txt - Build Configuration for cfi-harden LLVM Pass & Test Suite
#
# This project builds:
# 1. cfi-harden.so      - The out-of-tree LLVM pass plugin (New Pass Manager).
# 2. cfi-unit-tests     - C++ unit test runner for detection & signature logic.
# 3. cfi-runtime-tests  - C test runner for the CFI security runtime library.
# ==============================================================================

cmake_minimum_required(VERSION 3.20)
project(cfi_harden LANGUAGES C CXX)

# Require C++17 (standard for modern LLVM passes) and Position-Independent Code (-fPIC)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------------------------
# Locate LLVM Package Configuration
# find_package(LLVM REQUIRED CONFIG) discovers the installed LLVM toolchain
# without hardcoding paths or versions, reading llvm-config metadata directly.
# ------------------------------------------------------------------------------
find_package(LLVM CONFIG REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# Import LLVM include directories and definitions
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# ------------------------------------------------------------------------------
# Target 1: cfi-harden (Pass Plugin)
# Compiled from all member modules:
# - Member 1: Orchestrator (CFIHardenPass.cpp)
# - Member 2: Detection (IRScanner, IndirectCallDetector, SignatureAnalyzer)
# - Member 3: Generation & Instrumentation (TargetGenerator, InstrumentationEngine)
# ------------------------------------------------------------------------------
add_llvm_pass_plugin(cfi-harden
  member1-architecture-and-progress/lib/CFIHardenPass.cpp
  member2-detection-and-analysis/lib/IRScanner.cpp
  member2-detection-and-analysis/lib/IndirectCallDetector.cpp
  member2-detection-and-analysis/lib/SignatureAnalyzer.cpp
  member3-generation-and-instrumentation/lib/TargetGenerator.cpp
  member3-generation-and-instrumentation/lib/InstrumentationEngine.cpp
)

# Map LLVM core components needed for linking
llvm_map_components_to_libnames(LLVM_LIBS Core Support Passes TransformUtils AsmParser)

target_include_directories(cfi-harden PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/member1-architecture-and-progress/include
  ${CMAKE_CURRENT_SOURCE_DIR}/member2-detection-and-analysis/include
  ${CMAKE_CURRENT_SOURCE_DIR}/member3-generation-and-instrumentation/include
)

# Set library output name without 'lib' prefix (e.g. produces cfi-harden.so)
set_target_properties(cfi-harden PROPERTIES PREFIX "")

# ------------------------------------------------------------------------------
# Target 2: cfi-unit-tests (Unit Tests for Member 2)
# Tests indirect-call detection, direct call filtering, variadics, and invokes.
# ------------------------------------------------------------------------------
add_executable(cfi-unit-tests
  member2-detection-and-analysis/lib/IRScanner.cpp
  member2-detection-and-analysis/lib/IndirectCallDetector.cpp
  member2-detection-and-analysis/lib/SignatureAnalyzer.cpp
  member2-detection-and-analysis/tests/unit/detector_tests.cpp
)
target_link_libraries(cfi-unit-tests PRIVATE ${LLVM_LIBS})
target_include_directories(cfi-unit-tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/member2-detection-and-analysis/include)

# ------------------------------------------------------------------------------
# Target 3: cfi-runtime-tests (Unit Tests for Member 3 Runtime)
# Tests that the C runtime library properly accepts valid function targets.
# ------------------------------------------------------------------------------
add_executable(cfi-runtime-tests
  member3-generation-and-instrumentation/runtime/cfi_runtime.c
  member3-generation-and-instrumentation/tests/unit/runtime_test.c)
target_include_directories(cfi-runtime-tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/member3-generation-and-instrumentation/runtime)

# ------------------------------------------------------------------------------
# Installation Rules
# ------------------------------------------------------------------------------
install(TARGETS cfi-harden DESTINATION lib)
install(FILES member3-generation-and-instrumentation/runtime/cfi_runtime.h DESTINATION include)

