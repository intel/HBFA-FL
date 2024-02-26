# @file
# Transfer report.log file into HTML file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#


import os
import datetime


class GenSummaryReport(object):
    def __init__(self, execfile, outputPath, methods, reportType, failNum,
                 totalNum, execTime):
        self.__testCaseName = os.path.basename(execfile)
        self.__totalNum = totalNum
        self.__failNum = failNum
        self.__execTime = str(execTime).split(':')[0] + ' hrs, ' + \
            str(execTime).split(':')[1] + ' mins, ' + \
            str(execTime).split(':')[2] + ' secs'
        self.__methods = methods
        self.__reportType = reportType
        self.__oldPath = os.path.dirname(os.path.realpath(__file__))
        self.__oldPathFile = os.path.join(self.__oldPath,
                                          "IndexSummaryTemplate.html")
        self.__newSanitizerPathFile = \
            os.path.join(outputPath, "SanitizerSummaryReport.html")
        self.__newGdbPathFile = \
            os.path.join(outputPath, "GdbSummaryReport.html")
        self.__debugSanitizerReportlink = "IndexSanitizer.html"
        self.__debugGdbReportlink = "IndexGdb.html"
        self.__crashreportlink = "IndexCrashes.html"
        self.__hangreportlink = "IndexHangs.html"

    def __GetContentTable(self):
        file = open(self.__oldPathFile, "r")
        lines = file.readlines()
        file.close()
        for i, line in enumerate(lines):
            if "<!--PARTTWO_TABLE_KICKOFF-->" in line:
                self.__content_start = i
            if '<!--PARTTWO_TABLE_END-->' in line:
                self.__content_end = i
        return lines[self.__content_start + 1: self.__content_end]

    def __InsertContent(self, insertElem, newreportPath):
        if self.__methods == 'afl':
            testMethod = 'AFL'
        else:
            testMethod = self.__methods.capitalize()

        file = open(self.__oldPathFile, "r")
        fileData = file.readlines()
        file.close()
        date = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        for i, line in enumerate(fileData):
            if '$Testcase' in line:
                line = line.replace('$Testcase', self.__testCaseName)
                fileData[i] = line
            if '$TestMethod' in line:
                line = line.replace('$TestMethod', testMethod)
                fileData[i] = line
            if '$Date' in line:
                line = line.replace(line,
                                    '<td class="DateValue">%s</td>' % date)
                fileData[i] = line
                break
        fileData[self.__content_start:self.__content_end] = insertElem

        s = ''.join(fileData)
        file = open(newreportPath, "w")
        file.write(s)
        file.close()

    def __AddContentList(self, itemName, value, link):
        contents = self.__GetContentTable()
        new_list = []
        for line in contents:
            if '$Item' in line:
                line = line.replace('$Item', itemName)
            if '$Value' in line:
                line = line.replace('$Value', str(value))
            if '$Link' in line:
                if link != '':
                    line = line.replace('$Link', link)
                else:
                    line = line.replace('<a href="$Link">', '').replace('</a>',
                                                                        '')
            new_list.append(line)
        return new_list

    def __GenAFLGdbSumReport(self):
        totalList = self.__AddContentList('Total Case Numbers:',
                                          self.__totalNum, '')
        crashesList = self.__AddContentList('Crashes Case Numbers:',
                                            self.__failNum[0],
                                            self.__crashreportlink)
        hangsList = self.__AddContentList('Hangs Case Numbers:  ',
                                          self.__failNum[1],
                                          self.__hangreportlink)
        execTImeList = self.__AddContentList('Execution Time:  ',
                                             self.__execTime, '')
        contentList = totalList + crashesList + hangsList + execTImeList
        self.__InsertContent(''.join(contentList), self.__newGdbPathFile)

    def __GenOtherSumReport(self):
        totalList = self.__AddContentList('Total Case Numbers:  ',
                                          self.__totalNum, '')
        if self.__reportType == 'gdb':
            failList = self.__AddContentList('Failure Type Numbers:  ',
                                             self.__failNum,
                                             self.__debugGdbReportlink)
            execTImeList = self.__AddContentList('Execution Time:  ',
                                                 self.__execTime, '')
            contentList = totalList + failList + execTImeList
            self.__InsertContent(''.join(contentList), self.__newGdbPathFile)
        elif self.__reportType == 'sanitizer':
            failList = self.__AddContentList('Failure Type Numbers:  ',
                                             self.__failNum,
                                             self.__debugSanitizerReportlink)
            execTImeList = self.__AddContentList('Execution Time:  ',
                                                 self.__execTime, '')
            contentList = totalList + failList + execTImeList
            self.__InsertContent(''.join(contentList),
                                 self.__newSanitizerPathFile)

    def GenSumReport(self):
        if self.__methods == 'afl' and self.__reportType == 'gdb':
            self.__GenAFLGdbSumReport()
        else:
            self.__GenOtherSumReport()
