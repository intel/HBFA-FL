# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import shutil
import sys
try:
    import ConfigParser as ConfigParser
except Exception as e:
    print("Import for ConfigParser not found, attempting configparser: "
          "%s" % e)
    import configparser as ConfigParser
import argparse

Case_Path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Case')


class myconf(ConfigParser.RawConfigParser):
    def __init__(self, defaults=None):
        ConfigParser.RawConfigParser.__init__(self, defaults=None)

    def optionxform(self, optionstr):
        return optionstr


class errorInjection():
    def __init__(self, Num):
        self.CallErrorCountNum = int(Num)

    def run(self):
        if os.path.exists(Case_Path):
            shutil.rmtree(Case_Path)
            os.makedirs(Case_Path)
        else:
            os.makedirs(Case_Path)
        Case_Count_Num = 1
        for CountNum in range(1, self.CallErrorCountNum + 1):
            print('#######################################')
            print('Current CallErrorCountNum: {}'.format(CountNum))
            print('#######################################')
            for count in range(2):
                CaseName = 'test_' + str(Case_Count_Num) + '.ini'
                self.create_tcs(os.path.join(Case_Path, CaseName), CountNum,
                                count)
                Case_Count_Num = Case_Count_Num + 1

    def create_tcs(self, tcs_file, num, count):
        conf = myconf()
        if count == 0:
            conf.add_section('AllocatePages')
            conf.set('AllocatePages', 'CallErrorCount', num)
            conf.set('AllocatePages', 'ReturnValue', 0)
        elif count == 1:
            conf.add_section('FatReadBlock')
            conf.set('FatReadBlock', 'CallErrorCount', num)
            conf.set('FatReadBlock', 'ReturnValue', 'EFI_DEVICE_ERROR')
        else:
            pass

        with open(tcs_file, 'w') as f:
            conf.write(f)


if __name__ == '__main__':
    # # # Opt Parser
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", dest="CallErrorCountNum",
                        help="CallErrorCount number,if CallErrorCount = N,"
                        "script will try CallErrorCount = 1 ~ CallErrorCount"
                        " = N", default=None)

    options = parser.parse_args(sys.argv[1:])
    test = errorInjection(options.CallErrorCountNum)
    test.run()
