# Fuzzing with AFL: RunAFL.py

To demonstrate using the RunAFL.py script for fuzzing, we'll build and fuzz the TestBmpSupportLib test-case included with HBFA-FL. First, it is important to understand some of the required arguments for the RunAFL.py script. To that end, the help contents for the script are:

```console
# RunAFL.py -h
usage: RunAFL.py [-h] [-a {IA32,X64,ARM,AARCH64}] [-b BUILDTARGET] [-m MODULEFILE] [-i INPUTSEED] [-o OUTPUT] [-c {rawcommand,manual}]

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
                        Test output path for AFL.
  -c {rawcommand,manual}, --commandline {rawcommand,manual}
                        This specifies how the fuzzer is initiated from command-line for Linux-based distributions. Specify either: 'rawcommand' or 'manual'. Using 'rawcommand' will directly initiate the AFL fuzzer
                        after building the test module and is recommended for automated approaches. Alternatively, 'manual' may be used to simply build the test case and print out a command that the user can run to
                        start the fuzzer (this will preserve the fuzzing display for AFL.
```

Here, we will need to reference a few files when running the script. For the TestBmpSupportLib test case, the following files should be referenced as arguments to the RunAFL.py command. **Note**, the full paths for the files should be specified.

| File Location | Description |
| ------------- | ----------- |
| hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf | This file is used with the '-m' option. This is the component description file for the test-case/model. This file will reference the sources, dependencies, etc. for this test-case. |
| hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw | This folder is used with the '-i' option (for the input seed(s)). This directory contains several Bitmap image files (.BMP) that will serve as a fuzzing corpus. |

Additionally, one must specify the target architecture (this example will target 'X64'), via the option '-a'. Further, the output directory for fuzzing session output should be provided via the '-o' option. If not specified, when RunAFL.py is invoked, the default assumption/value for the '--commandline' argument will be 'rawcommand', meaning the RunAFL.py script will automatically launch AFL after building the fuzzing harness.

Putting all this together, the fuzzing test-case can be built and ran as shown:

```console
# RunAFL.py -a X64 -m /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -i /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -o /tmp/fuzz_RunAFL_TestBmpSupportLib
Start build Test Module:
build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -m UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -a X64 -b DEBUG -t AFL --conf /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf -t GCC5
Build Successfully !!!

Start run AFL test:
afl-fuzz -i /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -o /tmp/fuzz_RunAFL_TestBmpSupportLib /root/hbfa_workspace/Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestBmpSupportLib @@
afl-fuzz 2.52b by <lcamtuf@google.com>

[+] Looks like we're not running on a tty, so I'll be a bit less verbose.
...
[+] Here are some useful stats:



    Test case count : 8 favored, 0 variable, 10 total

       Bitmap range : 23 to 94 bits (average: 64.50 bits)

        Exec timing : 135 to 172 us (average: 147 us)



[*] No -t option specified, so I'll use exec timeout of 20 ms.

[+] All set and ready to roll!

[*] Entering queue cycle 1.

[*] Fuzzing test case #0 (10 total, 0 uniq crashes found)...

[*] Fuzzing test case #1 (36 total, 0 uniq crashes found)...

[*] Fuzzing test case #2 (37 total, 0 uniq crashes found)...
...
```

Here, we note your full-path to the hbfa-fl directly may differ. Alternatively, if you wish to simply build the test case and run the fuzzer later (or with other options), you can invoke the RunAFL.py script with the '--commandline' argument 'manual' as show in the following.

```console
# RunAFL.py -a X64 -m /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -i /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -o /tmp/fuzz_RunAFL_TestBmpSupportLib --commandline manual
Start build Test Module:
build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -m UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf -a X64 -b DEBUG -t AFL --conf /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf -t GCC5
Build Successfully !!!

Start run AFL test:
Run this command to initiate the fuzzer: afl-fuzz -i /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -o /tmp/fuzz_RunAFL_TestBmpSupportLib /root/hbfa_workspace/Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestBmpSupportLib @@
```

As can be seen in the output, a command is printed that can be copied and used to run the built-test case with 'afl-fuzz'. Adjust and run the command as needed to initiate a fuzzing session. E.g. for the present example, the following command will kick off the fuzzing session.

```console
# afl-fuzz -i /root/hbfa_workspace/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw -o /tmp/fuzz_RunAFL_TestBmpSupportLib /root/hbfa_workspace/Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestBmpSupportLib @@
```

The fuzzing session can be terminated by pressing 'Ctrl+c'.

### Examining the AFL output directory

In the example provided in the prior sub-section, the fuzzing output was set to output to the directory '/tmp/fuzz_RunAFL_TestBmpSupportLib'. A file listing of this output directory is shown in the following.

```console
[root@92fd93545422 hbfa_workspace]# ls /tmp/fuzz_RunAFL_TestBmpSupportLib
crashes  fuzz_bitmap  fuzzer_stats  hangs  plot_data  queue
```

This is the standard output directories and files per the AFL fuzzer. For more information on the output, see [Fuzzing with afl-fuzz](https://afl-1.readthedocs.io/en/latest/fuzzing.html). Some useful folder locations include the 'crashes' directory, which will include an input file suitable for triggering any of the crashes discovered. Likewise, the queue directory will contain input files (test cases) that will reach each code path discovered.

[&lt;&lt;](../harness/includedfuzzharnesses.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](./fuzzingwithLibFuzzer.md)