# @file
# Transfer report.log file into HTML file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import sys
import subprocess
import os
import time
try:
    import ConfigParser as ConfigParser
except Exception:
    import configparser as ConfigParser
import re
import argparse


class GenSanitizerInfo(object):

    def __init__(self, exefile, inputpath, output, silence=False, item=''):
        self.__exefile = exefile
        self.__inputpath = inputpath
        self.__output_path = output
        self.__silence = silence
        self.__classification = item
        if not os.path.exists(os.path.join(self.__output_path)):
            os.makedirs(os.path.join(self.__output_path))
        self.__scfgPath = os.path.join(self.__output_path,
                                       'AddressSanitizer.cfg')
        self.__lcfgPath = os.path.join(self.__output_path,
                                       'LeakSanitizer.cfg')

    def __CmpList(self, list1, list2):

        if len(list1) == 0 or len(list2) == 0:
            return False
        for i in list1:
            if i not in list2:
                return False
        for i in list2:
            if i not in list1:
                return False
        return True

    def __CmpLeakList(self, list1, list2):
        list1_temp = []
        result_list = []
        for i in list1:
            if (
                ' in ' in i and
                (('Indirect leak of ') not in i
                 and ('Direct leak of') not in i)
            ):
                i = i.split(' in ')[1]
            if ('Indirect leak of ') not in i and ('Direct leak of') not in i:
                list1_temp.append(i.strip())
        for sublist in list2:
            list2_temp = []
            for j in sublist:
                if (
                    ' in ' in j and (('Indirect leak of ')
                                     not in j and ('Direct leak of') not in j)
                ):
                    j = j.split(' in ')[1]
                if (
                    ('Indirect leak of ') not in j and ('Direct leak of')
                    not in j
                ):
                    list2_temp.append(j.strip())
            result = self.__CmpList(list1_temp, list2_temp)
            result_list.append(result)
        return result_list

    def __UpdateFileNum(self, filepath, keyword):
        file_data = ""
        with open(filepath, 'r') as f:
            for line in f:
                if '%s' % keyword in line:
                    num = line.split(':')[1].strip()
                    num = int(num) + 1
                    line = line.replace(line, '%s:%s\n' % (keyword, str(num)))
                file_data += line
        with open(filepath, 'w') as f:
            f.write(file_data)
            return num

    def __UpdateLFileNum(self, filepath, keyword, number):
        file_data = ""
        with open(filepath, 'r') as f:
            for line in f:
                if '%s' % keyword in line:
                    num = line.split(':')[1].strip()
                    num = int(num) + number
                    line = line.replace(line, '%s:%s\n' % (keyword, str(num)))

                file_data += line
        with open(filepath, 'w') as f:
            f.write(file_data)
            return num

    def __GetRunTimes(self):
        if not os.path.exists(os.path.join(self.__output_path, '.status')):
            file = open(os.path.join(self.__output_path, '.status'), 'w+')
            time_stamp = time.strftime("%m/%d/%Y %H:%M:%S",
                                       time.localtime(time.time()))
            file.write('start_time: %s\n' % time_stamp)
            number = 1
            file.write('number:%s\n' % str(number))
            file.close()
        else:
            number = self.__UpdateFileNum(os.path.join(self.__output_path,
                                                       '.status'), 'number')
        return number

    def __GetAcompareList(self, contentlines):
        new_list = []
        for line in contentlines:
            match1 = re.search(r'ERROR: AddressSanitizer: (\S)+', line)
            if match1:
                error_message = match1.group(0)
                new_list.append(error_message)
            match2 = re.search(r'#\d 0x(.)+', line)
            if match2:
                stack_info = \
                    match2.group().split(' in ')[1] if \
                    ' in ' in line else match2.group().split(' ')[-1]
                new_list.append(stack_info)
            if (
                'allocated by' in line or
                re.search(r'Address 0x(\w)+ is located in', line)
            ):
                break
        return new_list

    def __GetLcompareList(self, contentlines):
        lcompare_word_list = []
        list_key = []
        index = []
        for i, line in enumerate(contentlines):
            match1 = re.search('ERROR: LeakSanitizer: (.)+', line)
            if match1:
                error_message = match1.group(0)
                list_key.append(error_message)
            if line == '':
                index.append(i)
        for i in range(1, len(index)):
            if i == len(index) - 2:
                break
            list = contentlines[index[i] + 1:index[i + 1]]
            list.insert(0, list_key[0])
            list = [j.strip() + '\n' for j in list]
            lcompare_word_list.append(list)
        return lcompare_word_list

    def __UpdateAcfg(self, screenlogs, number, inputpath):
        errorlog_lines = screenlogs.split('\n')
        compare_word_list = self.__GetAcompareList(errorlog_lines)
        cfg = ConfigParser.ConfigParser()
        cfg.read(self.__scfgPath)
        error_list = cfg.sections()
        if len(error_list) == 0:
            flag = False
            file = open(os.path.join(self.__output_path, '.status'), 'a+')
            file.write('Atypenum:0\n')
            file.write('Atotalnum:1\n')
            file.close()
        else:
            self.__UpdateFileNum(os.path.join(self.__output_path, '.status'),
                                 'Atotalnum')
            for item in error_list:
                contents = cfg.get(item, 'stackinfo')
                contents_list = contents.split('\n')
                to_be_compared_list = self.__GetAcompareList(contents_list)
                flag = self.__CmpList(to_be_compared_list, compare_word_list)
                if flag is True:
                    seedsCount = cfg.get(item, 'TotalSeedsNum')
                    seedsCount = int(seedsCount) + 1
                    cfg.set(item, 'TotalSeedsNum', seedsCount)
                    break
                if flag is False and item == error_list[-1]:
                    self.__UpdateFileNum(os.path.join(self.__output_path,
                                                      '.status'),
                                         'Atypenum')

        if flag is not True:
            item = inputpath
            if item not in cfg.sections():
                cfg.add_section(item)
            cfg.set(item, 'stackInfo', screenlogs)
            seedsCount = 1
            cfg.set(item, 'TotalSeedsNum', seedsCount)

        error_list = cfg.sections()
        if not self.__silence:
            print('there are %s type errors,and they are : %s'
                  % (len(error_list), error_list))
        cfg.write(open(self.__scfgPath, "w"))

    def __UpdateLcfg(self, screenlogs, number, inputpath):
        errorlog_lines = screenlogs.split('\n')
        lcompare_word_list = self.__GetLcompareList(errorlog_lines)
        cfg = ConfigParser.ConfigParser()
        cfg.read(self.__lcfgPath)
        error_list = cfg.sections()
        if len(error_list) == 0:
            flag = False
            Ltypenum = 0
            file = open(os.path.join(self.__output_path, '.status'), 'a+')
            file.write('Ltypenum:0\n')
            file.write('Ltotalnum:1\n')
            file.close()
        else:
            self.__UpdateFileNum(os.path.join(self.__output_path,
                                              '.status'), 'Ltotalnum')
            compare_result_list = []
            all_result_list = []
            for item in error_list:
                to_be_compared_contents = cfg.get(item, 'stackinfo')
                to_be_compared_list = to_be_compared_contents.split('\n')
                flag = self.__CmpLeakList(to_be_compared_list,
                                          lcompare_word_list)
                compare_result_list.append(flag)

            for i in range(len(compare_result_list[0])):
                column_list = []
                for j in range(len(compare_result_list)):
                    column_list.append(compare_result_list[j][i])

                all_result_list.append(column_list)
            for i, result in enumerate(all_result_list):
                if True not in result:
                    Ltypenum = \
                        self.__UpdateFileNum(os.path.join(self.__output_path,
                                                          '.status'),
                                             'Ltypenum')
                    item = 'number' + str(Ltypenum) + '_' + 'LeakSanitizer'
                    if item not in cfg.sections():
                        cfg.add_section(item)
                    stackinfo = ''.join(lcompare_word_list[i])
                    if not self.__silence:
                        print('NEW DIFFERENT STACK INFO: \n%s' % stackinfo)
                    cfg.set(item, 'stackInfo', stackinfo)
                    cfg.set(item, 'filepath', os.path.dirname(inputpath))

        if flag is False:
            Ltypenum = len(lcompare_word_list)
            self.__UpdateLFileNum(os.path.join(self.__output_path, '.status'),
                                  'Ltypenum', Ltypenum)
            for i in range(0, len(lcompare_word_list)):
                Ltypenum = i + 1
                item = 'number' + str(Ltypenum) + '_' + 'LeakSanitizer'
                if item not in cfg.sections():
                    cfg.add_section(item)
                stackinfo = ''.join(lcompare_word_list[i])
                cfg.set(item, 'stackInfo', stackinfo)
                cfg.set(item, 'filepath', os.path.dirname(inputpath))
        cfg.write(open(self.__lcfgPath, "w"))

    def __GenSanitizerInfo(self, inputpath):
        number = str(self.__GetRunTimes())
        command = [self.__exefile, inputpath]
        ret = subprocess.Popen(command, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, shell=False)
        error_logs = list(ret.communicate())
        if sys.version_info[0] == 3:
            for num, submsg in enumerate(error_logs):
                if submsg is not None:
                    error_logs[num] = submsg.decode()
        if not self.__silence:
            print(error_logs[0])
            print(error_logs[1])

        logfile = os.path.splitext(inputpath)[0] + '.HBFA.Sanitizer.log'
        error_logs_file = open(logfile, 'w+')
        error_logs_file.write("{}\n{}".format(error_logs[0] if
                                              error_logs[0] else '',
                                              error_logs[1] if
                                              error_logs[1] else ''))
        error_logs_file.close()

        if 'ERROR: AddressSanitizer' in error_logs[0]:
            self.__UpdateAcfg(error_logs[0], number, inputpath)
        elif 'ERROR: LeakSanitizer:' in error_logs[0]:
            self.__UpdateLcfg(error_logs[0], number, inputpath)

    def __GenAFLSanitizerInfo(self, inputpath):
        command = [self.__exefile, inputpath]
        ret = subprocess.Popen(command, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, shell=False)
        error_logs = list(ret.communicate())
        if sys.version_info[0] == 3:
            for num, submsg in enumerate(error_logs):
                if submsg is not None:
                    error_logs[num] = submsg.decode()
        if not self.__silence:
            print(error_logs[0])
            print(error_logs[1])

        logfile = os.path.join(self.__output_path, 'HBFA.Sanitizer.log')
        error_logs_file = open(logfile, 'a+')
        error_logs_file.write("{}\n{}".format(error_logs[0] if
                                              error_logs[0] else '',
                                              error_logs[1] if
                                              error_logs[1] else ''))
        error_logs_file.close()
        item = inputpath
        if 'ERROR: AddressSanitizer' in error_logs[0] or error_logs[0] == '':
            cfg = ConfigParser.ConfigParser()
            cfg.read(self.__scfgPath)
            cfg.add_section(item)
            cfg.set(item, 'stackInfo', error_logs[0])
            TotalSeedsNum = 1
            cfg.set(item, 'TotalSeedsNum', TotalSeedsNum)
            cfg.write(open(self.__scfgPath, "w"))
        elif 'ERROR: LeakSanitizer:' in error_logs[0]:
            cfg = ConfigParser.ConfigParser()
            cfg.read(self.__lcfgPath)
            cfg.add_section(item)
            cfg.set(item, 'stackInfo', error_logs[0])
            TotalSeedsNum = 1
            cfg.set(item, 'TotalSeedsNum', TotalSeedsNum)
            cfg.write(open(self.__lcfgPath, "w"))

    def Run(self):
        if type(self.__inputpath) is list and len(self.__inputpath) != 0:
            for seedpath in self.__inputpath:
                if self.__classification == '':
                    self.__GenSanitizerInfo(seedpath)
                else:
                    self.__GenAFLSanitizerInfo(seedpath)
        else:
            if not self.__silence:
                print('please verify input seeds exist')


if __name__ == "__main__":
    parse = argparse.ArgumentParser()
    parse.add_argument("-e", dest="execute", help="executable file path")
    parse.add_argument("-i", dest="input", help="seed name")
    parse.add_argument("-o", dest="output", help="sanitizer output log path")
    parse.add_argument("-s", dest="silence", help="silence", default=False)
    parse.add_argument("-t", dest="item", help="item", default='')

    options = parse.parse_args(sys.argv[1:])
    if options.execute:
        if options.input:
            if options.output:
                genSInfo = GenSanitizerInfo(options.execute, options.input,
                                            options.output, False, '')
                genSInfo.Run()
            else:
                raise Exception("Plesase input -o output path.")
        else:
            raise Exception("Plesase input -i input seed name.")
    else:
        raise Exception("Please input -e executable file you use.")
