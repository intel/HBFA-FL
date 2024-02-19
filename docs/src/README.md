# Introduction

The Host-based Firmware Analyzer - Fuzzing Lite (HBFA-FL) is based off of forking the original HBFA from the [edk2-staging repository](https://github.com/tianocore/edk2-staging/tree/HBFA). The original release of HBFA is described in the white-paper: ["Using Host-based Firmware Analysis to Improve Platform Resiliency"](https://www.intel.com/content/dam/develop/external/us/en/documents/intel-usinghbfatoimproveplatformresiliency-820238.pdf). The original release authors were Brian Richardson, Chris Wu, Jiewen Yao, and Vincent J. Zimmer.

The goal for HBFA-FL is to update the original codebase, enhancing some features and removing/streamlining some others, with a focus on functionality for Linux environments. To that end, HBFA-FL removes much of the original HBFA code base, including support for Windows and unit-testing; we retain support for fuzzing with AFL and LibFuzzer. This repository is effectively a fork from the original version of HBFA at this commit : [ead8f4f8eefdb2eb762184e1a5809c43d6908952](https://github.com/tianocore/edk2-staging/commit/ead8f4f8eefdb2eb762184e1a5809c43d6908952)

## Version

The most recent release for HBFA-FL is version 0.11. The release is an initial release with a focus on functionality modern Linux, for AFL and LibFuzzer fuzzing support, and enabling additional sanitizers and coverage options for HBFA.

### Supported Features

- Command-line interfaces to support building and running of fuzzing
- Execute fuzzing harnesses from common fuzzing frameworks (e.g. AFL and LibFuzzer)
- Incorporation of Address Sanitizer (ASAN), Memory Sanitizer (MSAN), and the Undefined Behavior Sanitizer (UBSAN)
- Generating code coverage reports (GCOV/LCOV and Profraw)

### Features Not Fully Implemented

- Support for symbolic execution with KLEE/STP with HBFA-FL has not yet been fully updated.

### Features Not Supported from the Original HBFA

- Windows support has been removed
- GUI support has been removed

## Getting Started

To get started using HBFA-FL, we recommend taking a look at the documentation, starting [here](./SUMMARY.md).

Return to [Summary](./SUMMARY.md) | Next [&gt;&gt;](./setup/README.md)
