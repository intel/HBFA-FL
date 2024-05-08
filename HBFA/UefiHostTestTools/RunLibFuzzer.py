#!/usr/bin/env python3
# @file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import platform
import subprocess
import re
import shutil
import argparse

__prog__ = 'RunLibFuzzer.py'
__copyright__ = 'Copyright (c) 2019, Intel Corporation. All rights reserved.'
__version__ = '{} Version {}'.format(__prog__, '0.11 ')

# WORKSPACE
workspace = ""

# HBFA package path
HBFA_PATH = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

# Conf directory
Conf_Path = os.path.join(HBFA_PATH, 'UefiHostFuzzTestPkg', 'Conf')

# Get PYTHON version
PyVersion = sys.version_info[0]

# Get System type info
SysType = platform.system()

# Get System arch info
Is32bit = bool("64" not in platform.machine())

# Set a CLI command mode
CommandLineMode = None

# build tool chain
if SysType == "Windows":
    ToolChain = "LIBFUZZERWIN"
elif SysType == "Linux":
    ToolChain = "LIBFUZZER"


def CheckTestEnv(Arch):
    # Check whether EDKII BUILD WORKSPACE is set in system environment variable
    if 'WORKSPACE' not in os.environ:
        print("Please set system environment variable 'WORKSPACE' before "
              + "run this script.")
        os._exit(0)
    global workspace
    workspace = os.environ['WORKSPACE']

    # Check whether HBFA environment is set up
    if 'build_rule.txt' not in os.listdir(Conf_Path) or 'tools_def.txt' \
        not in os.listdir(Conf_Path) or 'target.txt'\
            not in os.listdir(Conf_Path):
        print("Please run HBFAEnvSetup.py before run this script.")
        os._exit(0)

    if "EDK_TOOLS_PATH" not in os.environ:
        print("[!] Please set EDK_TOOLS_PATH in system environment variables.")
        os._exit(0)
    global edkToolsPath
    edkToolsPath = os.environ['EDK_TOOLS_PATH']

    if SysType == "Windows":
        if Arch == "IA32":
            # Need to remove when LLVM support libFuzzer i386
            print("LLVM doesn't support libFuzzer i386 currently.")
            os._exit(0)
            # Check whether environment variable LLVMx86_PATH has been set
            if "LLVMx86_PATH" not in os.environ:
                print("Please set LLVMx86_PATH in system environment "
                      "variable.")
                os._exit(0)
        elif Arch == "X64":
            # Check whether environment variable LLVM_PATH has been set
            if "LLVM_PATH" not in os.environ:
                print("Please set LLVM_PATH in system environment variable.")
                os._exit(0)
    else:
        # Check whether environment variable CLANG_PATH has been set
        if "CLANG_PATH" not in os.environ:
            print("Please set CLANG_PATH in system environment variable.")
            os._exit(0)


def GetPkgName(path):
    return path.split(os.path.sep)[0]


def GetModuleBinName(ModuleInfPath):
    with open(ModuleInfPath, 'r') as f:
        lines = f.readlines()
        for line in lines:
            if 'BASE_NAME' in line:
                if SysType == "Windows":
                    return line.split('=')[1].strip() + '.exe'
                elif SysType == "Linux":
                    return line.split('=')[1].strip()


def CheckBuildResult(ModuleBinPath):
    if os.path.exists(ModuleBinPath):
        print("Build Successful !!!\n")
    else:
        print("Build failure, cannot find Module Binary file:"
              " {}".format(ModuleBinPath))
        os._exit(0)


def Build(Arch, Target, ModuleFilePath):
    PkgName = GetPkgName(ModuleFilePath)
    ModuleFileAbsPath = os.path.join(HBFA_PATH, ModuleFilePath)
    ModuleBinName = GetModuleBinName(ModuleFileAbsPath)
    ModuleBinAbsPath = os.path.join(workspace, 'Build', PkgName, Target
                                    + '_' + ToolChain, Arch, ModuleBinName)
    PlatformDsc = os.path.join(PkgName, PkgName+'.dsc')

    BuildCmdList = []
    BuildCmdList.append('-p')
    BuildCmdList.append('{}'.format(PlatformDsc))
    BuildCmdList.append('-m')
    BuildCmdList.append('{}'.format(ModuleFilePath))
    BuildCmdList.append('-a')
    BuildCmdList.append('{}'.format(Arch))
    BuildCmdList.append('-b')
    BuildCmdList.append('{}'.format(Target))
    BuildCmdList.append('-t')
    BuildCmdList.append('{}'.format(ToolChain))
    BuildCmdList.append('--conf')
    BuildCmdList.append('{}'.format(Conf_Path))

    if SysType == "Linux":
        # Add GCC5 build target for gcov output if Profraw format is used
        BuildCmdList.append('-t')
        BuildCmdList.append('GCC5')

    BuildCmd = [edkToolsPath + '/BinWrappers/PosixLike/build'] + BuildCmdList
    if SysType == "Windows":
        ExecCmd = BuildCmd
    elif SysType == "Linux":
        ExecCmd = BuildCmd
    print("Start build Test Module:")
    print(' '.join(BuildCmd))
    proccess = subprocess.Popen(ExecCmd,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT,
                                shell=False)
    msg = list(proccess.communicate())
    if PyVersion == 3:
        for num, submsg in enumerate(msg):
            if submsg is not None:
                msg[num] = submsg.decode()

    if msg[1]:
        print(msg[0] + msg[1])
        os._exit(0)
    elif "- Done -" not in msg[0]:
        print(msg[0])
        os._exit(0)
    else:
        pass
    CheckBuildResult(ModuleBinAbsPath)
    return ModuleBinAbsPath


def RunLibFuzzer(TestModuleBinPath, InputPath, OutputPath):
    LibFuzzer_CMD = []
    LibFuzzer_CMD.append("{}".format(TestModuleBinPath))
    LibFuzzer_CMD.append("{}".format(InputPath if InputPath
                                     else ''))
    LibFuzzer_CMD.append("-rss_limit_mb=0")
    LibFuzzer_CMD.append("-artifact_prefix={}".format(
        OutputPath + ('/' if SysType == "Linux" else '\\')))
    print("Start run LibFuzzer test:")
    if SysType == "Windows":
        ExecCmd = 'start cmd /k "{}"'.format(' '.join(LibFuzzer_CMD))
    elif SysType == "Linux":
        if CommandLineMode == 'rawcommand':
            print(' '.join(LibFuzzer_CMD))
            ExecCmd = LibFuzzer_CMD
            try:
                p = subprocess.Popen(ExecCmd,
                                     stdout=subprocess.PIPE,
                                     stderr=subprocess.STDOUT,
                                     shell=False)
                while True:
                    pout = p.stdout.readline()
                    if pout == b'' and p.poll() is not None:
                        break
                    elif pout:
                        print(pout.decode())
                p.poll()
            except Exception as err:
                print("Exception encountered: {}".format(err))
        elif CommandLineMode == 'manual':
            print('Run this command to initiate the fuzzer: '
                  '{}'.format(' '.join(LibFuzzer_CMD)))


def checkSanitizers(Sanitizers):
    CheckList = Sanitizers.split(',')
    CheckedSanitizers = ''
    SupportedSanitizers = ('address', 'memory', 'undefined',
                           'integer', 'bounds', 'enum', 'function')
    AddressMemoryCheck = {"address": False, "memory": False}
    for Sanitizer in CheckList:
        if Sanitizer in SupportedSanitizers:
            CheckedSanitizers += (',' + Sanitizer)
        else:
            return "[!] Unsupported sanitizer provided in option -s: [ %s ]" \
                % (Sanitizer)
        if Sanitizer == 'address' or Sanitizer == 'memory':
            AddressMemoryCheck[Sanitizer] = True
    if (AddressMemoryCheck['address'] is True) and \
       (AddressMemoryCheck['memory'] is True):
        return "[!] Unsupported combination of 'address' (ASAN) and 'memory'" \
               " (MSAN) for option '-s'."
    return CheckedSanitizers


# Parse command line options
def MyOptionParser():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--arch",
                        choices=['IA32', 'X64', 'ARM', 'AARCH64'],
                        dest="TargetArch", default="IA32",
                        help="ARCHS is one of list: IA32, X64, ARM or AARCH64,"
                        " which overrides target.txt's TARGET_ARCH "
                        "definition.")
    parser.add_argument("-b", "--buildtarget",
                        dest="BuildTarget", default="DEBUG",
                        help="Using the TARGET to build the platform, "
                        "overriding target.txt's TARGET definition.")
    parser.add_argument("-m", "--module", dest="ModuleFile",
                        help="Build the module specified by the INF file name "
                        "argument.")
    parser.add_argument("-i", "--input", dest="InputSeed",
                        help="Test input seed path.")
    parser.add_argument("-o", "--output", dest="Output",
                        help="Test output path for LibFuzzer.")
    parser.add_argument("-s", "--sanitizer", dest="SANITIZER",
                        default='address', help="A comma-separated list of "
                        "sanitizers to run with LibFuzzer. E.g. '--sanitizer="
                        "address'. Included sanitizers are: (ASAN) 'address'; "
                        "(MSAN) 'memory'; (UBSAN) 'undefined', 'integer', "
                        "'bounds', 'enum', and 'function'. NOTE: 'address' "
                        "and 'memory' cannot be used together. The default "
                        "sanitizer is 'address'. Support for Linux only.")
    parser.add_argument("-c", "--commandline", dest="CommandLine",
                        choices=['rawcommand', 'manual'],
                        default='rawcommand',
                        help="This specifies how the fuzzer is initiated from "
                        "command-line for Linux-based distributions. Specify "
                        "either: 'rawcommand', or 'manual'. "
                        "Using the 'rawcommand' mode is recommended/default."
                        " The 'manual' option will build the module and "
                        "then simply print a command-line option that the "
                        "end-user can subsequently use to run LibFuzzer.")
    parser.add_argument("-p", "--gen-profraw", dest="BuildProfraw",
                        choices=['t', 'T', 'true', 'True' or 'f', 'F', 'false',
                                 'False'],
                        default='False', help="Generate 'Source Based "
                        "Coverage' (Profraw) instead of the default "
                        "compilation option for gcov. Support for Linux "
                        "only. This setting will invoke the compilation "
                        "flags '-fprofile-instr-generate and "
                        "-fcoverage-mapping' for clang and libfuzzer.")

    args = parser.parse_args(sys.argv[1:])

    # Tokenize and check list input, pass checked list onward to Build
    args.SANITIZER = checkSanitizers(args.SANITIZER)
    if "[!]" in args.SANITIZER:
        print(args.SANITIZER)
        parser.print_help()
        os._exit(0)

    # Ensure bool
    boolStrTrue = ['t', 'T', 'true', 'True']
    boolStrFalse = ['f', 'F', 'false', 'False']
    if (args.BuildProfraw not in boolStrTrue) and \
       (args.BuildProfraw not in boolStrFalse):
        print("[!] Error for '-p, --gen-profraw' "
              + "option: {}".format(args.BuildProfraw)
              + " not supported. Please specify 'true' or 'false'.")
        parser.print_help()
        os._exit(0)
    else:
        if args.BuildProfraw in boolStrTrue:
            args.BuildProfraw = True
        elif args.BuildProfraw in boolStrFalse:
            args.BuildProfraw = False
        else:
            print("[!] Unexpected error parsing input for -p, "
                  "--gen-profraw argument: {}".format(args.BuildProfraw))

    return args


def updateBuildFlags(SanitizerFlags, buildProfraw):
    # Update the build option file for HBFA:
    # E.g. HBFA/UefiHostFuzzTestPkg/UefiHostFuzzTestBuildOption.dsc
    print("Updating UefiHostFuzzTestBuildOption.dsc")
    BasePath = HBFA_PATH + "/UefiHostFuzzTestPkg/"
    OriginalBuildOptionFile = BasePath + \
        "UefiHostFuzzTestBuildOption.dsc.original"
    # If OriginalBuildOptionFile does not exist, copy
    # UefiHostFuzzTestBuildOption.dsc
    if not os.path.isfile(OriginalBuildOptionFile):
        shutil.copyfile(BasePath + "UefiHostFuzzTestBuildOption.dsc",
                        OriginalBuildOptionFile)
    OutputBuildOptionFile = BasePath + "UefiHostFuzzTestBuildOption.dsc"
    try:
        frs = open(OriginalBuildOptionFile, 'rb')
        raw = frs.read()

        profrawCcCovFlag = rb'-fprofile-instr-generate -fcoverage-mapping'
        profrawLdCovFlag = rb'-fprofile-instr-generate -fcoverage-mapping'
        if os.getenv('LIB_FUZZING_ENGINE') is not None:
            LIB_FUZZING_ENGINE = os.getenv('LIB_FUZZING_ENGINE')
        else:
            LIB_FUZZING_ENGINE = ''
        if os.getenv('CXXFLAGS') is not None:
            CXXFLAGS = os.getenv('CXXFLAGS')
        else:
            CXXFLAGS = ''

        # Patch with appropriate coverage and sanitizer
        if buildProfraw:
            raw = re.sub(rb'GCC:\*_LIBFUZZER_\*_CC_FLAGS = '
                         rb'"-DTEST_WITH_LIBFUZZER=TRUE" -O1 '
                         rb'-fsanitize=fuzzer,address',
                         rb'GCC:*_LIBFUZZER_*_CC_FLAGS = "-DTEST_WITH_'
                         rb'LIBFUZZER=TRUE" -O1 ' + profrawCcCovFlag
                         + rb' -fsanitize=fuzzer' +
                         SanitizerFlags.encode(), raw)
            raw = re.sub(rb'GCC:\*_LIBFUZZER_\*_DLINK2_FLAGS = -fsanitize='
                         rb'fuzzer,address',
                         rb'GCC:*_LIBFUZZER_*_DLINK2_FLAGS = ' +
                         profrawLdCovFlag
                         + rb' -fsanitize=fuzzer' + SanitizerFlags.encode(),
                         raw)
            raw = re.sub(rb'GCC:\*_CLANG8_\*_CC_FLAGS = -O1 -fsanitize='
                         rb'address -fprofile-arcs -ftest-coverage',
                         rb'GCC:*_CLANG8_*_CC_FLAGS = -O1 '
                         + profrawCcCovFlag + rb' -fsanitize='
                         + SanitizerFlags[1::].encode(), raw)
            raw = re.sub(rb'GCC:\*_CLANG8_\*_DLINK2_FLAGS = -fsanitize=address'
                         rb' --coverage', rb'GCC:*_CLANG8_*_DLINK2_FLAGS = '
                         rb'-fsanitize=' + SanitizerFlags[1::].encode() + rb' '
                         + profrawLdCovFlag, raw)
        else:
            raw = re.sub(rb'GCC:\*_LIBFUZZER_\*_CC_FLAGS = '
                         rb'"-DTEST_WITH_LIBFUZZER=TRUE" -O1 '
                         rb'-fsanitize=fuzzer,address',
                         rb'GCC:*_LIBFUZZER_*_CC_FLAGS = '
                         rb'"-DTEST_WITH_LIBFUZZER=TRUE" -O1'
                         rb' -fsanitize=fuzzer' +
                         SanitizerFlags.encode() +
                         rb' ' +
                         CXXFLAGS.encode(), raw)
            raw = re.sub(rb'GCC:\*_LIBFUZZER_\*_DLINK2_FLAGS = -fsanitize='
                         rb'fuzzer,address',
                         rb'GCC:*_LIBFUZZER_*_DLINK2_FLAGS = -fsanitize=fuzzer'
                         + SanitizerFlags.encode() +
                         rb' ' +
                         LIB_FUZZING_ENGINE.encode(), raw)
            raw = re.sub(rb'GCC:\*_CLANG8_\*_CC_FLAGS = -O1 -fsanitize=address'
                         rb' -fprofile-arcs -ftest-coverage',
                         rb'GCC:*_CLANG8_*_CC_FLAGS = -O1 -fsanitize='
                         + SanitizerFlags[1::].encode() +
                         rb' -fprofile-arcs -ftest-coverage ' +
                         CXXFLAGS.encode(), raw)
            raw = re.sub(rb'GCC:\*_CLANG8_\*_DLINK2_FLAGS = -fsanitize=address'
                         rb' --coverage', rb'GCC:*_CLANG8_*_DLINK2_FLAGS = '
                         rb'-fsanitize=' + SanitizerFlags[1::].encode() +
                         rb' --coverage ' +
                         LIB_FUZZING_ENGINE.encode(), raw)

        # Write out file
        fws = open(OutputBuildOptionFile, 'wb')
        fws.write(raw)
        frs.close()
        fws.close()
    except Exception as err:
        print("[!] Error attempting to patch build description file "
              "UefiHostFuzzTestBuildOption.dsc: {}".format(err))
        os._exit(0)


def restoreBuildOptionFile():
    # Restore the original UefiHostFuzzTestBuildOption.dsc file for HBFA
    BasePath = HBFA_PATH + "/UefiHostFuzzTestPkg/"
    OriginalBuildOptionFile = BasePath + \
        "UefiHostFuzzTestBuildOption.dsc.original"
    OutputBuildOptionFile = BasePath + "UefiHostFuzzTestBuildOption.dsc"
    if os.path.isfile(OriginalBuildOptionFile):
        try:
            shutil.copyfile(OriginalBuildOptionFile, OutputBuildOptionFile)
        except IOError as err:
            print("[!] Error making backup of UefiHostFuzzTestBuildOption.dsc"
                  " file: {}".format(err))
    else:
        print("[!] Error: backup copy of UefiHostFuzzTestBuildOption.dsc not "
              "found at path: {}".format(OriginalBuildOptionFile))


def main():
    args = MyOptionParser()
    TargetArch = args.TargetArch
    BuildTarget = args.BuildTarget

    CheckTestEnv(TargetArch)

    if (SysType == 'Linux'):
        if (TargetArch == "IA32") ^ Is32bit:
            print("For CLANG: {}".format("i386" if Is32bit else "x64_86") +
                  "system cannot support arch {} build.".format(TargetArch))
            os._exit(0)

    if not args.ModuleFile:
        print("ModuleFile should be set once by command -m MODULEFILE, "
              "--module=MODULEFILE.")
        os._exit(0)
    elif os.path.isabs(args.ModuleFile):
        if args.ModuleFile.startswith(HBFA_PATH):
            ModuleFilePath = os.path.relpath(args.ModuleFile, HBFA_PATH)
        else:
            print("ModuleFile path: {}".format(args.ModuleFile) +
                  " should start with {}.".format(HBFA_PATH))
            os._exit(0)
    elif not os.path.exists(os.path.join(HBFA_PATH, args.ModuleFile)):
        print("ModuleFile path: {}".format(args.ModuleFile)
              + " does not exist or is not in the relative path for HBFA: {}".format(os.path.abspath(HBFA_PATH)))
        os._exit(0)
    else:
        ModuleFilePath = args.ModuleFile

    OutputPath = args.Output if args.Output \
        else os.path.join(os.getcwd(), 'failureSeeds')
    if not os.path.isabs(OutputPath):
        OutputPath = os.path.join(os.getcwd(), OutputPath)
    if not os.path.exists(OutputPath):
        try:
            os.makedirs(OutputPath)
        except Exception as err:
            print(err)
    elif os.path.exists(os.path.join(OutputPath, 'fuzzfile.bin')):
        print("OutputPath:{} has already".format(OutputPath) +
              " been used for LibFuzzer test, please change another"
              " directory.")
        os._exit(0)
    print("LibFuzzer output will be generated in current "
          "directory:{}".format(OutputPath))

    InputSeedPath = args.InputSeed if args.InputSeed \
        else os.path.join(os.getcwd(), 'Seeds')
    if not os.path.isabs(InputSeedPath):
        InputSeedPath = os.path.join(os.getcwd(), InputSeedPath)
    if not os.path.exists(InputSeedPath):
        try:
            os.makedirs(InputSeedPath)
        except Exception as err:
            print(err)

    if (SysType == 'Linux'):
        global CommandLineMode
        CommandLineMode = args.CommandLine

    if (SysType == 'Linux'):
        updateBuildFlags(args.SANITIZER, args.BuildProfraw)

    TestModuleBinPath = Build(TargetArch, BuildTarget, ModuleFilePath)

    if (SysType == 'Linux'):
        restoreBuildOptionFile()

    RunLibFuzzer(TestModuleBinPath, InputSeedPath, OutputPath)


if __name__ == "__main__":
    main()
