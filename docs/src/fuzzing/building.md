# Creating and Compiling New Test Cases

The original HBFA documentation provides some helpful information on how to to about adding new test cases/harnesses. For more information, see [Host-based Firmware Analyzer User Guide: How to Add New Case](https://github.com/tianocore/edk2-staging/blob/HBFA/HBFA/Doc/User%20Guide%20-%20How-to-Add-New-Case.pdf). Further, a [tutorial](../tutorials/writingafuzzingharness.md) is included in HBFA-FL, which gives details on creating a fuzzing test harness for a vulnerable (fictitious) testHelloWorld UEFI shell program.

## Compiling Test Cases

The recommended approach with HBFA is to use the included RunAFL.py and RunLibFuzzer.py scripts. These scripts will invoke the build commands needed (see the sections: [Using RunAFL.py to build and fuzzing a module](#b-using-runaflpy-to-build-and-fuzzing-a-module) and [Using LibFuzzer.py to build and fuzzing a module](#b-using-libfuzzerpy-to-build-and-fuzzing-a-module). However, one can directly invoke the build system in EDK-II.

For building a test module in EDK-II, some of the important files used in the HBFA environment are described in the following. (Noting, the file paths are releative to the base of the edk2 repository.)

| Filename | Description |
| -------- | ----------- |
| edk2/BaseTools/BinWrappers/PosixLike/build | When invoking 'build' from the CLI in HBFA, this Bash script is ran and acts as a wrapper to invoke a Python-based build script 'build.py' for the HBFA environment in this Docker image.|
| edk2/BaseTools/Source/Python/build/build.py | Primary script used to orchestrate  building a platform or a module for EDK-II |

When building a test module in HBFA, an invocation of the 'build' command may be done similar to that shown in the following.

```console
build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -m UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -a X64 -b DEBUG -t AFL --conf /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf -t GCC5
```

The 'build' script has many options and features (e.g. see the output from ```build -h```). Some useful flags used (and available in the HBFA environment are)

| Build CLI option | Purpose | Notes/Available options |
| ---------------- | ------- | ----------------- |
| -p | To specify the platform (.dsc) file name | In this case, the platform should be the HBFA, UefiHostFuzzTestCasePkg.dsc file. |
| -m | To specify the test case the module specified by the INF file name argument | This should point to the test module file you have created and with to build/fuzz (or a pre-built test-case) |
| -a | To specify the target architecture | Per HBFA documentation, only 'X64' is supported for LibFuzzer. Use of 'IA32' is ok for AFL in HBFA. |
| -t | This is used to specify the toolchain | Note multiple targets can be specified (e.g. ```-t AFL -t GCC5```) |
| --conf | the customized Conf directory | For HBFA, this should be set as 'hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf' |

[&lt;&lt;](./README.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](../harness/includedfuzzharnesses.md)