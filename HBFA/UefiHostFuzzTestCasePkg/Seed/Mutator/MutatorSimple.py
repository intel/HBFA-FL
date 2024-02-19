# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
MutatorSimple
'''

import os
import argparse
import subprocess
import random

#
# Globals for help information
#
__prog__ = 'MutatorSimple'
__version__ = '%s Version %s' % (__prog__, '0.11 ')
__copyright__ = 'Copyright (c) 2018, Intel Corporation. All rights reserved.'
__usage__ = '%s [options] -e <exec_file>' % (__prog__)


def WriteFile():
    args.OutputFile = open(args.OutputFileName, 'wb')
    args.OutputFile.write(args.InputBuffer)
    args.OutputFile.close()


def GenerateCommand(Command, InputFile):
    Template = "<Command> <InputFile>"
    CommandLine = Template.replace("<Command>",
                                   Command).replace("<InputFile>", InputFile)
    return CommandLine


def CallCommand(CommandLine):
    subprocess.Popen(CommandLine,
                     stdin=subprocess.PIPE,
                     stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE,
                     shell=False)


def RunCase():
    CallCommand(GenerateCommand(args.Executable, args.OutputFileName))


def DelCase():
    os.remove(args.OutputFileName)


if __name__ == '__main__':
    #
    # Create command line argument parser object
    #
    parser = argparse.ArgumentParser(prog=__prog__,
                                     usage=__usage__,
                                     description=__copyright__,
                                     conflict_handler='resolve')
    parser.add_argument('--version', action='version', version=__version__)
    parser.add_argument("-v", "--verbose", dest='Verbose',
                        action="store_true",
                        help="increase output messages")
    parser.add_argument("-q", "--quiet", dest='Quiet', action="store_true",
                        help="reduce output messages")
    parser.add_argument(metavar="input_file", dest='InputFile',
                        type=argparse.FileType('rb'),
                        help="specify the input file")
    parser.add_argument("--Offset", dest='Offset_Str', type=str,
                        help="specify the offset of buffer to be mutated.")
    parser.add_argument("--Size", dest='Size_Str', type=str,
                        help="specify the size of buffer to be mutated.")
    parser.add_argument("-e", "--exec", dest='Executable', type=str,
                        metavar='filename',
                        help="specify the executable filename",
                        required=True)

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    #
    # Read input file into a buffer and save input filename
    #
    args.InputFileName = args.InputFile.name
    args.InputFileBuffer = args.InputFile.read()
    args.InputFile.seek(0, 2)
    InputFileSize = args.InputFile.tell()
    args.InputFile.close()

    print("file size - 0x%x (%d)" % (InputFileSize, InputFileSize))

    args.Offset = 0x0
    if args.Offset_Str:
        try:
            if args.Offset_Str.upper().startswith('0X'):
                args.Offset = (int)(args.Offset_Str, 16)
            else:
                args.Offset = (int)(args.Offset_Str)
        except Exception as error:
            print("Unexpected format for Offset: %s" % (error))
            pass

    args.Size = InputFileSize
    if args.Size_Str:
        try:
            if args.Size_Str.upper().startswith('0X'):
                args.Size = (int)(args.Size_Str, 16)
            else:
                args.Size = (int)(args.Size_Str)
        except Exception as error:
            print("Unexpected format for input file size: %s" % (error))
            pass

    print("fuzz offset - 0x%x (%d)" % (args.Offset, args.Offset))
    print("fuzz size - 0x%x (%d)" % (args.Size, args.Size))

    #
    # mutation
    #
    args.InputBuffer = bytearray(args.InputFileBuffer)

    # UINT8 based mutation
    for Index in range(args.Offset, args.Offset + args.Size):
        OrgData = args.InputBuffer[Index:Index+1]

        #
        # mutate
        #
        RandomData = random.randint(0, 0xFF)
        args.InputBuffer[Index:Index+1] = [RandomData]

        #
        # Write output file
        #
        args.OutputFileName = args.InputFileName + "_b_" + str(Index) + ".bin"
        WriteFile()
        RunCase()
        DelCase()

        args.InputBuffer[Index:Index+1] = OrgData

    # UINT16 based mutation
    for Index in range(args.Offset, args.Offset + args.Size - 1):
        OrgData = args.InputBuffer[Index:Index+2]

        #
        # mutate
        #
        RandomData = random.randint(0, 0xFF)
        RandomData2 = random.randint(0, 0xFF)
        args.InputBuffer[Index:Index+2] = [RandomData, RandomData2]

        #
        # Write output file
        #
        args.OutputFileName = args.InputFileName + "_w_" + str(Index) + ".bin"
        WriteFile()
        RunCase()
        DelCase()

        args.InputBuffer[Index:Index+2] = OrgData

    # UINT32 based mutation
    for Index in range(args.Offset, args.Offset + args.Size - 3):
        OrgData = args.InputBuffer[Index:Index+4]

        #
        # mutate
        #
        RandomData = random.randint(0, 0xFF)
        RandomData2 = random.randint(0, 0xFF)
        RandomData3 = random.randint(0, 0xFF)
        RandomData4 = random.randint(0, 0xFF)
        args.InputBuffer[Index:Index+4] = [RandomData, RandomData2,
                                           RandomData3, RandomData4]

        #
        # Write output file
        #
        args.OutputFileName = args.InputFileName + "_d_" + str(Index) + ".bin"
        WriteFile()
        RunCase()
        DelCase()

        args.InputBuffer[Index:Index+4] = OrgData

    # UINT64 based mutation
    for Index in range(args.Offset, args.Offset + args.Size - 7):
        OrgData = args.InputBuffer[Index:Index+8]

        #
        # mutate
        #
        RandomData = random.randint(0, 0xFF)
        RandomData2 = random.randint(0, 0xFF)
        RandomData3 = random.randint(0, 0xFF)
        RandomData4 = random.randint(0, 0xFF)
        RandomData5 = random.randint(0, 0xFF)
        RandomData6 = random.randint(0, 0xFF)
        RandomData7 = random.randint(0, 0xFF)
        RandomData8 = random.randint(0, 0xFF)
        args.InputBuffer[Index:Index+8] = [RandomData, RandomData2,
                                           RandomData3, RandomData4,
                                           RandomData5, RandomData6,
                                           RandomData7, RandomData8]

        #
        # Write output file
        #
        args.OutputFileName = args.InputFileName + "_q_" + str(Index) + ".bin"
        WriteFile()
        RunCase()
        DelCase()

        args.InputBuffer[Index:Index+8] = OrgData
