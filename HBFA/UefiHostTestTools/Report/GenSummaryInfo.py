# @file
# Transfer report.log file into HTML file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#


import os
import sys
try:
    import ConfigParser as ConfigParser
except Exception:
    import configparser as ConfigParser
import datetime
import time
import argparse


class GenSummaryInfo(object):
    def __init__(self, inputSeedsPath, debugReportPath, methods,
                 reportType, silence):
        self.__afcfg = os.path.join(debugReportPath,
                                    'AddressSanitizer.cfg')
        self.__lcfg = os.path.join(debugReportPath,
                                   'LeakSanitizer.cfg')
        self.__gdbcfg = os.path.join(debugReportPath,
                                     'HBFA.GDB.cfg')
        self.__afl_gdbCrashcfg = os.path.join(debugReportPath,
                                              'HBFA.GDB.Crashes.cfg')
        self.__afl_gdbHangcfg = os.path.join(debugReportPath,
                                             'HBFA.GDB.Hangs.cfg')
        self.__inputPath = inputSeedsPath
        self.__debugReportPath = debugReportPath
        self.__methods = methods
        self.__reportType = reportType
        self.__silence = silence

    def __GetFailedTypeNum(self, logPath):
        fail_num = 0
        if os.path.exists(logPath):
            cfg = ConfigParser.ConfigParser()
            cfg.read(logPath)
            sections = cfg.sections()
            fail_num = len(sections)
        return fail_num

    def __GetCfgSeedsNum(self, logPath):
        fail_num = 0
        if os.path.exists(logPath):
            cfg = ConfigParser.ConfigParser()
            cfg.read(logPath)
            sections = cfg.sections()
            for section in sections:
                fail_num = fail_num + int(cfg.get(section, 'totalseedsnum'))
        return fail_num

    def __GetSanitizerlNum(self, keyword, statuspath):
        num = 0
        if os.path.exists(os.path.join(statuspath, '.status')):
            with open(os.path.join(statuspath, '.status'), 'r') as f:
                for line in f:
                    if keyword in line:
                        num = int(line.split(':')[1].strip())
        return num

    def __GetSanitizerlTime(self, statuspath):
        if os.path.exists(os.path.join(statuspath, '.status')):
            with open(os.path.join(statuspath, '.status'), 'r') as f:
                for line in f:
                    if 'start_time' in line:
                        starttime = line.split(': ')[1].strip()
                        starttime = datetime.datetime.strptime(
                            starttime, '%m/%d/%Y %H:%M:%S')
                        break
        endtime = time.localtime(os.path.getmtime(os.path.join(statuspath,
                                                               '.status')))
        endtime = time.strftime('%m/%d/%Y %H:%M:%S', endtime)
        endtime = datetime.datetime.strptime(endtime, '%m/%d/%Y %H:%M:%S')
        execTime = endtime - starttime
        return execTime

    def __getAFLTotalSeeds(self):
        num = 0
        if os.path.exists(os.path.join(self.__inputPath, 'fuzzer_stats')):
            with (
                open(os.path.join(self.__inputPath, 'fuzzer_stats'), 'r') as f
            ):
                for line in f:
                    if 'execs_done' in line:
                        num = int(line.split(':')[-1].strip())
        return num

    def __GetAFLExecTime(self):
        if os.path.exists(os.path.join(self.__inputPath, 'plot_data')):
            with open(os.path.join(self.__inputPath, 'plot_data'), 'r') as f:
                lines = f.readlines()
                f.close()
                if len(lines) > 1:
                    line_last = lines[-1]
                    time_start = int(lines[1].split(',')[0])
                    time_start = time.strftime('%m/%d/%Y %H:%M:%S',
                                               time.localtime(time_start))
                    time_start = \
                        datetime.datetime.strptime(time_start,
                                                   '%m/%d/%Y %H:%M:%S')
                    time_end = int(line_last.split(',')[0])
                    time_end = time.strftime('%m/%d/%Y %H:%M:%S',
                                             time.localtime(time_end))
                    time_end = datetime.datetime.strptime(time_end,
                                                          '%m/%d/%Y %H:%M:%S')
                    execution_time = time_end - time_start
                    return execution_time
                else:
                    return 0

    def GenSumInfo(self):
        if self.__methods.lower() == 'afl':

            if self.__reportType == "gdb":
                crashNum = self.__GetFailedTypeNum(self.__afl_gdbCrashcfg)
                hangNum = self.__GetFailedTypeNum(self.__afl_gdbHangcfg)
                failNum = crashNum, hangNum
            elif self.__reportType == "sanitizer":
                AddressFailedNum = self.__GetFailedTypeNum(self.__acfg)
                LeakFailedNum = self.__GetFailedTypeNum(self.__lcfg)
                failNum = AddressFailedNum + LeakFailedNum
            totalNum = self.__getAFLTotalSeeds()
            execTime = self.__GetAFLExecTime()

        else:
            if self.__reportType == "gdb":
                failNum = self.__GetFailedTypeNum(self.__gdbcfg)
            elif self.__reportType == "sanitizer":
                AddressFailedNum = self.__GetFailedTypeNum(self.__afcfg)
                LeakFailedNum = self.__GetFailedTypeNum(self.__lcfg)
                failNum = AddressFailedNum + LeakFailedNum
            totalNum = len(os.listdir(self.__inputPath))
            execTime = '--:--:--'
        if not self.__silence:
            print(str(totalNum) + ';' + str(failNum) + ';' + str(execTime))
        return totalNum, failNum, execTime


if __name__ == "__main__":

    parse = argparse.ArgumentParser()
    parse.add_argument("-i", dest="input", help="seeds output path")
    parse.add_argument("-o", dest="output", help="report output path")
    parse.add_argument("-m", dest="method",
                       choices=['afl', 'libfuzzer'],
                       default="afl", help="test method")
    parse.add_argument("-t", dest="reportType",
                       choices=['gdb', 'sanitizer'], default="gdb",
                       help="report type")
    parse.add_argument("-s", dest="silence", help="silence")
    options = parse.parse_args(sys.argv[1:])

    if options.input:
        if options.output:
            if options.method:
                if options.reportType:
                    genSInfo = GenSummaryInfo(options.input, options.output,
                                              options.method,
                                              options.reportType,
                                              options.silence)
                    genSInfo.GenSumInfo()
                else:
                    raise Exception("Please choose reportType: "
                                    "gdb,sanitizer.")
            else:
                raise Exception("Please choose method: afl, "
                                "libfuzzer.")
        else:
            raise Exception("Please input -o output path.")
    else:
        raise Exception("Please input -i input path.")
