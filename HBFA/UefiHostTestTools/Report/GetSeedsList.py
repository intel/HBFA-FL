# @file
#
# Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import glob

# python version
python_version = sys.version_info[0]


class GetSeedsList(object):
    def __init__(self, seedsType, inputPath, silence):
        self.__seedsType = seedsType
        self.__inputPath = inputPath
        self.__seedList = [[], []] if self.__seedsType.lower() == "afl" else []
        self.__silence = silence

    def __GetList(self):
        seedList = []
        if self.__seedsType.lower() == 'afl':
            for item in ['crashes', 'hangs']:
                lists = []
                subdir = os.path.join(self.__inputPath, item)
                if os.path.exists(subdir):
                    lists = glob.glob(os.path.join(subdir, 'id*'))
                lists.sort(key=lambda fn: os.path.getmtime(fn), reverse=False)
                seedList.append(lists)
        elif self.__seedsType.lower() == 'libfuzzer':
            seedList = [fn for fn in
                        glob.glob(os.path.join(self.__inputPath, '*'))
                        if os.path.isfile(fn)]
            seedList.sort(key=lambda fn: os.path.getmtime(fn), reverse=False)
        return seedList

    def SeekNewSeed(self):
        """
        :return: afl [[crash], [hang]], libfuzzer [seed]
        """
        newSeedList = self.__GetList()

        if self.__seedsType.lower() == 'afl':
            newList = [[], []]
            for index in range(len(self.__seedList)):
                if len(newSeedList[index]) != len(self.__seedList[index]):
                    newList[index] = [fn for fn in
                                      (newSeedList[index] +
                                       self.__seedList[index])
                                      if fn not in self.__seedList[index]]
                    self.__seedList[index] = newSeedList[index]
                    if not self.__silence:
                        print('Current crashes number is %d, hangs'
                              ' number is %d' %
                              (len(self.__seedList[0]),
                               len(self.__seedList[1]))
                              )
        elif (
            self.__seedsType.lower() == 'libfuzzer'
        ):
            newList = []
            if len(newSeedList) != len(self.__seedList):
                newList = [fn for fn in (newSeedList + self.__seedList)
                           if fn not in self.__seedList]
                self.__seedList = newSeedList
                if not self.__silence:
                    print('Current fault number is %d' %
                          (len(self.__seedList))
                          )
        return newList
