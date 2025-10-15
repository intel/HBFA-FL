#!/usr/bin/env python3
# @file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import shutil
import platform
import subprocess
import argparse

__prog__ = 'RunAFL.py'
__copyright__ = 'Copyright (c) 2019, Intel Corporation. All rights reserved.'
__version__ = '{} Version {}'.format(__prog__, '0.11 ')

# Get System type info
SysType = platform.system()

# Get System arch info
Is32bit = bool("64" not in platform.machine())

# build tool chain
if SysType == "Windows":
    ToolChain = "VS2015x86"
elif SysType == "Linux":
    ToolChain = "AFL"

# WORKSPACE
workspace = ''

# HBFA package path
HBFA_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Conf directory
Conf_Path = os.path.join(HBFA_PATH, 'UefiHostFuzzTestPkg', 'Conf')

# Get PYTHON version
PyVersion = sys.version_info[0]

# Set a CLI command mode
CommandLineMode = None


def CheckTestEnv():
    # Check EDKII BUILD WORKSPACE whether be set in system environment
    # variable
    if 'WORKSPACE' not in os.environ:
        print("[!] Please set system environment variable 'WORKSPACE' before "
              "run this script.")
        os._exit(0)
    global workspace
    workspace = os.environ['WORKSPACE']

    # Check whether HBFA environment is set up
    if 'build_rule.txt' not in os.listdir(Conf_Path) or 'tools_def.txt' \
        not in os.listdir(Conf_Path) or 'target.txt'\
            not in os.listdir(Conf_Path):
        print("[!] Please run HBFAEnvSetup.py before run this script.")
        os._exit(0)

    if "AFL_PATH" not in os.environ:
        print("[!] Please set AFL_PATH in system environment variables.")
        os._exit(0)

    if "EDK_TOOLS_PATH" not in os.environ:
        print("[!] Please set EDK_TOOLS_PATH in system environment variables.")
        os._exit(0)
    global edkToolsPath
    edkToolsPath = os.environ['EDK_TOOLS_PATH']

    if SysType == "Windows" and "DRIO_PATH" not in os.environ:
        print("Please set DRIO_PATH in system environment variables.")
        os._exit(0)


def GetPkgName(path):
    return path.split(os.path.sep)[0]


def GetModuleBinName(ModuleInfPath):
    with open(ModuleInfPath, 'r') as f:
        lines = f.readlines()
        for line in lines:
            if 'BASE_NAME' in line:
                if SysType == "Windows":
                    return line.split('=')[1].strip() + ".exe"
                elif SysType == "Linux":
                    return line.split('=')[1].strip()


def CheckBuildResult(ModuleBinPath):
    if os.path.exists(ModuleBinPath):
        print("Build Successfully !!!\n")
    else:
        print("Build failure, can not find Module Binary file: "
              "{}".format(ModuleBinPath))
        os._exit(0)


def CopyFile(src, dst):
    try:
        shutil.copyfile(src, dst)
    except Exception as err:
        print(err)


def Build(Arch, Target, ModuleFilePath):
    PkgName = GetPkgName(ModuleFilePath)
    ModuleFileAbsPath = os.path.join(HBFA_PATH, ModuleFilePath)
    ModuleBinName = GetModuleBinName(ModuleFileAbsPath)
    ModuleBinAbsPath = os.path.join(workspace, 'Build', PkgName, Target +
                                    '_' + ToolChain, Arch, ModuleBinName)
    PlatformDsc = os.path.join(HBFA_PATH, PkgName, PkgName+'.dsc')

    BuildCmdList = []
    BuildCmdList.append('-p')
    BuildCmdList.append('{}'.format(PlatformDsc))
    BuildCmdList.append('-m')
    BuildCmdList.append('{}'.format(ModuleFileAbsPath))
    BuildCmdList.append('-a')
    BuildCmdList.append('{}'.format(Arch))
    BuildCmdList.append('-b')
    BuildCmdList.append('{}'.format(Target))
    BuildCmdList.append('-t')
    BuildCmdList.append('{}'.format(ToolChain))
    BuildCmdList.append('--conf')
    BuildCmdList.append('{}'.format(Conf_Path))
    if SysType == 'Linux':
        BuildCmdList.append('-t')
        BuildCmdList.append('GCC5')

    buildBinary = [edkToolsPath + '/BinWrappers/PosixLike/build']
    ExecCmd = buildBinary + BuildCmdList
    p = subprocess.Popen(ExecCmd,
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


def RunAFL(TestModuleBinPath, InputPath, OutputPath):
    TestModuleName = TestModuleBinPath.split('\\')[-1]
    if SysType == "Windows":
        if "IA32" in TestModuleBinPath:
            AFL_PATH = os.path.join(os.environ["AFL_PATH"], "bin32")
            # Copy xxx.exe and xxx.pdb to the same dir as winafl\bin32 or
            # winafl\bin64
            CopyFile(TestModuleBinPath, os.path.join(AFL_PATH,
                                                     TestModuleName))
            CopyFile(TestModuleBinPath,
                     os.path.join(AFL_PATH,
                                  TestModuleName.replace('.exe', '.pdb')))
            AFL_CMD = "afl-fuzz.exe -i {} ".format(InputPath) + \
                "-o {}".format(OutputPath) + \
                " -D %DRIO_PATH%\\bin32 -t 20000 -- -coverage_module " + \
                "{}".format(TestModuleName) + \
                " -fuzz_iterations 1000 -target_module " + \
                "{}".format(TestModuleName) + \
                " -target_method main -nargs 2 -- " + \
                "{} @@".format(TestModuleName)
        elif "X64" in TestModuleBinPath:
            AFL_PATH = os.path.join(os.environ["AFL_PATH"], "bin64")
            # Copy xxx.exe and xxx.pdb to the same dir as winafl\bin32
            # or winafl\bin64
            CopyFile(TestModuleBinPath, os.path.join(AFL_PATH, TestModuleName))
            CopyFile(TestModuleBinPath,
                     os.path.join(AFL_PATH,
                                  TestModuleName.replace('.exe', '.pdb')))
            AFL_CMD = "afl-fuzz.exe -i {} ".format(InputPath) + \
                "-o {} ".format(OutputPath) + \
                "-D %DRIO_PATH%\\bin64 -t 20000 -- -coverage_module " + \
                "{}".format(TestModuleName) + \
                " -fuzz_iterations 1000 -target_module " + \
                "{}".format(TestModuleName) + \
                " -target_method main -nargs 2 -- " + \
                "{} @@".format(TestModuleName)
    elif SysType == "Linux":
        AFL_PATH = os.environ["AFL_PATH"]
        AFL_CMD = []
        AFL_CMD.append(AFL_PATH+"/afl-fuzz")
        AFL_CMD.append("-i")
        AFL_CMD.append("{}".format(InputPath))
        AFL_CMD.append("-o")
        AFL_CMD.append("{}".format(OutputPath))
        AFL_CMD.append("{}".format(TestModuleBinPath))
        AFL_CMD.append("@@")
    print("Start run AFL test:")
    if SysType == "Windows":
        print(AFL_CMD)
        ExecCmd = 'start cmd /k "cd/d {} && {}"'.format(AFL_PATH, AFL_CMD)
    elif SysType == "Linux":
        if CommandLineMode == 'rawcommand':
            print(AFL_CMD)
            ExecCmd = AFL_CMD
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
            runCommand = ' '.join(AFL_CMD)
            print('Run this command to initiate the fuzzer: '
                  '{}'.format(runCommand))


def main():
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
                        help="Test output path for AFL.")
    parser.add_argument("-c", "--commandline", dest="CommandLine",
                        choices=['rawcommand', 'manual'],
                        default='rawcommand',
                        help="This specifies how the fuzzer is "
                        "initiated from command-line for Linux-based "
                        "distributions. Specify either: "
                        "'rawcommand' or 'manual'. Using 'rawcommand' "
                        "will directly initiate "
                        "the AFL fuzzer after building the test module and is "
                        "recommended for automated approaches. Alternatively, "
                        "'manual' may be used to simply build the test case "
                        "and print out a command that the user can run to "
                        "start the fuzzer (this will preserve the fuzzing "
                        "display for AFL.")
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
            print("ModuleFile path: {}".format(args.ModuleFile) +
                  " should start with " +
                  "{}.".format(HBFA_PATH))
            os._exit(0)
    elif not os.path.exists(os.path.join(HBFA_PATH, args.ModuleFile)):
        print("ModuleFile path: {}".format(args.ModuleFile) +
              " does not exist or is not in the relative path for HBFA")
        os._exit(0)
    else:
        ModuleFilePath = args.ModuleFile

    if not args.InputSeed:
        print("InputSeed path should be set once by command -i INPUTSEED, "
              "--input=INPUTSEED.")
        os._exit(0)
    elif not os.path.exists(args.InputSeed):
        print("InputSeed path: {}".format(os.path.abspath(args.InputSeed)) +
              " does not exist")
        os._exit(0)
    else:
        InputSeedPath = args.InputSeed

    if not args.Output:
        print("OutputSeed path should be set once by command -o OUTPUT, "
              "--output=OUTPUT.")
        os._exit(0)
    else:
        OutputSeedPath = args.Output
        if not os.path.isabs(OutputSeedPath):
            OutputSeedPath = os.path.join(os.getcwd(), OutputSeedPath)
        if not os.path.exists(OutputSeedPath):
            try:
                os.makedirs(OutputSeedPath)
            except Exception as err:
                print(err)
        elif os.path.exists(os.path.join(OutputSeedPath, 'fuzzer_stats')):
            print("OutputSeedPath:{}".format(OutputSeedPath) +
                  " is already exists, please change another directory.")
            os._exit(0)

    if (SysType == 'Linux'):
        global CommandLineMode
        CommandLineMode = args.CommandLine

    TestModuleBinPath = Build(TargetArch, BuildTarget, ModuleFilePath)
    RunAFL(TestModuleBinPath, InputSeedPath, OutputSeedPath)


if __name__ == "__main__":
    main()
