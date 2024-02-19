# @file
# Transfer report.log file into HTML file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#


import datetime
import os
import re

try:
    import ConfigParser as ConfigParser
except Exception:
    import configparser as ConfigParser

# report template folder
TemplatePath = os.path.dirname(os.path.realpath(__file__))


class GenGdbHtmlReport(object):
    def __init__(self, cfgPath, outputPath, Item):
        self.__cfgPath = cfgPath
        self.__Item = Item
        self.__oldPathFile = os.path.join(TemplatePath, "IndexTemplate.html")
        self.__newPathFile = \
            os.path.join(outputPath,
                         "IndexTemplate.html".replace(
                             'Template', self.__Item.capitalize()
                             if self.__Item else 'Gdb'.capitalize()))

    def __Str2HyperLink(self, oldStr, link):
        return oldStr.replace(link, '<a href=%s>%s</a>' % (link, link)) \
            if len(link.split('/' if '/' in link else '\\')) > 1 else oldStr

    def __ParseFailure(self, oriInfo):
        filePath = "--"
        lineNum = "--"
        errorMessage = "--"

        stackInfo = oriInfo.split('\n')
        InfoList = [fn for fn in stackInfo if fn]

        startPos = 0
        for index in range(len(InfoList)):
            if "Program" in InfoList[index]:
                startPos = index
                errorMessage = InfoList[startPos]
                break

        for index in range(startPos, len(InfoList)):
            Info = InfoList[index]
            if "#0" not in Info:
                if " at " in Info:
                    patten = re.compile(r'/.*.c')
                    obj = patten.findall(Info)
                    if len(obj) != 0:
                        lineNum = \
                            Info.split(':')[-1] if Info.split(':')[-1] \
                            else "--"
                        filePath = obj[0]
                        break
                elif " from " in Info:
                    lineNum = "--"
                    filePath = Info.split(' from ')[-1]
                    break
                elif " in " in Info:
                    lineNum = "--"
                    filePath = Info.split(' in ')[-1]
                    break

        for index in range(len(InfoList)):
            if " at " in InfoList[index]:
                patten = re.compile(r'/.*.c')
                obj = patten.findall(InfoList[index])
                if len(obj) != 0:
                    InfoList[index] = \
                        self.__Str2HyperLink(InfoList[index], obj[0])
            elif " from " in InfoList[index]:
                InfoList[index] = \
                    self.__Str2HyperLink(InfoList[index],
                                         InfoList[index].split(' from ')[-1])
            elif " in " in InfoList[index]:
                InfoList[index] = \
                    self.__Str2HyperLink(InfoList[index],
                                         InfoList[index].split(' in ')[-1])
            InfoList[index] = InfoList[index] + '<br><br>'

        stackInfo = "".join(InfoList) if InfoList else "--"
        return filePath, lineNum, errorMessage, stackInfo

    def __ContentTable(self):
        file = open(self.__oldPathFile, "r")
        lines = file.readlines()
        file.close()
        for i, line in enumerate(lines):
            if "<!--PARTTWO_TABLE_KICKOFF-->" in line:
                self.__content_start = i
            if '<!--PARTTWO_TABLE_END-->' in line:
                self.__content_end = i
        return lines[self.__content_start + 1: self.__content_end]

    def __InsertContent(self):
        cfg = ConfigParser.ConfigParser()
        cfg.read(self.__cfgPath)
        sections = cfg.sections()

        # if not self.__Item:
        #     for index, section in enumerate(sections):
        #         if "no stack" in cfg.get(section, "stackinfo").lower():
        #             sections.pop(index)
        #             sections.append(section)

        failNum = len(sections)

        file = open(self.__oldPathFile, "r")
        fileData = file.readlines()
        file.close()
        date = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        itemDetail = "crashes'" if self.__Item.lower() == 'crashes' \
            else ("hangs'" if self.__Item.lower() == 'hangs' else "failures'")
        itemNum = "Crash" if self.__Item.lower() == 'crashes' \
            else ("Hang" if self.__Item.lower() == 'hangs' else 'Failure')

        if (self.__Item.lower() in ['crashes', 'hangs']):
            testMethod = "AFL"
        elif (self.__Item):
            testMethod = "libFuzzer"
        else:
            print("[!] Unexpected testMethod")
            exit(1)
        for i, line in enumerate(fileData):
            if '$itemDetail' in fileData[i]:
                fileData[i] = fileData[i].replace('$itemDetail', itemDetail)
            if '$testMethod' in fileData[i]:
                fileData[i] = fileData[i].replace('$testMethod', testMethod)
            if '$itemNum' in fileData[i]:
                fileData[i] = fileData[i].replace('$itemNum', itemNum)
            if '$Date' in fileData[i]:
                fileData[i] = fileData[i].replace('$Date', date)
            if '$failNum' in fileData[i]:
                fileData[i] = fileData[i].replace('$failNum', str(failNum))
                break

        contentTemplate = self.__ContentTable()
        contentList = []

        for section in sections:
            info = cfg.get(section, 'stackinfo')
            TotalSeedNum = cfg.get(section, 'totalseedsnum')
            failType = section
            filePath, lineNum, errorMessage, stackInfo = \
                self.__ParseFailure(info)
            for line in contentTemplate:
                if '$failure' in line:
                    line = \
                        line.replace('$failure',
                                     os.path.basename(failType).split(',')[0]
                                     if self.__Item
                                     else ('--' if errorMessage == '--' else
                                           os.path.dirname(failType.split(
                                             os.path.dirname(
                                               os.path.dirname(
                                                 os.path.dirname(failType)))
                                             + ('/' if '/' in failType
                                                else '\\'))[-1])))
                if '$seedsPath' in line:
                    line = line.replace('$seedsPath',
                                        os.path.dirname(failType))
                if '$seedsNum' in line:
                    line = line.replace('$seedsNum', TotalSeedNum)
                if '$filePath' in line:
                    if len(filePath.split('/' if '/' in filePath else '\\')) \
                          > 1:
                        line = \
                            line.replace('$filePath', filePath).replace(
                                '$fileName', os.path.basename(filePath))
                    else:
                        line = line.replace(
                            '<a href="$filePath">$fileName</a>',
                            os.path.basename(filePath))
                if '$lineNum' in line:
                    line = line.replace('$lineNum', lineNum)
                if '$error' in line:
                    line = line.replace('$error', errorMessage)
                if '$stack' in line:
                    line = line.replace('$stack', stackInfo)
                contentList.append(line)
        fileData[self.__content_start:self.__content_end] = contentList

        s = ''.join(fileData)
        file = open(self.__newPathFile, "w")
        file.write(s)
        file.close()

    def GenerateHtml(self):
        self.__InsertContent()
