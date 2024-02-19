# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import ctypes

UDF_LOGICAL_SECTOR_SIZE = 0x800
UDF_VRS_START_OFFSET = 0x8000

MAX_CORRECTION_BLOCKS_NUM = 512

UDF_BEA_IDENTIFIER = "BEA01"
UDF_NSR2_IDENTIFIER = "NSR02"
UDF_NSR3_IDENTIFIER = "NSR03"
UDF_TEA_IDENTIFIER = "TEA01"


class UDF_VOLUME_DESCRIPTOR_ID:
    UdfPrimaryVolumeDescriptor = 1
    UdfAnchorVolumeDescriptorPointer = 2
    UdfVolumeDescriptorPointer = 3
    UdfImplemenationUseVolumeDescriptor = 4
    UdfPartitionDescriptor = 5
    UdfLogicalVolumeDescriptor = 6
    UdfUnallocatedSpaceDescriptor = 7
    UdfTerminatingDescriptor = 8
    UdfLogicalVolumeIntegrityDescriptor = 9
    UdfFileSetDescriptor = 256
    UdfFileIdentifierDescriptor = 257
    UdfAllocationExtentDescriptor = 258
    UdfFileEntry = 261
    UdfExtendedFileEntry = 266

    TagStringTable = [
        [UdfPrimaryVolumeDescriptor, "UdfPrimaryVolumeDescriptor"],
        [UdfAnchorVolumeDescriptorPointer, "UdfAnchorVolumeDescriptorPointer"],
        [UdfVolumeDescriptorPointer, "UdfVolumeDescriptorPointer"],
        [UdfImplemenationUseVolumeDescriptor,
         "UdfImplemenationUseVolumeDescriptor"],
        [UdfPartitionDescriptor, "UdfPartitionDescriptor"],
        [UdfLogicalVolumeDescriptor, "UdfLogicalVolumeDescriptor"],
        [UdfUnallocatedSpaceDescriptor, "UdfUnallocatedSpaceDescriptor"],
        [UdfTerminatingDescriptor, "UdfTerminatingDescriptor"],
        [UdfLogicalVolumeIntegrityDescriptor,
         "UdfLogicalVolumeIntegrityDescriptor"],
        [UdfFileSetDescriptor, "UdfFileSetDescriptor"],
        [UdfFileIdentifierDescriptor, "UdfFileIdentifierDescriptor"],
        [UdfAllocationExtentDescriptor, "UdfAllocationExtentDescriptor"],
        [UdfFileEntry, "UdfFileEntry"],
        [UdfExtendedFileEntry, "UdfExtendedFileEntry"],
    ]


class CDROM_VOLUME_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('Type', ctypes.c_uint8),
        ('Id', ctypes.ARRAY(ctypes.c_uint8, 5)),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 82)),
    ]


class UDF_DESCRIPTOR_TAG(ctypes.Structure):
    _fields_ = [
        ('TagIdentifier', ctypes.c_uint16),
        ('DescriptorVersion', ctypes.c_uint16),
        ('TagChecksum', ctypes.c_uint8),
        ('Reserved', ctypes.c_uint8),
        ('TagSerialNumber', ctypes.c_uint16),
        ('DescriptorCRC', ctypes.c_uint16),
        ('DescriptorCRCLength', ctypes.c_uint16),
        ('TagLocation', ctypes.c_uint32),
        ]


class UDF_EXTENT_AD(ctypes.Structure):
    _fields_ = [
        ('ExtentLength', ctypes.c_uint32),
        ('ExtentLocation', ctypes.c_uint32),
        ]


class UDF_CHAR_SPEC(ctypes.Structure):
    _fields_ = [
        ('CharacterSetType', ctypes.c_uint8),
        ('CharacterSetInfo', ctypes.ARRAY(ctypes.c_uint8, 63)),
        ]


class UDF_ENTITY_ID_DOMAIN(ctypes.Structure):
    _fields_ = [
        ('Flags', ctypes.c_uint8),
        ('Identifier', ctypes.ARRAY(ctypes.c_uint8, 23)),
        ('UdfRevision', ctypes.c_uint16),
        ('DomainFlags', ctypes.c_uint8),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 5)),
        ]


class UDF_ENTITY_ID_ENTITY(ctypes.Structure):
    _fields_ = [
        ('Flags', ctypes.c_uint8),
        ('Identifier', ctypes.ARRAY(ctypes.c_uint8, 23)),
        ('UdfRevision', ctypes.c_uint16),
        ('OSClass', ctypes.c_uint8),
        ('OSIdentifier', ctypes.c_uint8),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 4)),
        ]


class UDF_ENTITY_ID_IMPLEMENTATION_ENTITY(ctypes.Structure):
    _fields_ = [
        ('Flags', ctypes.c_uint8),
        ('Identifier', ctypes.ARRAY(ctypes.c_uint8, 23)),
        ('OSClass', ctypes.c_uint8),
        ('OSIdentifier', ctypes.c_uint8),
        ('ImplementationUseArea', ctypes.ARRAY(ctypes.c_uint8, 6)),
        ]


class UDF_ENTITY_ID_APPLICATION_ENTITY(ctypes.Structure):
    _fields_ = [
        ('Flags', ctypes.c_uint8),
        ('Identifier', ctypes.ARRAY(ctypes.c_uint8, 23)),
        ('ApplicationUseArea', ctypes.ARRAY(ctypes.c_uint8, 8)),
        ]


class UDF_ENTITY_ID(ctypes.Structure):
    _fields_ = [
        ('Flags', ctypes.c_uint8),
        ('Identifier', ctypes.ARRAY(ctypes.c_uint8, 23)),
        ('Data', ctypes.ARRAY(ctypes.c_uint8, 8)),
        ]


class UDF_TIMESTAMP(ctypes.Structure):
    _fields_ = [
        ('TypeAndTimezone', ctypes.c_uint16),
        ('Year', ctypes.c_int16),
        ('Month', ctypes.c_uint8),
        ('Day', ctypes.c_uint8),
        ('Hour', ctypes.c_uint8),
        ('Minute', ctypes.c_uint8),
        ('Second', ctypes.c_uint8),
        ('Centiseconds', ctypes.c_uint8),
        ('HundredsOfMicroseconds', ctypes.c_uint8),
        ('Microseconds', ctypes.c_uint8),
        ]


class UDF_LONG_ALLOCATION_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('ExtentLength', ctypes.c_uint32),
        ('ExtentLocationLogicalBlockNumber', ctypes.c_uint32),
        ('ExtentLocationPartitionReferenceNumber', ctypes.c_uint16),
        ('ImplementationUse', ctypes.ARRAY(ctypes.c_uint8, 6)),
        ]


class UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('MainVolumeDescriptorSequenceExtent', UDF_EXTENT_AD),
        ('ReserveVolumeDescriptorSequenceExtent', UDF_EXTENT_AD),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 480)),
        ]


class UDF_LOGICAL_VOLUME_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('VolumeDescriptorSequenceNumber', ctypes.c_uint32),
        ('DescriptorCharacterSet', UDF_CHAR_SPEC),
        ('LogicalVolumeIdentifier', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ('LogicalBlockSize', ctypes.c_uint32),
        ('DomainIdentifier', UDF_ENTITY_ID_DOMAIN),
        ('LogicalVolumeContentsUse', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('MapTableLength', ctypes.c_uint32),
        ('NumberOfPartitionMaps', ctypes.c_uint32),
        ('ImplementationIdentifier', UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('ImplementationUse', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ('IntegritySequenceExtent', UDF_EXTENT_AD),
        ('PartitionMaps', ctypes.ARRAY(ctypes.c_uint8, 6)),
        ]


class UDF_PARTITION_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('VolumeDescriptorSequenceNumber', ctypes.c_uint32),
        ('PartitionFlags', ctypes.c_uint16),
        ('PartitionNumber', ctypes.c_uint16),
        ('PartitionContents', UDF_ENTITY_ID),
        ('PartitionContentsUse', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ('AccessType', ctypes.c_uint32),
        ('PartitionStartingLocation', ctypes.c_uint32),
        ('PartitionLength', ctypes.c_uint32),
        ('ImplementationIdentifier', UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('ImplementationUse', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ]


class UDF_FILE_SET_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('RecordingDateAndTime', UDF_TIMESTAMP),
        ('InterchangeLevel', ctypes.c_uint16),
        ('MaximumInterchangeLevel', ctypes.c_uint16),
        ('CharacterSetList', ctypes.c_uint32),
        ('MaximumCharacterSetList', ctypes.c_uint32),
        ('FileSetNumber', ctypes.c_uint32),
        ('FileSetDescriptorNumber', ctypes.c_uint32),
        ('LogicalVolumeIdentifierCharacterSet', UDF_CHAR_SPEC),
        ('LogicalVolumeIdentifier', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ('FileSetCharacterSet', UDF_CHAR_SPEC),
        ('FileSetIdentifier', ctypes.ARRAY(ctypes.c_uint8, 32)),
        ('CopyrightFileIdentifier', ctypes.ARRAY(ctypes.c_uint8, 32)),
        ('AbstractFileIdentifier', ctypes.ARRAY(ctypes.c_uint8, 32)),
        ('RootDirectoryIcb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('DomainIdentifier', UDF_ENTITY_ID_DOMAIN),
        ('NextExtent', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('SystemStreamDirectoryIcb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 32)),
        ]


class UDF_ICB_TAG(ctypes.Structure):
    _fields_ = [
        ('PriorRecordNumberOfDirectEntries', ctypes.c_uint32),
        ('StrategyType', ctypes.c_uint16),
        ('StrategyParameter', ctypes.c_uint16),
        ('MaximumNumberOfEntries', ctypes.c_uint16),
        ('Reserved', ctypes.c_uint8),
        ('FileType', ctypes.c_uint8),
        ('ParentIcbLocationLogicalBlockNumber', ctypes.c_uint32),
        ('ParentIcbLocationPartitionReferenceNumber', ctypes.c_uint16),
        ('Flags', ctypes.c_uint16),
        ]


class UDF_EXTENDED_FILE_ENTRY(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('IcbTag', UDF_ICB_TAG),
        ('Uid', ctypes.c_uint32),
        ('Gid', ctypes.c_uint32),
        ('Permissions', ctypes.c_uint32),
        ('FileLinkCount', ctypes.c_uint16),
        ('RecordFormat', ctypes.c_uint8),
        ('RecordDisplayAttributes', ctypes.c_uint8),
        ('RecordLength', ctypes.c_uint32),
        ('InformationLength', ctypes.c_uint64),
        ('ObjectSize', ctypes.c_uint64),
        ('LogicalBlocksRecorded', ctypes.c_uint64),
        ('AccessTime', UDF_TIMESTAMP),
        ('ModificationTime', UDF_TIMESTAMP),
        ('CreationTime', UDF_TIMESTAMP),
        ('AttributeTime', UDF_TIMESTAMP),
        ('CheckPoint', ctypes.c_uint32),
        ('Reserved', ctypes.c_uint32),
        ('ExtendedAttributeIcb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('StreamDirectoryIcb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('ImplementationIdentifier', UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('UniqueId', ctypes.c_uint64),
        ('LengthOfExtendedAttributes', ctypes.c_uint32),
        ('LengthOfAllocationDescriptors', ctypes.c_uint32),
        ]


class UDF_FILE_ENTRY(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('IcbTag', UDF_ICB_TAG),
        ('Uid', ctypes.c_uint32),
        ('Gid', ctypes.c_uint32),
        ('Permissions', ctypes.c_uint32),
        ('FileLinkCount', ctypes.c_uint16),
        ('RecordFormat', ctypes.c_uint8),
        ('RecordDisplayAttributes', ctypes.c_uint8),
        ('RecordLength', ctypes.c_uint32),
        ('InformationLength', ctypes.c_uint64),
        ('LogicalBlocksRecorded', ctypes.c_uint64),
        ('AccessTime', UDF_TIMESTAMP),
        ('ModificationTime', UDF_TIMESTAMP),
        ('AttributeTime', UDF_TIMESTAMP),
        ('CheckPoint', ctypes.c_uint32),
        ('ExtendedAttributeIcb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('ImplementationIdentifier', UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('UniqueId', ctypes.c_uint64),
        ('LengthOfExtendedAttributes', ctypes.c_uint32),
        ('LengthOfAllocationDescriptors', ctypes.c_uint32),
    ]


class UDF_FILE_IDENTIFIER_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('FileVersionNumber', ctypes.c_uint16),
        ('FileCharacteristics', ctypes.c_uint8),
        ('LengthOfFileIdentifier', ctypes.c_uint8),
        ('Icb', UDF_LONG_ALLOCATION_DESCRIPTOR),
        ('LengthOfImplementationUse', ctypes.c_uint16),
        ('Pad', ctypes.c_uint16),
        ]


class UDF_PRIMARY_VOLUME_DESCRIPTOR(ctypes.Structure):
    _fields_ = [
        ('DescriptorTag', UDF_DESCRIPTOR_TAG),
        ('VolumeDescriptorSequenceNumber', ctypes.c_uint32),
        ('PrimaryVolumeDescriptorNumber', ctypes.c_uint32),
        ('VolumeIdentifier', ctypes.ARRAY(ctypes.c_uint8, 32)),
        ('VolumeSequenceNumber', ctypes.c_uint16),
        ('MaximumVolumeSequenceNumber', ctypes.c_uint16),
        ('InterchangeLevel', ctypes.c_uint16),
        ('MaximumInterchangeLevel', ctypes.c_uint16),
        ('CharacterSetList', ctypes.c_uint32),
        ('MaximumCharacterSetList', ctypes.c_uint32),
        ('VolumeSetIdentifier', ctypes.ARRAY(ctypes.c_uint8, 128)),
        ('DescriptorCharacterSet', UDF_CHAR_SPEC),
        ('ExplanatoryCharacterSet', UDF_CHAR_SPEC),
        ('VolumeAbstract', UDF_EXTENT_AD),
        ('VolumeCopyrightNotice', UDF_EXTENT_AD),
        ('ApplicationIdentifier', UDF_ENTITY_ID_APPLICATION_ENTITY),
        ('RecordingDateAndTime', UDF_TIMESTAMP),
        ('ImplementationIdentifier', UDF_ENTITY_ID_IMPLEMENTATION_ENTITY),
        ('ImplementationUse', ctypes.ARRAY(ctypes.c_uint8, 64)),
        ('PredecessorVolumeDescriptorSequenceLocation', ctypes.c_uint32),
        ('Flags', ctypes.c_uint16),
        ('Reserved', ctypes.ARRAY(ctypes.c_uint8, 22)),
        ]
