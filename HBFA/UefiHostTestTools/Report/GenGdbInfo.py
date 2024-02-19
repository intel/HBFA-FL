# @file
# Transfer report.log file into HTML file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import subprocess
import time
import sys
import re
try:
    import ConfigParser as ConfigParser
except Exception:
    import configparser as ConfigParser

# python version
python_version = sys.version_info[0]


class GenGdbInfo(object):
    def __init__(self, execFile, cfgPath):
        self.__execFile = execFile
        self.__cfgPath = cfgPath
        self.__logPath = os.path.splitext(cfgPath)[0] + ".log"
        self.__infoList = []

    def __CompareList(self, list1, list2):
        """
        :param list1: []
        :param list2: []
        """
        if len(list1) != len(list2):
            return False
        for index in range(len(list1)):
            if list1[index] != list2[index]:
                return False
        return True

    def __GenerateCommand(self, file):
        """
        :param file: str
        """
        Template = "timeout 0.5s \'<ExecPath>\' \'<File>\'; " + \
            "if [ $? -eq 124 ]; then cp \'<ExecPath>\' /tmp/uefi_hang && " + \
            "( gdb -batch -ex run -ex bt --args /tmp/uefi_hang \'<File>\'" + \
            " & ) && sleep 0.5 && pkill uefi_hang && sleep 0.5 && " + \
            "rm -rf /tmp/uefi_hang; else (gdb -batch -ex run -ex bt " + \
            "--args \'<ExecPath>\' \'<File>\'); fi;"
        CommandLine = \
            Template.replace("<ExecPath>",
                             self.__execFile
                             ).replace("<File>", file
                                       ).replace("<ExecName>",
                                                 os.path.basename(
                                                     self.__execFile))
        return CommandLine

    def __CallCommand(self, CommandLine):
        """
        :param CommandLine: str
        """
        Cm = subprocess.Popen(CommandLine,
                              stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              shell=False)
        msg = list(Cm.communicate())
        return msg if python_version == 2 else \
            (msg[0].decode() if msg[0] else msg[0],
             msg[1].decode() if msg[1] else msg[1])

    def __GetKeyInfo(self, oriInfo):
        """
        :param oriInfo: output from subprocess ('','')
        """
        if "No stack" in oriInfo[-1]:
            InfoList = oriInfo[-1].split('\n')
            InfoList = [fn for fn in InfoList if fn]
        else:
            InfoList = oriInfo[0].split('\n')
            InfoList = [fn for fn in InfoList if fn]

            DetailInfo = []
            for index in range(len(InfoList)):
                if "Program" in InfoList[index]:
                    DetailInfo.append(InfoList[index])
                if "#" in InfoList[index]:
                    if " at " in InfoList[index]:
                        patten = re.compile(r'/.*.c')
                        obj = patten.findall(InfoList[index])
                        if len(obj) != 0:
                            DetailInfo.append(obj[0] +
                                              InfoList[index].split(obj[0])[-1]
                                              )
                    elif " from " in InfoList[index]:
                        DetailInfo.append(InfoList[index].split('from ')[-1])
                    else:
                        DetailInfo.append(InfoList[index].split(' in ')[-1])
            InfoList = DetailInfo
        return InfoList

    def __FilterInfoToCfg(self, newInfo, filterSwitch):
        """
        :param newInfo: [seedName, InfoList]
        :param filterSwitch: True False
        """
        seedName = newInfo[0]
        InfoList = newInfo[-1]
        newStack = self.__GetKeyInfo(InfoList)
        configPath = self.__cfgPath

        cfg = ConfigParser.ConfigParser()
        cfg.read(configPath)
        searchFlag = False
        if filterSwitch:
            if 'no stack' in newStack[-1].lower():
                searchFlag = True
            else:
                for section in cfg.sections():
                    oldStack = cfg.get(section, 'stackinfo')
                    oldStack = self.__GetKeyInfo([oldStack])
                    if self.__CompareList(newStack, oldStack):
                        searchFlag = True
                        cfg.set(section, 'TotalSeedsNum',
                                int(cfg.get(section, 'TotalSeedsNum')) + 1)
                        break
        if not searchFlag:
            cfg.add_section(seedName)
            if 'no stack' in newStack[-1].lower():
                cfg.set(seedName, 'stackinfo', InfoList[-1])
            else:
                cfg.set(seedName, 'stackinfo', InfoList[0])
            cfg.set(seedName, 'TotalSeedsNum', 1)
        self.__infoList = []
        for section in cfg.sections():
            self.__infoList.append((time.asctime(
                time.localtime(os.path.getmtime(section))),
                section,
                cfg.get(section, 'stackinfo')))
        cfg.write(open(configPath, 'w'))

    def __ClearRedundantInfo(self, oriInfo):
        """
        :param oriInfo: output from subprocess ('','')
        """
        InfoList = oriInfo[0].split('\n')
        InfoList = [fn for fn in InfoList if fn]
        Pos = 0
        for index in range(len(InfoList)):
            if "Program" in InfoList[index]:
                Pos = index
                break
        if Pos > 100:
            InfoList = InfoList[Pos-100:]
        oriInfoNew = ('\n'.join(InfoList), oriInfo[-1])
        return oriInfoNew

    def __UpdateInfoList(self, newInputList, filterSwitch):
        """
        :param newInputList: []
        :param filterSwitch: True False
        """
        for newInput in newInputList:
            if os.path.isfile(newInput):
                errorInfo = \
                    self.__CallCommand(self.__GenerateCommand(newInput))
                errorInfo = self.__ClearRedundantInfo(errorInfo)
                self.__FilterInfoToCfg([newInput, errorInfo], filterSwitch)
                if filterSwitch:
                    self.__GenereteInfoFile(
                        [(time.asctime(time.localtime(
                            os.path.getmtime(newInput))),
                          newInput,
                          errorInfo)],
                        os.path.join(os.path.dirname(newInput),
                                     os.path.basename(self.__logPath)))
                else:
                    self.__GenereteInfoFile(self.__infoList, self.__logPath)

    def __GenereteInfoFile(self, infoList, infoPath):
        """
        :param infoList: []
        :param infoPath: str
        """
        info = "Processing \'%s\' :\n\n" % (self.__execFile)
        for index in range(len(infoList)):
            info += "------------------------------------------------" + \
                "------------\n"
            info += "%s\n\'%s/%s/%s\' :\n" % \
                    (infoList[index][0],
                     infoList[index][1].replace('\\', '/').split('/')[-3],
                     infoList[index][1].replace('\\', '/').split('/')[-2],
                     infoList[index][1].replace('\\', '/').split('/')[-1])
            for msg in infoList[index][2]:
                info += "%s" % msg
            info += '\n\n'
        output = open(infoPath, 'w+')
        output.writelines(info)
        output.close()

    def Run(self, newInput, filterSwitch):
        """
        :param newInput: []
        :param filterSwitch: True False
        """
        self.__UpdateInfoList(newInput, filterSwitch)
