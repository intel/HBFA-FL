# @file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import shutil
import sys
import re

__copyright__ = ''
__version__ = '0.11'
__prog__ = 'HBFAEnvSetup.py'


def HelpInfo():
    print("Usage: %s [Options]\n" % __prog__)
    print("Create Conf specified for HBFA.\n")
    print("Options: ")
    print("  --help, -h, -?        Print this help screen and exit.\n")
    print("  --reconfig            "
          "Overwrite the UefiHostFuzzTestPkg/Conf/*.txt")
    print("                        files with the template "
          "files from the BaseTools/Conf directory.\n")
    print("Please note: This script must be put under HBFA/UefiHostTestTools,"
          " so the Conf can be generated.")


if len(sys.argv) == 2:
    if sys.argv[1] == '-h' or sys.argv[1] == '-?' or \
       sys.argv[1] == '--help' or sys.argv[1] == 'help':
        HelpInfo()
        os._exit(0)

# EDK_TOOLS_PATH
if 'EDK_TOOLS_PATH' in os.environ:
    edk_tools_path = os.environ['EDK_TOOLS_PATH']
    if not os.path.exists(edk_tools_path):
        print("EDK_TOOLS_PATH is incorrect, cannot find this folder.")
        os._exit(0)
else:
    print("EDK_TOOLS_PATH should be set, or you need to run "
          "edksetup.sh/edksetup.bat")
    os._exit(0)

# Conf template path
ConfTemplate = os.path.join(edk_tools_path, 'Conf')
if not os.path.exists(ConfTemplate):
    print("EDK_TOOLS_PATH is incorrect, cannot find Conf under this folder.")
    os._exit(0)

# HBFA package path
HBFA_PATH = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

# extend parts of tools_def.txt
ExtendDefineFile = os.path.join(HBFA_PATH, 'UefiHostFuzzTestPkg',
                                'Conf', 'tools_def.customized')


def GenerateNewToolsDef(Src, Dst):
    src = open(Src, 'r')
    lines = src.readlines()
    src.close()
    FlagVer = "GCC48_ALL_CC_FLAGS"

    for line in lines:
        pattern = re.compile(r"GCC\d{1,2}_ALL_CC_FLAGS")
        obj = pattern.findall(line)
        if len(obj) != 0:
            FlagVer = obj[0]
            break

    extendSrc = open(ExtendDefineFile, 'r')
    ExtendDefine = extendSrc.read()
    extendSrc.close()
    lines.append(ExtendDefine.replace("GCC48_ALL_CC_FLAGS", FlagVer))

    dst = open(Dst, 'w+')
    dst.writelines(lines)
    dst.close()


def InsertContentToBuildRules(Src, Dst, SectionName, InsertContent):
    src = open(Src, 'r')
    lines = src.readlines()
    src.close()
    StartPos = 0
    for i in range(len(lines)):
        if SectionName in lines[i]:
            StartPos = i + 1
            break

    lines.insert(StartPos, InsertContent)

    dst = open(Dst, 'w+')
    dst.writelines(lines)
    dst.close()


def ExtendOptionToBuildRules(Src, Dst, SectionName, TargetOption,
                             ExtendOption):
    src = open(Src, 'r')
    lines = src.readlines()
    src.close()

    Sections = SectionName if isinstance(SectionName, list) else [SectionName]
    for section in Sections:
        for i in range(len(lines)):
            if section in lines[i]:
                break
        for j in range(i, len(lines)):
            if TargetOption in lines[j]:
                OptionPos = j
                break
        lines[OptionPos] = lines[OptionPos].replace('>', ', ' + ExtendOption
                                                    + '>')

    dst = open(Dst, 'w+')
    dst.writelines(lines)
    dst.close()


def GenerateConfCustomized(ConfSrc, ConfPath, ReConfig):
    if not os.path.exists(ConfPath):
        os.makedirs(ConfPath)
    if (not os.path.exists(os.path.join(ConfPath, 'target.txt'))) or ReConfig:
        shutil.copy(os.path.join(ConfSrc, 'target.template'),
                    os.path.join(ConfPath, 'target.txt'))
    if (not os.path.exists(os.path.join(ConfPath, 'build_rule.txt'))) \
            or ReConfig:
        SectionName = '[Static-Library-File.USER_DEFINED]'
        InsertContent = '''    <Command.KLEE>
        echo $(STATIC_LIBRARY_FILES_LIST)
        python $(SCRIPT_PATH) ${DLINK_FLAGS} -t $(DLINK) -d $(OUTPUT_DIR)

'''
        InsertContentToBuildRules(os.path.join(ConfSrc, 'build_rule.template'),
                                  os.path.join(ConfPath, 'build_rule.txt'),
                                  SectionName, InsertContent)
        SectionNames = ['[Object-File]', '[Static-Library-File]',
                        '[Static-Library-File.USER_DEFINED, '
                        'Static-Library-File.HOST_APPLICATION]',
                        '[Dynamic-Library-File]',
                        '[Hii-Binary-Package.UEFI_HII]']
        TargetOption = 'Command.MSFT'
        ExtendOption = 'Command.CLANGWIN'
        ExtendOptionToBuildRules(os.path.join(ConfPath, 'build_rule.txt'),
                                 os.path.join(ConfPath, 'build_rule.txt'),
                                 SectionNames, TargetOption, ExtendOption)
    if (not os.path.exists(os.path.join(ConfPath, 'tools_def.txt'))) \
            or ReConfig:
        GenerateNewToolsDef(os.path.join(ConfSrc, 'tools_def.template'),
                            os.path.join(ConfPath, 'tools_def.txt'))


def main(ReConfig):
    GenerateConfCustomized(ConfTemplate,
                           os.path.join(HBFA_PATH, 'UefiHostFuzzTestPkg',
                                        'Conf'), ReConfig)


if __name__ == "__main__":
    if len(sys.argv) == 2:
        if sys.argv[1] == '--reconfig':
            main(ReConfig=True)
    else:
        main(ReConfig=False)
