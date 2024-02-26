#!/usr/bin/env python3
# @file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import stat
import subprocess
import shutil
import platform
import argparse
from GenLLVMReport import GenLLVMReport

__prog__ = 'GenCodeCoverage.py'
__copyright__ = 'Copyright (c) 2019, Intel Corporation. All rights reserved.'
__version__ = '{} Version {}'.format(__prog__, '0.11 ')

WORK_DIR = os.getcwd()

# Get System type info
SysType = platform.system()

# HBFA package path
HBFA_PATH = os.path.dirname(os.path.dirname(os.path.dirname(
    os.path.realpath(__file__))))

# WORKSPACE
workspace = ''

# Check EDKII BUILD WORKSPACE whether be set in system environment variable
if 'WORKSPACE' not in os.environ:
    print("Please set system environment variable 'WORKSPACE' before run "
          "this script.")
    os._exit(0)
workspace = os.environ['WORKSPACE']


def CheckTestEnv():
    if SysType == "Windows" and "DRIO_PATH" not in os.environ:
        print("Please set DRIO_PATH in system environment variables.")
        os._exit(0)


def delete_gcda_file(TestModuleBinPath):
    for root, dirs, files in os.walk(os.path.dirname(TestModuleBinPath)):
        for file in files:
            if file.endswith('.gcda'):
                file_path = os.path.join(root, file)
                try:
                    os.remove(file_path)
                except Exception as err:
                    print(err)


def HasIni(path):
    if os.listdir(path) == []:
        return False

    for file in os.listdir(path):
        if file.endswith(".ini"):
            return True

    return False


def CreateGcovTool(path):
    with open(path, 'w') as fd:
        fd.write('#!/bin/bash\nexport PATH=$CLANG_PATH:$PATH\nexec '
                 'llvm-cov gcov "$@"')
    os.chmod(path, stat.S_IEXEC)


def Run_All_Seeds(TestModuleBinPath, SeedPath, TestIniPath):
    TestModuleBinFolder = os.path.dirname(TestModuleBinPath)
    if SysType == "Windows":
        LogDir = os.path.join(TestModuleBinFolder, "temp", "log")
        if os.path.exists(os.path.dirname(LogDir)):
            shutil.rmtree(os.path.dirname(LogDir))
        os.makedirs(LogDir)
        for file in os.listdir(SeedPath):
            SeedFilePath = os.path.join(SeedPath, file)
            if os.path.isfile(SeedFilePath):
                if "IA32" in TestModuleBinPath:
                    cmd = r"cd {} && %DRIO_PATH%\bin32".format(LogDir) + \
                          r"\drrun.exe -c %DRIO_PATH%\tools\lib32\release" + \
                          r"\drcov.dll " \
                          r"-- {} {}".format(TestModuleBinPath, SeedFilePath)
                elif "X64" in TestModuleBinPath:
                    cmd = r"cd {} && %DRIO_PATH%\bin64".format(LogDir) + \
                          r"\drrun.exe -c %DRIO_PATH%\tools\lib64" + \
                          r"\release\drcov.dll -- " + \
                          r"{} {}".format(TestModuleBinPath, SeedFilePath)
                try:
                    subprocess.run(cmd.split(' '), shell=False, check=True)
                except subprocess.CalledProcessError as err:
                    print(err)
    elif SysType == "Linux":
        if TestIniPath != "":
            for file in os.listdir(SeedPath):
                SeedFilePath = os.path.join(SeedPath, file)
                if os.path.isfile(SeedFilePath):
                    for IniFile in os.listdir(TestIniPath):
                        IniFilePath = os.path.join(TestIniPath, IniFile)
                        cmd = TestModuleBinPath + ' ' + SeedFilePath + ' ' \
                            + IniFilePath
                        try:
                            subprocess.run(cmd.split(' '), shell=False,
                                           check=True)
                        except subprocess.CalledProcessError as err:
                            print(err)
        else:
            for file in os.listdir(SeedPath):
                SeedFilePath = os.path.join(SeedPath, file)
                if os.path.isfile(SeedFilePath):
                    cmd = TestModuleBinPath + ' ' + SeedFilePath
                    try:
                        subprocess.run(cmd.split(' '), shell=False,
                                       check=True)
                    except subprocess.CalledProcessError as err:
                        print(err)


def GenCodeCoverage(TestModuleBinPath, ReportPath):
    TestModuleBinFolder = os.path.dirname(TestModuleBinPath)
    if SysType == "Windows":
        LogDir = os.path.join(TestModuleBinFolder, "temp", "log")
        TempDir = os.path.join(TestModuleBinFolder, "temp")
        if "IA32" in TestModuleBinPath:
            run_command = \
                r"cd {} && %DRIO_PATH%\tools".format(TempDir) + \
                r"\bin32\drcov2lcov.exe -dir " + \
                r"{} -src_filter ".format(LogDir) + \
                r"{}".format(workspace.lower())
            try:
                subprocess.run(run_command.split(' '), shell=False,
                               check=True)
            except subprocess.CalledProcessError as err:
                print(err)
            run_command = \
                r"cd {} && perl ".format(TempDir) + \
                r"%DRIO_PATH%\tools\bin32\genhtml " + \
                r"coverage.info"
            try:
                subprocess.run(run_command.split(' '),
                               shell=False, check=True)
            except subprocess.CalledProcessError as err:
                print(err)
        elif "X64" in TestModuleBinPath:
            run_command = \
                r"cd {} && %DRIO_PATH%\tools".format(TempDir) + \
                r"\bin64\drcov2lcov.exe -dir " + \
                r"{} -src_filter ".format(LogDir) + \
                r"{}".format(workspace.lower())
            try:
                subprocess.run(run_command.split(' '), shell=False,
                               check=True)
            except subprocess.CalledProcessError as err:
                print(err)
            run_command = \
                r"cd {}".format(TempDir) + \
                r" && perl %DRIO_PATH%\tools" + \
                r"\bin64\genhtml coverage.info"
            try:
                subprocess.run(run_command.split(' '), shell=False,
                               check=True)
            except subprocess.CalledProcessError as err:
                print(err)
        if os.path.exists(ReportPath):
            shutil.rmtree(ReportPath)
        shutil.copytree(os.path.join(TestModuleBinFolder, "temp"), ReportPath)
    elif SysType == "Linux":
        try:
            if "LIBFUZZER" in TestModuleBinFolder:
                genLLVMReports = GenLLVMReport(TestModuleBinPath, ReportPath)
                genLLVMReports.gen_reports()
                print("Please view code coverage report in "
                      "{}".format(ReportPath))
                return
            elif "CLANG8" in TestModuleBinFolder:
                GcovToolPath = os.path.join(WORK_DIR, 'llvm-gcov.sh')
                if not os.path.exists(GcovToolPath):
                    CreateGcovTool(GcovToolPath)
                run_command = \
                    "lcov --capture --directory " + \
                    "{} --gcov-tool ".format(TestModuleBinFolder) + \
                    "{} ".format(GcovToolPath) + \
                    "--output-file coverage.info"
                try:
                    subprocess.run(run_command.split(' '), shell=False,
                                   check=True)
                except subprocess.CalledProcessError as err:
                    print(err)
                    os.remove(GcovToolPath)
                os.remove(GcovToolPath)
            else:
                run_command = \
                    "lcov --capture --directory " + \
                    "{} ".format(TestModuleBinFolder) + \
                    "--output-file coverage.info"
                try:
                    subprocess.run(run_command.split(' '), shell=False,
                                   check=True)
                except subprocess.CalledProcessError as err:
                    print(err)
            run_command = \
                "lcov -r coverage.info '*UefiHostTestPkg*' " + \
                "--output-file " + \
                "coverage.info"
            try:
                subprocess.run(run_command.split(' '), shell=False,
                               check=True)
            except subprocess.CalledProcessError as err:
                print(err)
        except Exception as err:
            print(err)
        try:
            run_command = \
                "genhtml coverage.info --output-directory " + \
                "{}".format(ReportPath)
            try:
                subprocess.run(run_command.split(' '), shell=False,
                               check=True)
            except subprocess.CalledProcessError as err:
                print(err)
                os.remove('coverage.info')
            os.remove('coverage.info')
        except Exception as err:
            print(err)

    print("Please view code coverage report in {}".format(ReportPath))
    return


# Parse command line options
def MyOptionParser():
    Parser = argparse.ArgumentParser()
    Parser.add_argument("-e", "--execbinary", dest="ModuleBin",
                        help="Test module binary file name.")
    Parser.add_argument("-d", "--dir", dest="SeedPath",
                        help="Test output seed directory path.")
    Parser.add_argument("-t", "--testini", dest="TestIniPath",
                        help="Test ini files path for ErrorInjection, "
                        "only for ErrorInjection.")
    Parser.add_argument("-r", "--report", dest="ReportPath",
                        help="Generated code coverage report path.")

    args = Parser.parse_args(sys.argv[1:])
    return args


def main():
    Option = MyOptionParser()

    CheckTestEnv()

    if not Option.ModuleBin:
        print("Test module binary path should be set once by command -e "
              "MODULEBIN, --execbinary=MODULEBIN.")
        os._exit(0)
    elif not os.path.exists(Option.ModuleBin):
        print("Test module binary path: "
              "{} does not exist.".format(os.path.abspath(Option.ModuleBin)))
        os._exit(0)
    else:
        ModuleBinPath = Option.ModuleBin

    if not Option.ReportPath:
        ReportPath = os.path.join(WORK_DIR, 'CodeCoverageReport')
    elif os.path.isabs(Option.ReportPath):
        ReportPath = os.path.join(Option.ReportPath, 'CodeCoverageReport')
    elif not os.path.isabs(Option.ReportPath):
        ReportPath = os.path.join(WORK_DIR, Option.ReportPath,
                                  'CodeCoverageReport')
    else:
        print("Please check the input report path.")
        os._exit(0)

    if "CLANG8" not in ModuleBinPath:
        if not Option.SeedPath:
            print("Test output seed directory path should be set once by "
                  "command -d SEEDPATH, --dir=SEEDPATH.")
            os._exit(0)
        elif not os.path.exists(Option.SeedPath):
            print("Test output seed directory path:" +
                  "{} ".format(os.path.abspath(Option.SeedPath)) +
                  "does not exist.")
            os._exit(0)
        else:
            OutputSeedPath = Option.SeedPath

        if not Option.TestIniPath:
            TestIniPath = ""
        elif not os.path.exists(Option.TestIniPath):
            print("Test ini path:" +
                  "{}".format(os.path.abspath(Option.TestIniPath)) +
                  " does not exist.")
            os._exit(0)
        elif not HasIni(Option.TestIniPath):
            print("No .ini file in {}".format(Option.TestIniPath))
            os._exit(0)
        else:
            TestIniPath = Option.TestIniPath

        if SysType == "Linux":
            # delete .gcda files before collect code coverage
            delete_gcda_file(ModuleBinPath)

        # Run binary with all seeds
        Run_All_Seeds(ModuleBinPath, OutputSeedPath, TestIniPath)

    # Generate Code coverage report
    GenCodeCoverage(ModuleBinPath, ReportPath)


if __name__ == "__main__":
    main()
