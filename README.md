# Host-based Firmware Analyzer - Fuzzing Lite (HBFA-FL)

[![OpenSSF Scorecard](https://api.securityscorecards.dev/project/github.com/intel/hbfa-fl/badge)](https://securityscorecards.dev/viewer/?uri=github.com%2Fintel%2FHBFA-FL)

The the Host-based Firmware Analysis - Fuzzing Lite (HBFA-FL) is based off of forking the original HBFA from the [edk2-staging repository](https://github.com/tianocore/edk2-staging/tree/HBFA). The original release of HBFA is described in the white-paper: ["Using Host-based Firmware Analysis to Improve Platform Resiliency"](https://www.intel.com/content/dam/develop/external/us/en/documents/intel-usinghbfatoimproveplatformresiliency-820238.pdf). The original release authors were Brian Richardson, Chris Wu, Jiewen Yao, and Vincent J. Zimmer.

The goal for HBFA-FL is to update the original codebase, enhancing some features and removing/streamlining some others, with a focus on functionality for Linux environments. To that end, HBFA-FL removes much of the original HBFA code base, including support for Windows and unit-testing; we retain support for fuzzing with AFL and LibFuzzer. This repository is effectively a fork from the original version of HBFA at this commit : [ead8f4f8eefdb2eb762184e1a5809c43d6908952](https://github.com/tianocore/edk2-staging/commit/ead8f4f8eefdb2eb762184e1a5809c43d6908952)

## Getting Started

To get started using HBFA-FL, we recommend taking a look at the documentation, starting [here](./docs/src/SUMMARY.md).
