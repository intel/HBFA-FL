### Fuzzing with LibFuzzer: RunLibFuzzer.py

Overall, the general approach for fuzzing with LibFuzzer is similar that that done for AFL; however, instead of RunAFL.py, we will use the RunLibFuzzer.py script in HBFA. This command offers a similar set of command-line arguments that are required; however, RunLibFuzzer.py offers a few important, additional features. Examining the '-h' (help) contents:

```console
# RunLibFuzzer.py -h
usage: RunLibFuzzer.py [-h] [-a {IA32,X64,ARM,AARCH64}] [-b BUILDTARGET] [-m MODULEFILE] [-i INPUTSEED] [-o OUTPUT] [-s SANITIZER] [-c {rawcommand,manual}] [-p {t,T,true,True,F,false,False}]

options:
  -h, --help            show this help message and exit
  -a {IA32,X64,ARM,AARCH64}, --arch {IA32,X64,ARM,AARCH64}
                        ARCHS is one of list: IA32, X64, ARM or AARCH64, which overrides target.txt's TARGET_ARCH definition.
  -b BUILDTARGET, --buildtarget BUILDTARGET
                        Using the TARGET to build the platform, overriding target.txt's TARGET definition.
  -m MODULEFILE, --module MODULEFILE
                        Build the module specified by the INF file name argument.
  -i INPUTSEED, --input INPUTSEED
                        Test input seed path.
  -o OUTPUT, --output OUTPUT
                        Test output path for LibFuzzer.
  -s SANITIZER, --sanitizer SANITIZER
                        A comma-separated list of sanitizers to run with LibFuzzer. E.g. '--sanitizer=address'. Included sanitizers are: (ASAN) 'address'; (MSAN) 'memory'; (UBSAN) 'undefined', 'integer', 'bounds',
                        'enum', and 'function'. NOTE: 'address' and 'memory' cannot be used together. The default sanitizer is 'address'. Support for Linux only.
  -c {rawcommand,manual}, --commandline {rawcommand,manual}
                        This specifies how the fuzzer is initiated from command-line for Linux-based distributions. Specify either: 'rawcommand', or 'manual'. Using the 'rawcommand' mode is recommended/default. The
                        'manual' option will build the module and then simply print a command-line option that the end-user can subsequently use to run LibFuzzer.
  -p {t,T,true,True,F,false,False}, --gen-profraw {t,T,true,True,F,false,False}
                        Generate 'Source Based Coverage' (Profraw) instead of the default compilation option for gcov. Support for Linux only. This setting will invoke the compilation flags '-fprofile-instr-generate
                        and -fcoverage-mapping' for clang and libfuzzer.
```

As in the prior example with AFL, we will fuzz the TestBmpSupportLib test-case. Therefore, arguments for the architecture ('-a'), the fuzzing module file ('-m'), and the fuzzing input seed ('-i') will remain the same. Likewise, we will not specify a value for the command line ('-c'), leaving it as default so the fuzzing session will be ran automatically. Also, we will specify a different location for the output directory ('-o'). Notably, there are two additional options we can consider with RunLibFuzz.py: '-s' for specifying the sanitizers to be built into our fuzzer and '-p', which is an option to use LLVM-based, 'Source Based Coverage' (Profraw format) coverage data. 

For the sanitizers ('-s') option, one can choose any combination of those listed in the help output, except the sanitizers 'address' and 'memory' cannot be used at the same time; a different build and run of the fuzzing session would be required if both are needed by the end user in their fuzzing efforts.

Regardless of whether '-p' is specified or not, a GCC-based gov binary will be created when the RunLibFuzzer.py' script is ran; this can be later used by the 'ReportGen.py' and 'GenCodeCoverage.py' scripts for GCOV and lcovtool-based reports. When the '-p' option is included, the LibFuzzer fuzzing binary built for the test-case will be instrumented and emit Profraw ('Source-based' coverage data). This can later be used when invoking the 'GenCodeCoverage.py' command to generate llvm-tool based coverage reports. Note, you may wish to set the LLVM_PROFILE_FILE environment variable to control where the Profraw coverage data is emitted.

In the following show an example build and run of the TestBmpSupportLib test-case using 'RunLibFuzzer.py'. Here, '-p' is used to add 'Source Based Coverage' (Profraw) and nothing is specified for the '-s' option (a default use of 'address' sanitizer is applied).

```console
# RunLibFuzzer.py -a X64 -m /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -i /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -o /tmp/fuzz_RunLibFuzzer_TestBmpSupportLib -p true
LibFuzzer output will be generated in current directory:/tmp/fuzz_RunLibFuzzer_TestBmpSupportLib
Updating UefiHostFuzzTestBuildOption.dsc
Start build Test Module:
build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -m UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -a X64 -b DEBUG -t LIBFUZZER --conf /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf -t GCC5
Build Successful !!!

Start run LibFuzzer test:
/root/hbfa_workspace/Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestBmpSupportLib /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -rss_limit_mb=0 -artifact_prefix=/tmp/fuzz_RunLibFuzzer_TestBmpSupportLib/
INFO: Running with entropic power schedule (0xFF, 100).

INFO: Seed: 446151718

INFO: Loaded 1 modules   (754 inline 8-bit counters): 754 [0x5abdc8, 0x5ac0ba), 

INFO: Loaded 1 PC tables (754 PCs): 754 [0x5797a0,0x57c6c0), 

INFO:       10 files found in /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw

INFO: -max_len is not provided; libFuzzer will not generate inputs larger than 4096 bytes
...

```

For LibFuzzer, the fuzzing session will continue until either a crash is encountered or you press 'Ctrl+c'. If crashes are found, the files will be saved to the output directory specified via the '-o' option. *Importantly*, for any additional code paths discovered during the fuzzing sessions, additional seed files are added by LibFuzzer to the directory specified via the '-i' option. *Note*, if you wish to run the fuzzing session with additional parameters for LibFuzzer, take note of the following lines from the 'RunLibFuzzer.py output'; use the command printed as a starting point to add/modify to the command.

```console
Start run LibFuzzer test:
/root/hbfa_workspace/Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestBmpSupportLib /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -rss_limit_mb=0 -artifact_prefix=/tmp/fuzz_RunLibFuzzer_TestBmpSupportLib/
```

### Examining the LibFuzzer output directory

If there were not any crashes discovered, the output directory will be empty as shown in the following.

```console
# ls -l /tmp/fuzz_RunLibFuzzer_TestBmpSupportLib
total 0
```

However, if there were crashes found, the input files that generated those crashes (or sanitizer detections) will be in the output directory, e.g. see the subsequent directory listing. Noting that some UBSAN detections are printed to the console output and do not terminate the fuzzing session.

```console
# ls -l /tmp/fuzz_RunLibFuzzer_TestBmpSupportLib
total 4
-rw-r--r-- 1 root root 62 May 21 18:35 crash-0205a0e4ec89a2fd77df87a0688599f704a5ddc2
```

[&lt;&lt;](./fuzzingwithAFL.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](./generatingCoverageReports.md)