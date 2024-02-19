# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import subprocess
import sys
from shutil import which
import argparse

# python version
python_version = sys.version_info[0]


def GetObjFile(TargetPath):
    ObjFileList = []
    static_library_files = open(os.path.join(TargetPath,
                                             r'static_library_files.lst'), 'r')
    static_library_files_list = static_library_files.readlines()
    static_library_files.close()

    for file in static_library_files_list:
        object_files = open(os.path.join(os.path.dirname(file.replace(
            '.lib\n', '.lib')), r'object_files.lst'), 'r')
        object_files_list = object_files.readlines()
        object_files.close()
        for objfile in object_files_list:
            ObjFileList.append(objfile.replace('.obj\n', '.obj'))

    return ObjFileList


def VerifyCommand(command):
    # Find full path of command binary / verity it is present
    # return command with full path
    try:
        full_path_command = which(command)
    except FileNotFoundError as e:
        print(f"[!] Error finding command binary on system: {e}")
        exit(1)
    return full_path_command


def GenerateCommand(Command, Flags, InputFileList, OutputFile):
    Template = "<Command> -o <OutputFile> <Flags> <InputFileList>"
    InputFile = ' '.join(InputFileList)
    VerifiedCommand = VerifyCommand(Command)
    CommandLine = Template.replace("<Command>", VerifiedCommand).replace(
        "<Flags>", Flags).replace("<InputFileList>", InputFile).replace(
            "<OutputFile>", OutputFile)
    print(CommandLine)
    return CommandLine


def CallCommand(CommandLine):
    CommandList = [item for item in CommandLine.split(' ') if len(item) > 0]
    Cm = subprocess.Popen(CommandList,
                          stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          shell=False)
    msg = Cm.communicate()
    for m in msg:
        print(m)


if __name__ == '__main__':
    # # # Opt Parser
    parse = argparse.ArgumentParser()
    parse.add_argument("-o", dest="output",
                       help="output file name", default=None)
    parse.add_argument("-d", dest="targetpath",
                       help="target path", default=None)
    parse.add_argument("-t", dest="tool",
                       help="the tool you use", default=None)
    options = parse.parse_args(sys.argv[1:])
    if options.tool:
        if options.targetpath:
            if options.output:
                CallCommand(GenerateCommand(options.tool, "",
                                            GetObjFile(options.targetpath),
                                            options.output))
            else:
                raise Exception("Please input -o output file name")
        else:
            raise Exception("Plesase input -d target file path")
    else:
        raise Exception("Please input -t tool you use.")
