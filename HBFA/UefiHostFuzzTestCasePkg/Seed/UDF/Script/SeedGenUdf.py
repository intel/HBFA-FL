# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
SeedGenUdf
'''

import argparse
import ctypes
import Udf

#
# Globals for help information
#
__prog__ = 'SeedGenUdf'
__version__ = '%s Version %s' % (__prog__, '0.11 ')
__copyright__ = 'Copyright (c) 2018, Intel Corporation. All rights reserved.'
__usage__ = '%s [options] -o <output_file>' % (__prog__)

TOTAL_SIZE = 1 * 1024 * 1024
BLOCK_SIZE = 2048

if __name__ == '__main__':
    #
    # Create command line argument parser object
    #
    parser = argparse.ArgumentParser(prog=__prog__,
                                     usage=__usage__,
                                     description=__copyright__,
                                     conflict_handler='resolve')
    parser.add_argument('--version', action='version', version=__version__)
    parser.add_argument("-v", "--verbose", dest='Verbose', action="store_true",
                        help="increase output messages")
    parser.add_argument("-q", "--quiet", dest='Quiet', action="store_true",
                        help="reduce output messages")
    parser.add_argument("-o", "--output", dest='OutputFileName', type=str,
                        metavar='filename', help="specify the output filename",
                        required=True)

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    #
    # Write output file
    #
    args.OutputFile = open(args.OutputFileName, 'wb')

    DummyBuf = ctypes.create_string_buffer(TOTAL_SIZE)
    Udf.CdromDesc = Udf.CDROM_VOLUME_DESCRIPTOR.from_buffer(
        DummyBuf,
        Udf.UDF_VRS_START_OFFSET)
    Udf.CdromDesc.Id[0] = (ord('B'))
    Udf.CdromDesc.Id[1] = (ord('E'))
    Udf.CdromDesc.Id[2] = (ord('A'))
    Udf.CdromDesc.Id[3] = (ord('0'))
    Udf.CdromDesc.Id[4] = (ord('1'))

    Udf.CdromDesc = Udf.CDROM_VOLUME_DESCRIPTOR.from_buffer(
        DummyBuf,
        Udf.UDF_VRS_START_OFFSET + Udf.UDF_LOGICAL_SECTOR_SIZE)
    Udf.CdromDesc.Id[0] = (ord('N'))
    Udf.CdromDesc.Id[1] = (ord('S'))
    Udf.CdromDesc.Id[2] = (ord('R'))
    Udf.CdromDesc.Id[3] = (ord('0'))
    Udf.CdromDesc.Id[4] = (ord('2'))

    Udf.CdromDesc = Udf.CDROM_VOLUME_DESCRIPTOR.from_buffer(
        DummyBuf,
        Udf.UDF_VRS_START_OFFSET + Udf.UDF_LOGICAL_SECTOR_SIZE * 2)
    Udf.CdromDesc.Id[0] = (ord('T'))
    Udf.CdromDesc.Id[1] = (ord('E'))
    Udf.CdromDesc.Id[2] = (ord('A'))
    Udf.CdromDesc.Id[3] = (ord('0'))
    Udf.CdromDesc.Id[4] = (ord('1'))

    AnchorDesc = Udf.UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER.from_buffer(
        DummyBuf,
        0x100 * BLOCK_SIZE)
    (AnchorDesc.DescriptorTag.TagIdentifier
     ) = Udf.UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer

    LastBlock = (TOTAL_SIZE + BLOCK_SIZE - 1) // BLOCK_SIZE  # - 1
    Offset = (LastBlock - Udf.MAX_CORRECTION_BLOCKS_NUM) * BLOCK_SIZE
    (AnchorDesc
     ) = Udf.UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER.from_buffer(DummyBuf, Offset)
    (AnchorDesc.DescriptorTag.TagIdentifier
     ) = Udf.UDF_VOLUME_DESCRIPTOR_ID.UdfAnchorVolumeDescriptorPointer

    SeqBlocksNum = 16
    SeqStartBlock = 16
    (AnchorDesc.MainVolumeDescriptorSequenceExtent.ExtentLength
     ) = SeqBlocksNum * BLOCK_SIZE
    (AnchorDesc.MainVolumeDescriptorSequenceExtent.ExtentLocation
     ) = SeqStartBlock

    Offset = SeqStartBlock * BLOCK_SIZE
    DescTag = Udf.UDF_DESCRIPTOR_TAG.from_buffer(DummyBuf, Offset)
    (DescTag.TagIdentifier
     ) = Udf.UDF_VOLUME_DESCRIPTOR_ID.UdfTerminatingDescriptor

    args.OutputFile.write(DummyBuf)

    args.OutputFile.close()
