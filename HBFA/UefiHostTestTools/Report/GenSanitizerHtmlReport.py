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


class GenSanitizerHtmlReport(object):
    def __init__(self, reportLogPath, item, outputPath):
        self.__reportLogPath = reportLogPath
        self.__oldPath = os.path.dirname(os.path.realpath(__file__))
        self.__classification = item
        if (
            self.__classification == 'crashes' or
            self.__classification == 'hangs'
        ):
            self.testMethod = 'AFL'
        else:
            self.testMethod = 'libFuzzer'
        self.__oldPathFile = os.path.join(self.__oldPath, "IndexTemplate.html")
        self.__newPathFile = os.path.join(outputPath, "IndexSanitizer.html")
        self.__cfg = ConfigParser.ConfigParser()
        self.__lcfg = ConfigParser.ConfigParser()
        self.__AddressSanitizerLog = os.path.join(self.__reportLogPath,
                                                  'AddressSanitizer.cfg')
        self.__LeakSanitizerLog = os.path.join(self.__reportLogPath,
                                               'LeakSanitizer.cfg')

    def __GetContentsInfo(self, contents):
        contents_list = contents.split('\n')
        error_message = '--'
        for content in contents_list:
            match1 = re.search(r'ERROR: AddressSanitizer: (\S)+', content)
            if match1:
                error_message = match1.group(0)
            elif 'ERROR: AddressSanitizer' in content:
                error_message = content.split('==')[-1]
            match2 = re.search(r'#0 0x(.)+', content)
            if match2:
                FilePath = match2.group().split(' ')[-1]
                LineNum = FilePath.replace(':\\', '\\').split(':')[1]
                FileName = \
                    os.path.basename(
                        FilePath.replace(':\\', '\\').split(':')[0])
                TmpLink = \
                    FilePath.replace(':\\', '\\').split(':')[0].split('\\')
                TmpLink[0] = TmpLink[0].replace(TmpLink[0], TmpLink[0] + ':') \
                    if len(TmpLink) > 1 else TmpLink[0]
                FileLink = '\\'.join(TmpLink)
            if 'allocated by' in content or re.search(r'Address 0x(\w)+ is '
                                                      r'located in', content):
                break
        return FileName, FileLink, LineNum, error_message

    def __GetLContentsInfo(self, contents):
        contents_list = contents.split('\n')
        error_message = '--'
        for content in contents_list:
            match1 = re.search('ERROR: LeakSanitizer: (.)+', content)
            if match1:
                error_message = match1.group(0)
            match2 = re.search(r'#0 0x(.)+', content)
            if match2:
                FilePath = match2.group().split(' ')[-1]
                LineNum = FilePath.replace(':\\', '\\').split(':')[1]
                FileName = \
                    os.path.basename(
                        FilePath.replace(':\\', '\\').split(':')[0])
                TmpLink = \
                    FilePath.replace(':\\', '\\').split(':')[0].split('\\')
                TmpLink[0] = TmpLink[0].replace(TmpLink[0], TmpLink[0] + ':') \
                    if len(TmpLink) > 1 else TmpLink[0]
                FileLink = '\\'.join(TmpLink)
                break
        return FileName, FileLink, LineNum, error_message

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

    def __InsertContent(self, insertElem, failNum):
        file = open(self.__oldPathFile, "r")
        fileData = file.readlines()
        file.close()
        date = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        for i, line in enumerate(fileData):
            if '$Date' in line:
                line = line.replace(line, '<td class="DateValue">%s</td>'
                                    % date)
                fileData[i] = line
            if '$itemDetail' in line:
                line = line.replace('$itemDetail', "failures'")
                fileData[i] = line
            if '$testMethod' in line:
                line = line.replace('$testMethod', self.testMethod)
                fileData[i] = line
            if '$itemNum' in line:
                line = line.replace('$itemNum', 'Failure Type')
                fileData[i] = line
            if '$failNum' in line:
                line = line.replace(line, '<td class="Numbervalue">%s</td>'
                                    % failNum)
                fileData[i] = line
                break
        fileData[self.__content_start:self.__content_end] = insertElem

        s = ''.join(fileData)
        file = open(self.__newPathFile, "w")
        file.write(s)
        file.close()

    def GenHtml(self):
        afailNum = 0
        lfailNum = 0
        new_list = []
        contents = self.__GetContentTable()
        if os.path.exists(self.__AddressSanitizerLog):
            self.__cfg.read(self.__AddressSanitizerLog)
            sections = self.__cfg.sections()
            for item in sections:
                amatch = re.search(r'number(\d)+_AddressSanitizer', item)
                lmatch = re.search(r'number(\d)+_LeakSanitizer', item)
                if (not amatch) and (not lmatch):
                    seedsPath = os.path.dirname(item)
                    if self.__classification != '':
                        if os.path.basename(seedsPath) in ['crashes', 'hangs']:
                            failType = os.path.join(
                                os.path.basename(seedsPath),
                                os.path.basename(item).split(',')[0])
                        else:
                            failType = os.path.basename(item)
                    else:
                        failType = \
                            os.path.basename(os.path.dirname(
                                os.path.dirname(item))) + '/' \
                            + os.path.basename(os.path.dirname(item))
                else:
                    print("[!] Unable to match Sanitizer "
                          "(not AFL or LibFuzzer output)")
                    exit(1)
                section_contents = self.__cfg.get(item, 'stackinfo')
                totalseednum = self.__cfg.get(item, 'totalseedsnum')
                afailNum = len(sections)
                if section_contents != '':
                    fileName, fileLink, lineNum, errorMessage = \
                        self.__GetContentsInfo(section_contents)
                    stack_info = section_contents.split('\n')
                    new_stack_info_list = []
                    for sline in stack_info:
                        match = re.search(r'#\d 0x(.)+', sline)
                        if match:
                            FilePath = match.group().split(' ')[-1]
                            a = ' '.join(match.group().split(' ')[:-1])
                            TmpLink = \
                                FilePath.replace(
                                    ':\\', '\\').split(
                                        ':')[0].split('\\')
                            TmpLink[0] = \
                                TmpLink[0].replace(TmpLink[0],
                                                   TmpLink[0]
                                                   + ':') \
                                if len(TmpLink) > 1 \
                                else TmpLink[0]
                            FileLink = '\\'.join(TmpLink)
                            if '(' in FileLink:
                                FileLink = FileLink.lstrip(r'\(')
                                FileLink = FileLink.rstrip(r'\)')
                            sline = sline.replace(sline, a + ('<a href=%s>'
                                                              % FileLink) +
                                                  ' ' + FilePath + '</a>')
                        sline = sline + '<br>'
                        new_stack_info_list.append(sline)
                    new_stack_info = ''.join(new_stack_info_list)
                else:
                    fileName, fileLink, lineNum, errorMessage = \
                        '--', '', '--', '--'
                    new_stack_info = 'No stack'
                contents = self.__GetContentTable()
                for line in contents:
                    if '$failure' in line:
                        line = line.replace('$failure', failType)
                    if '$filePath' in line:
                        if fileLink == '':
                            line = line.replace('<a href="$filePath">',
                                                '').replace('</a>', '')
                        else:
                            line = line.replace('$filePath', fileLink)
                    if '$seedsNum' in line:
                        line = line.replace('$seedsNum', totalseednum)
                    if '$seedsPath' in line:
                        if seedsPath == '':
                            line = line.replace('<a href="$seedsPath">',
                                                '').replace('</a>', '')
                        else:
                            line = line.replace('$seedsPath', seedsPath)
                    if '$fileName' in line:
                        line = line.replace('$fileName', fileName)
                    if '$lineNum' in line:
                        line = line.replace('$lineNum', lineNum)
                    if '$error' in line:
                        line = line.replace('$error', errorMessage)
                    if '$stack' in line:
                        line = line.replace('$stack', new_stack_info)

                    new_list.append(line)

        if os.path.exists(self.__LeakSanitizerLog):
            self.__lcfg.read(self.__LeakSanitizerLog)
            section_names = self.__lcfg.sections()
            for item in section_names:
                amatch1 = re.search(r'number(\d)+_AddressSanitizer', item)
                lmatch1 = re.search(r'number(\d)+_LeakSanitizer', item)
                if (not amatch1) and (not lmatch1):
                    lseedsPath = os.path.dirname(item)
                    if self.__classification != '':
                        if os.path.basename(lseedsPath) in ['crashes',
                                                            'hangs']:
                            lfailType = \
                                os.path.join(os.path.basename(lseedsPath),
                                             os.path.basename(item).split(',')
                                             [0])
                        else:
                            lfailType = os.path.basename(item)
                    else:
                        lfailType = \
                            os.path.basename(os.path.dirname(
                                os.path.dirname(item))) + '/' + \
                            os.path.basename(os.path.dirname(item))
                else:
                    lfailType = item
                    lseedsPath = os.path.join(self.__reportLogPath,
                                              'FailureSeeds', lfailType)

                section_contents = self.__lcfg.get(item, 'stackinfo')
                totalseednum = '--'
                lfailNum = len(section_names)
                lfileName, lfileLink, llineNum, lerrorMessage = \
                    self.__GetLContentsInfo(section_contents)
                stack_info = section_contents.split('\n')
                lnew_stack_info_list = []
                for sline in stack_info:
                    match = re.search(r'#\d 0x(.)+', sline)
                    if match:
                        FilePath = match.group().split(' ')[-1]
                        a = ' '.join(match.group().split(' ')[:-1])
                        LtmpLink = \
                            FilePath.replace(':\\',
                                             '\\').split(':')[0].split('\\')
                        LtmpLink[0] = \
                            LtmpLink[0].replace(LtmpLink[0],
                                                LtmpLink[0] + ':') \
                            if len(LtmpLink) > 1 else LtmpLink[0]
                        lfileLink = '\\'.join(LtmpLink)
                        if '(' in lfileLink:
                            lfileLink = lfileLink.lstrip(r'\(')
                            lfileLink = lfileLink.rstrip(r'\)')
                        sline = sline.replace(sline, a +
                                              ('<a href=%s>' % lfileLink)
                                              + ' ' + FilePath + '</a>')
                    sline = sline + '<br>'
                    lnew_stack_info_list.append(sline)
                new_stack_info = ''.join(lnew_stack_info_list)
                contents = self.__GetContentTable()
                for line in contents:
                    if '$failure' in line:
                        line = line.replace('$failure', lfailType)
                    if '$filePath' in line:
                        line = line.replace('$filePath', lfileLink)
                    if '$seedsNum' in line:
                        line = line.replace('$seedsNum', totalseednum)
                    if '$seedsPath' in line:
                        line = line.replace('$seedsPath', lseedsPath)
                    if '$fileName' in line:
                        line = line.replace('$fileName', lfileName)
                    if '$lineNum' in line:
                        line = line.replace('$lineNum', llineNum)
                    if '$error' in line:
                        line = line.replace('$error', lerrorMessage)
                    if '$stack' in line:
                        line = line.replace('$stack', new_stack_info)
                    new_list.append(line)

        failNum = afailNum + lfailNum
        self.__InsertContent(''.join(new_list), failNum)
