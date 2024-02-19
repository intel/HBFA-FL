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
import argparse

__prog__ = 'RunKLEE.py'
__copyright__ = 'Copyright (c) 2019, Intel Corporation. All rights reserved.'
__version__ = '{} Version {}'.format(__prog__, '0.11 ')

# Get System type info
SysType = platform.system()

# build tool chain
ToolChain = "KLEE"

# WORKSPACE
workspace = ""

# KLEE_SRC_PATH
klee_src_path = ""

# HBFA package path
HBFA_PATH = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

# Conf directory
Conf_Path = os.path.join(HBFA_PATH, 'UefiHostFuzzTestPkg', 'Conf')

# Get PYTHON version
PyVersion = sys.version_info[0]

# Set a CLI command mode
CommandLineMode = None


def CheckTestEnv():
    # Need to remove if support KLEE in Windows
    if SysType == 'Windows':
        print("KLEE is not supported in Windows currently.")
        os._exit(0)

    # Check whether EDKII BUILD WORKSPACE is set in system environment variable
    if 'WORKSPACE' not in os.environ:
        print("Please set system environment variable 'WORKSPACE' before run "
              "this script.")
        os._exit(0)
    global workspace
    workspace = os.environ['WORKSPACE']

    # Check whether KLEE_SRC_PATH is set in system environment variable
    if 'KLEE_SRC_PATH' not in os.environ:
        print("Please set system environment variable 'KLEE_SRC_PATH' before "
              "run this script.")
        os._exit(0)
    global klee_src_path
    klee_src_path = os.environ['KLEE_SRC_PATH']

    if "EDK_TOOLS_PATH" not in os.environ:
        print("[!] Please set EDK_TOOLS_PATH in system environment variables.")
        os._exit(0)
    global edkToolsPath
    edkToolsPath = os.environ['EDK_TOOLS_PATH']

    # Check whether HBFA environment is set up
    if 'build_rule.txt' not in os.listdir(Conf_Path) or 'tools_def.txt' \
        not in os.listdir(Conf_Path) or 'target.txt'\
            not in os.listdir(Conf_Path):
        print("Please run HBFAEnvSetup.py before run this script.")
        os._exit(0)


def GetPkgName(path):
    return path.split(os.path.sep)[0]


def GetModuleBinName(ModuleInfPath):
    with open(ModuleInfPath, 'r') as f:
        lines = f.readlines()
        for line in lines:
            if 'BASE_NAME' in line:
                return line.split('=')[1].strip()


def CheckBuildResult(ModuleBinPath):
    if os.path.exists(ModuleBinPath):
        print("Build Successfully !!!\n")
    else:
        print("Can not find Module Binary file: {}".format(ModuleBinPath))
        os._exit(0)


def Build(Arch, Target, ModuleFilePath):
    PkgName = GetPkgName(ModuleFilePath)
    ModuleFileAbsPath = os.path.join(HBFA_PATH, ModuleFilePath)
    ModuleBinName = GetModuleBinName(ModuleFileAbsPath)
    ModuleBinAbsPath = os.path.join(workspace, 'Build', PkgName,
                                    Target + '_' + ToolChain, Arch,
                                    ModuleBinName)
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
    BuildCmdList.append('-DTEST_WITH_KLEE')
    BuildCmdList.append('--disable-include-path-check')

    BuildCmd = [edkToolsPath + '/BinWrappers/PosixLike/build'] + BuildCmdList
    os.environ['SCRIPT_PATH'] = HBFA_PATH \
        + "/UefiHostFuzzTestPkg/Conf/LLVMLink.py"
    os.environ['LLVM_COMPILER'] = 'clang'
    print("Start build Test Module:")
    print(' '.join(BuildCmd))
    p = subprocess.Popen(BuildCmd,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT,
                         shell=False)
    msg = list(p.communicate())
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


def RunKLEE(TestModuleBinPath, OutPutPath):
    SetEnvCmd = "export PATH=$KLEE_BIN_PATH:$PATH\n"
    SetLimit = "ulimit -s unlimited\n"
    KLEE_CMD = "klee --only-output-states-covering-new " \
        + "-output-dir={}  {}".format(OutPutPath, TestModuleBinPath)
    print("Start run KLEE test:")
    print(KLEE_CMD)
    if CommandLineMode == 'rawcommand':
        ExecCmd = SetEnvCmd + SetLimit + KLEE_CMD
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
        except Exception as error:
            print("Exception encountered: {}".format(error))
    elif CommandLineMode == 'manual':
        print('Run this command to initiate KLEE: '
              '{}'.format(SetEnvCmd + SetLimit + KLEE_CMD))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--arch",
                        help="ARCHS is one of the list: IA32, X64, "
                        "ARM or AARCH64, which overrides target.txt's "
                        "TARGET_ARCH definition",
                        choices=['IA32', 'X64', 'ARM', 'AARCH64'],
                        dest="TargetArch", default="IA32")
    parser.add_argument("-b", "--buildtarget",
                        help="Using the TARGET to build the platform, "
                        "overriding target.txt's TARGET definition.",
                        dest="BuildTarget", default="DEBUG")
    parser.add_argument("-m", "--module",
                        help="Build the module specified by the INF file name "
                        "argument.",
                        dest="ModuleFile")
    parser.add_argument("-o", "--output",
                        help="Test output path for Klee.",
                        dest="Output")
    parser.add_argument("-c", "--commandline",
                        help="This specifies how the Klee is initiated from "
                        "command-line for Linux-based distributions. Specify "
                        "either: 'rawcommand' or 'manual'. "
                        "Using the 'rawcommand' mode is recommended. "
                        "The 'manual' option will build the module and "
                        "then simply print a command-line option that "
                        "the end-user can subsequently use to run Klee.",
                        choices=['rawcommand', 'manual'],
                        dest="CommandLine", default="rawcommand")

    args = parser.parse_args(sys.argv[1:])

    TargetArch = args.TargetArch
    BuildTarget = args.BuildTarget

    CheckTestEnv()

    if not args.ModuleFile:
        print("ModuleFile should be set once by command -m MODULEFILE, "
              "--module=MODULEFILE.")
        os._exit(0)
    elif os.path.isabs(args.ModuleFile):
        if args.ModuleFile.startswith(HBFA_PATH):
            ModuleFilePath = os.path.relpath(args.ModuleFile, HBFA_PATH)
        else:
            print("ModuleFile path: {} should start with {}.".format(
                args.ModuleFile, HBFA_PATH))
            os._exit(0)
    elif not os.path.exists(os.path.join(HBFA_PATH, args.ModuleFile)):
        print("ModuleFile path: {}".format(args.ModuleFile) +
              " does not exist or is not in the relative path for HBFA")
        os._exit(0)
    else:
        ModuleFilePath = args.ModuleFile

    if not args.Output:
        print("OutputSeed path should be set once by command -o OUTPUT, "
              "--output=OUTPUT.")
        os._exit(0)
    else:
        OutputSeedPath = args.Output
        if os.path.exists(OutputSeedPath):
            print("OutputSeedPath:{}".format(OutputSeedPath) +
                  " already exists, please select another folder.")
            os._exit(0)
        else:
            if not os.path.exists(os.path.dirname(OutputSeedPath)):
                os.makedirs(os.path.dirname(OutputSeedPath))

    global CommandLineMode
    CommandLineMode = args.CommandLine

    TestModuleBinPath = Build(TargetArch, BuildTarget, ModuleFilePath)
    RunKLEE(TestModuleBinPath, OutputSeedPath)


if __name__ == "__main__":
    main()
