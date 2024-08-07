# Setting up HBFA-FL for a Linux environment

In order to use HBFA-FL, several dependencies should be installed. If viable, a recommended approach for setting up an environment for HBFA is to leverage the [Tianocore/containers repository](https://github.com/tianocore/containers). The Dockerfiles there for the Fedora- and Ubuntu-based images provide a good baseline for HBFA as they can provide all the necessary packages and dependencies for building in the EDK-II environment. More, specifically, the 'build' targets in those images are a great starting point.

## Installing EDK-II

For EDK-II, please refer to the [instructions from EDK-II](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II) for additional details on setting up the EDK-II Environment. (Note the use of the Tianocore/container Dockerfile should result in an environment suitable for setting up the EDK-II code-base and building various platforms/components.)

For a quick-start, you can readily obtain the EDK-II source code from their [repository](https://github.com/tianocore/edk2). Following this approach, we can clone the repository to our local machine, and then build the base EDK-II tools, as shown in the following.

```console
root@a791b2478af5:/root/hbfa_workspace# git clone https://github.com/tianocore/edk2.git --recursive
root@a791b2478af5:/root/hbfa_workspace# cd edk2
root@a791b2478af5:/root/hbfa_workspace/edk2# make -C BaseTools
```

Here we note that when the 'make' command is ran for  building the EDK-II base tools, a series of tests should be ran. Verify that the tests are all passing ('ok') before proceeding. Lastly, we can run the 'edk2/edksetup.sh' script, this will set several important environment variables. Depending on how you are invoking a shell instance, you may need to source this command again to set the environment variables.

```console
root@a791b2478af5:/root/hbfa_workspace/edk2# source edksetup.sh
Using EDK2 in-source Basetools
WORKSPACE: /root/hbfa_workspace/edk2
EDK_TOOLS_PATH: /root/hbfa_workspace/edk2/BaseTools
CONF_PATH: /root/hbfa_workspace/edk2/Conf
Copying $EDK_TOOLS_PATH/Conf/build_rule.template
     to /root/hbfa_workspace/edk2/Conf/build_rule.txt
Copying $EDK_TOOLS_PATH/Conf/tools_def.template
     to /root/hbfa_workspace/edk2/Conf/tools_def.txt
Copying $EDK_TOOLS_PATH/Conf/target.template
     to /root/hbfa_workspace/edk2/Conf/target.txt
```

## Setting up HBFA-FL and the Build Environment (EDK-II with HBFA-FL)

Next, obtain the source code for HBFA-FL (e.g. via cloning the repo with the 'git' tool, as done for edk2). Once the source code for HBFA-FL has been retrieved, you'll need to set up a few environment variables and run a Python script included in HBFA-FL to help set-up the environment. To do this, follow the steps shown:

```console
root@a791b2478af5:~/hbfa_workspace# export WORKSPACE=~/hbfa_workspace/
root@a791b2478af5:~/hbfa_workspace# export PACKAGES_PATH=$WORKSPACE/edk2:$WORKSPACE/hbfa-fl/HBFA/
root@a791b2478af5:~/hbfa_workspace# python3 hbfa-fl/HBFA/UefiHostTestTools/HBFAEnvSetup.py
```

Noting, we are redefining the 'WORKSPACE' environment variable and defining the PACKAGES_PATH for EDK-II relative to this, which will include all code from the edk2 source and hbfa-fl source when building. Running the HBFAEnvSetup.py script, we will generate the 'build_rule.txt' and the 'tools_def.txt' configuration files under the HBFA-FL directory: ```HBFA/UefiHostFuzzTestPkg/Conf/```. These will be used by the EDK-II build system when compiling fuzzing test harnesses.

## Installing and setting up AFL

If you wish to fuzz with AFL, you will need to obtain, build, configure, and set-up a few environment variables.

First, assuming the use of the original AFL-2.52b version of AFL, you can obtain the source code from https://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz. Here, we'll use the ```wget``` tool to download the source, then build, and set up the appropriate environment.

```console
root@a791b2478af5:~/hbfa_workspace# wget -q https://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz
root@a791b2478af5:~/hbfa_workspace# tar xf afl-latest.tgz && rm afl-latest.tgz
root@a791b2478af5:~/hbfa_workspace# ll
total 20
drwxr-xr-x  5 root root 4096 Dec  7 19:03 ./
drwx------  1 root root 4096 Dec  7 17:03 ../
drwxr-xr-x 10  500  500 4096 Nov  5  2017 afl-2.52b/
drwxr-xr-x 36 root root 4096 Dec  7 16:35 edk2/
drwxrwxr-x  9 root root 4096 Dec  4 18:24 hbfa-fl/
root@a791b2478af5:~/hbfa_workspace# export AFL_PATH=/root/hbfa_workspace/afl-2.52b
root@a791b2478af5:~/hbfa_workspace# export PATH=$PATH:$AFL_PATH
```

Here, we have downloaded and extracted the source code for AFL and we set the environment variable 'AFL_PATH' to point to the extracted AFL directory. We also add the AFL_PATH to the shell path. Before proceeding, we need to build the source code for AFL, as shown in the following.

```console
root@a791b2478af5:~/hbfa_workspace/afl-2.52b# make
[*] Checking for the ability to compile x86 code...
[+] Everything seems to be working, ready to compile.
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-gcc.c -o afl-gcc -ldl
set -e; for i in afl-g++ afl-clang afl-clang++; do ln -sf afl-gcc $i; done
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-fuzz.c -o afl-fuzz -ldl
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-showmap.c -o afl-showmap -ldl
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-tmin.c -o afl-tmin -ldl
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-gotcpu.c -o afl-gotcpu -ldl
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-analyze.c -o afl-analyze -ldl
cc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" afl-as.c -o afl-as -ldl
ln -sf afl-as as
[*] Testing the CC wrapper and instrumentation output...
unset AFL_USE_ASAN AFL_USE_MSAN; AFL_QUIET=1 AFL_INST_RATIO=100 AFL_PATH=. ./afl-gcc -O3 -funroll-loops -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -DAFL_PATH=\"/usr/local/lib/afl\" -DDOC_PATH=\"/usr/local/share/doc/afl\" -DBIN_PATH=\"/usr/local/bin\" test-instr.c -o test-instr -ldl
echo 0 | ./afl-showmap -m none -q -o .test-instr0 ./test-instr
echo 1 | ./afl-showmap -m none -q -o .test-instr1 ./test-instr
[+] All right, the instrumentation seems to be working!
[+] All done! Be sure to review README - it's pretty short and useful.
```

Last, you may need to adjust the core pattern and CPU scaling for the AFL fuzzer. To do so, you may run command similar to the following. Note, if you are running a container image (e.g. via Docker), you may need to run the container in privileged mode to run these commands.

```console
# echo 'echo "core_%p_%s_%c_%d_%P_%E" > /proc/sys/kernel/core_pattern' >> /root/.bashrc
# echo 'echo "performance" | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor' >> /root/.bashrc
```

### Test build for the included fuzzing test-harnesses with AFL as the fuzzer

Next, to verify the set-up, you can copy the 'build_rule.txt' and 'tools_def.txt' files generated when you ran the HBFAEnvSetup.py script into the configuration directory for EDK-II, then invoke the build command for EDK-II (where the platform provide via the '-p' option the HBFA platform UefiHostFuzzTestCasePkg.dsc). We specify the '-t' option as 'AFL' to ensure the build leverages AFL for instrumenting the binary fuzzing harnesses. Note, additional details on building and running test-cases are included in this documentation (e.g. see this [section](../fuzzing/building.md)):

```console
root@a791b2478af5:~/hbfa_workspace# cp hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt edk2/Conf/build_rule.txt
root@a791b2478af5:~/hbfa_workspace# cp hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt edk2/Conf/tools_def.txt
root@a791b2478af5:~/hbfa_workspace# build -p hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a X64 -t AFL
```

If all is set-up correctly, all of the included test-cases should build without error and can be seen in subdirectories under the 'Build' directory, e.g. see the following directory listing.

```console
root@a791b2478af5:~/hbfa_workspace# ll Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/Test*
-rwxr-xr-x 1 root root  197952 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestBmpSupportLib*
-rwxr-xr-x 1 root root  148848 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestCapsulePei*
-rwxr-xr-x 1 root root  479448 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestFileName*
-rwxr-xr-x 1 root root   96528 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestFmpAuthenticationLibPkcs7*
-rwxr-xr-x 1 root root  114360 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestFmpAuthenticationLibRsa2048Sha256*
-rwxr-xr-x 1 root root  383072 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestIdentifyAtaDevice*
-rwxr-xr-x 1 root root 1149560 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestPartition*
-rwxr-xr-x 1 root root  207256 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestPeiGpt*
-rwxr-xr-x 1 root root  383688 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestPeiUsb*
-rwxr-xr-x 1 root root 1164584 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestTcg2MeasureGptTable*
-rwxr-xr-x 1 root root 1165784 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestTcg2MeasurePeImage*
-rwxr-xr-x 1 root root  268480 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestTpm2CommandLib*
-rwxr-xr-x 1 root root 1267064 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestUdf*
-rwxr-xr-x 1 root root 1347160 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestUsb*
-rwxr-xr-x 1 root root 2096272 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestValidateTdxCfv*
-rwxr-xr-x 1 root root 1702408 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestVariableSmm*
-rwxr-xr-x 1 root root 1171248 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestVirtio10Blk*
-rwxr-xr-x 1 root root 1085704 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestVirtioBlk*
-rwxr-xr-x 1 root root 1086016 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestVirtioBlkReadWrite*
-rwxr-xr-x 1 root root  937816 Dec  7 20:07 Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/X64/TestVirtioPciDevice*
```

## Installing and setting up LibFuzzer

For LibFuzzer, it is assumed that a modern version of Clang/LLVM is installed on your system. For example, at the time this document was written, the Ubuntu 22.04 default package for 'clang' and 'llvm' install version 14. If you wish or need to install and build from source, see the [LLVM web-site](https://releases.llvm.org/). For fuzzing with LibFuzzer in HBFA-FL, the necessary steps for setting up the right environment includes setting a few environment variables, as shown in the following. Take care to ensure you provide the correct paths for the 'clang' and 'llvm-symbolizer' binaries.

```console
root@a791b2478af5:~/hbfa_workspace# export CLANG_PATH=/usr/bin/
root@a791b2478af5:~/hbfa_workspace# export ASAN_SYMBOLIZER_PATH=$CLANG_PATH/llvm-symbolizer
```

### Test build for the included fuzzing test-harnesses with LibFuzzer as the fuzzer

Before attempting to build the fuzzing test-harnesses, we should re-generate the 'build_rule.txt' and 'tools_def.txt' configuration files and copy those to the EDK-II configuration directory.

```console
root@a791b2478af5:~/hbfa_workspace# python3 hbfa-fl/HBFA/UefiHostTestTools/HBFAEnvSetup.py
root@a791b2478af5:~/hbfa_workspace# cp hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt edk2/Conf/build_rule.txt
root@a791b2478af5:~/hbfa_workspace# cp hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt edk2/Conf/tools_def.txt
```

Lastly, we invoke the build command, this time specifying 'LIBFUZZER' with the '-t' option for the build target.

```console
root@a791b2478af5:~/hbfa_workspace# build -p hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a X64 -t LIBFUZZER
```

As with the build for AFL, if all is set-up correctly, all of the included test-cases should build without error and can be seen in subdirectories under the 'Build' directory, e.g. see the following directory listing.

```console
root@a791b2478af5:~/hbfa_workspace# ll Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/Test*
-rwxr-xr-x 1 root root 1887656 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestBmpSupportLib*
-rwxr-xr-x 1 root root 1851200 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestCapsulePei*
-rwxr-xr-x 1 root root 1981072 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestFileName*
-rwxr-xr-x 1 root root 1791440 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestFmpAuthenticationLibPkcs7*
-rwxr-xr-x 1 root root 1801336 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestFmpAuthenticationLibRsa2048Sha256*
-rwxr-xr-x 1 root root 2013056 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestIdentifyAtaDevice*
-rwxr-xr-x 1 root root 2610336 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestPartition*
-rwxr-xr-x 1 root root 1880272 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestPeiGpt*
-rwxr-xr-x 1 root root 2031512 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestPeiUsb*
-rwxr-xr-x 1 root root 2595768 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestTcg2MeasureGptTable*
-rwxr-xr-x 1 root root 2601456 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestTcg2MeasurePeImage*
-rwxr-xr-x 1 root root 1945488 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestTpm2CommandLib*
-rwxr-xr-x 1 root root 2643528 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestUdf*
-rwxr-xr-x 1 root root 2737248 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestUsb*
-rwxr-xr-x 1 root root 3115320 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestValidateTdxCfv*
-rwxr-xr-x 1 root root 3024768 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestVariableSmm*
-rwxr-xr-x 1 root root 2637264 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestVirtio10Blk*
-rwxr-xr-x 1 root root 2535064 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestVirtioBlk*
-rwxr-xr-x 1 root root 2535432 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestVirtioBlkReadWrite*
-rwxr-xr-x 1 root root 2402336 Dec  7 22:39 Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestVirtioPciDevice*
```

## Setting up conveniences and additional path and environment variables for HBFA-FL

A few additional items should be set-up to help improve usability of HBFA-FL. First, a few directories should be added to the system PATH, as well as a value for 'LLVM_PROFILE_FILE' should be specified if you intend to use LLVM-based coverage data with HBFA-FL.

```console
export PATH="$PATH:$WORKSPACE/hbfa-fl/HBFA/UefiHostTestTools/"
export PATH="$PATH:$WORKSPACE/hbfa-fl/HBFA/UefiHostTestTools/Report/"
export LLVM_PROFILE_FILE="$WORKSPACE/fuzz_session.profraw"
```

Last, ensuring several Python scripts associated with HBFA-FL are executable is recommended for usability. The following files should be set executable(here the files are relative to the $WORKSPACE environment variable you have selected, e.g. /root/hbfa_workspace in the present example).

- hbfa-fl/HBFA/UefiHostTestTools/Run*.py
- hbfa-fl/HBFA/UefiHostTestTools/Report/ReportMain.py
- hbfa-fl/HBFA/UefiHostTestTools/Report/GenCodeCoverage.py

[&lt;&lt;](./README.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](../fuzzing/README.md)