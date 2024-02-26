# Getting started with fuzzing in HBFA-FL

Fuzzing with AFL and LibFuzzer are supported in HBFA-FL. For a given test-harness, the source can be compiled and fuzzed with either AFL or LibFuzzer. To do this, you may directly invoke the build process from EDK-II or you may leverage the included 'RunAFL.py' and 'RunLibFuzzer.py' scripts (recommended). Additionally, several fuzzing test-harnesses are included with HBFA-FL and can be helpful for fuzzing, or as a good reference for developing other fuzzing harnesses.

- For information on creating and building test cases, see these sections: [Where to create and save a fuzzing test case harness](../harness/wheretoharness.md) and [Creating and Compiling New Test Cases](./building.md)
- For a full-tutorial on creating fuzzing harnesses for HBFA-FL, see the tutorial [HBFA-FL: Writing a fuzzing harness](../tutorials/writingafuzzingharness.md)
- For more information on the fuzzing test-harnesses included with HBFA-FL, see this [section](../harness/includedfuzzharnesses.md)
- To get started fuzzing with AFL, see [Fuzzing with AFL](fuzzingwithAFL.md)
- To get started fuzzing with LibFuzzer, see [Fuzzing with LibFuzzer](./fuzzingwithLibFuzzer.md)
- To learn more on how to generate Fuzzing Report and Coverage data for your fuzzing, see [Generating fuzzing summary and coverage data reports](./generatingCoverageReports.md)

[&lt;&lt;](../setup/linux.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](./building.md)