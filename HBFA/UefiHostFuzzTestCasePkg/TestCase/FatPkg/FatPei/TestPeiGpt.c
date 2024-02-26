/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>
#include <Guid/Gpt.h>
#include <Uefi/UefiGpt.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "FatLitePeim.h"

BOOLEAN
FatFindGptPartitions(
  IN  PEI_FAT_PRIVATE_DATA *PrivateData,
  IN  UINTN                ParentBlockDevNo
);

#define TOTAL_SIZE   (512 * 1024)
#define BLOCK_SIZE   (512)

VOID
FixBuffer (
  UINT8                   *TestBuffer,
  UINTN                   BufferSize,
  UINT32                  BlockSize
  )
{
  EFI_PARTITION_TABLE_HEADER   *PrimaryHeader;
  EFI_PARTITION_TABLE_HEADER   *BackupHeader;
  EFI_LBA                      LastBlock;
  UINT8                        *Ptr;

  PrimaryHeader = NULL;
  BackupHeader = NULL;

  LastBlock = (BufferSize + BlockSize - 1) / BlockSize - 1;

  PrimaryHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, BlockSize));
  if (PrimaryHeader->PartitionEntryLBA <= LastBlock -1 && MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockSize) + MultU64x32(PrimaryHeader->NumberOfPartitionEntries, PrimaryHeader->SizeOfPartitionEntry) < BufferSize) {
    Ptr = TestBuffer + MultU64x32(PrimaryHeader->PartitionEntryLBA, BlockSize);
    PrimaryHeader->PartitionEntryArrayCRC32 = CalculateCrc32(Ptr, (UINTN)MultU64x32(PrimaryHeader->NumberOfPartitionEntries, PrimaryHeader->SizeOfPartitionEntry));
  }

  if (PrimaryHeader->Header.HeaderSize + MultU64x32(PRIMARY_PART_HEADER_LBA, BlockSize) < BufferSize) {
    PrimaryHeader->Header.CRC32 = 0;
    PrimaryHeader->Header.CRC32 = CalculateCrc32(PrimaryHeader, PrimaryHeader->Header.HeaderSize);
  }


  BackupHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(LastBlock, BlockSize));
  if (BackupHeader->PartitionEntryLBA <= LastBlock - 1 && MultU64x32(BackupHeader->PartitionEntryLBA, BlockSize) + MultU64x32(BackupHeader->NumberOfPartitionEntries, BackupHeader->SizeOfPartitionEntry) < BufferSize) {
    Ptr = TestBuffer + MultU64x32(BackupHeader->PartitionEntryLBA, BlockSize);
    BackupHeader->PartitionEntryArrayCRC32 = CalculateCrc32(Ptr, (UINTN)MultU64x32(BackupHeader->NumberOfPartitionEntries, BackupHeader->SizeOfPartitionEntry));
  }

  if (BackupHeader->Header.HeaderSize + MultU64x32(LastBlock, BlockSize) < BufferSize) {
    BackupHeader->Header.CRC32 = 0;
    BackupHeader->Header.CRC32 = CalculateCrc32(BackupHeader, BackupHeader->Header.HeaderSize);
  }
}

VOID
FixBuffer0(
  UINT8                   *TestBuffer,
  UINTN                   BufferSize,
  UINT32                  BlockSize
)
{
  EFI_PARTITION_TABLE_HEADER   *PrimaryHeader;
  EFI_PARTITION_TABLE_HEADER   *BackupHeader;
  EFI_LBA                      LastBlock;

  PrimaryHeader = NULL;
  BackupHeader = NULL;

  LastBlock = (BufferSize + BlockSize - 1) / BlockSize - 1;

  PrimaryHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, BlockSize));
  PrimaryHeader->PartitionEntryArrayCRC32 = 0;
  PrimaryHeader->Header.CRC32 = 0;

  BackupHeader = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(LastBlock, BlockSize));
  BackupHeader->PartitionEntryArrayCRC32 = 0;
  BackupHeader->Header.CRC32 = 0;
}

EFI_STATUS
CreatePrivateData(
  UINT8                   *TestBuffer,
  UINTN                   BufferSize,
  UINT32                  BlockSize,
  UINTN                   ParentBlockDevNo,
  UINTN                   BlockDeviceCont,
  PEI_FAT_PRIVATE_DATA    **PrivateData
)
{
  PEI_FAT_PRIVATE_DATA    *TestPrivateData;
  TestPrivateData = malloc (sizeof(PEI_FAT_PRIVATE_DATA));
  if (TestPrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  memset (TestPrivateData, 0, sizeof(PEI_FAT_PRIVATE_DATA));

  TestPrivateData->BlockDevice[ParentBlockDevNo].BlockSize = BlockSize;
  TestPrivateData->BlockDevice[ParentBlockDevNo].LastBlock = (BufferSize + TestPrivateData->BlockDevice[ParentBlockDevNo].BlockSize - 1) / TestPrivateData->BlockDevice[ParentBlockDevNo].BlockSize - 1;
  TestPrivateData->BlockDevice[ParentBlockDevNo].StartingPos = (UINTN)TestBuffer;
  TestPrivateData->BlockDevice[ParentBlockDevNo].ParentDevNo = ParentBlockDevNo;
  TestPrivateData->BlockDeviceCount = BlockDeviceCont;

  *PrivateData = TestPrivateData;
  return EFI_SUCCESS;
}

VOID
TestGpt(
  UINT8                   *TestBuffer,
  UINTN                   TestBufferSize,
  UINT32                  BlockSize,
  UINTN                   ParentBlockDevNo,
  UINTN                   BlockDevCount
)
{
  PEI_FAT_PRIVATE_DATA   *PrivateData;

  PrivateData = NULL;
  if (EFI_ERROR(CreatePrivateData(TestBuffer, TestBufferSize, BlockSize, ParentBlockDevNo, BlockDevCount, &PrivateData)))
    return;

  // fuzz function:
  // buffer overflow, crash will be detected at place.
  // only care about security, not for function bug.
  // 
  // try to separate EFI lib, use stdlib function.
  // no asm code.
  FatFindGptPartitions(
    PrivateData,
    ParentBlockDevNo
  );
  free(PrivateData);
}

#ifndef TEST_WITH_KLEE
VOID
GetBlockSize(
  IN  UINT8  *TestBuffer,
  IN  UINTN  TestBufferSize,
  OUT UINT32 *RtnBlockSize
)
{
  UINT32                      BlockSize;
  EFI_PARTITION_TABLE_HEADER  *PartHdr;

  BlockSize = 512;
  if (MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)512) >= TestBufferSize)
    goto Done;
  PartHdr = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)512));
  if (PartHdr->Header.Signature == EFI_PTAB_HEADER_ID) {
    BlockSize = 512;
    goto Done;
  }
  if (MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)1024) >= TestBufferSize)
    goto Done;
  PartHdr = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)1024));
  if (PartHdr->Header.Signature == EFI_PTAB_HEADER_ID) {
    BlockSize = 1024;
    goto Done;
  }
  if (MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)2048) >= TestBufferSize)
    goto Done;
  PartHdr = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)2048));
  if (PartHdr->Header.Signature == EFI_PTAB_HEADER_ID) {
    BlockSize = 2048;
    goto Done;
  }
  if (MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)4096) >= TestBufferSize)
    goto Done;
  PartHdr = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)4096));
  if (PartHdr->Header.Signature == EFI_PTAB_HEADER_ID) {
    BlockSize = 4096;
    goto Done;
  }
  if (MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)8192) >= TestBufferSize)
    goto Done;
  PartHdr = (EFI_PARTITION_TABLE_HEADER*)(TestBuffer + MultU64x32(PRIMARY_PART_HEADER_LBA, (UINT32)8192));
  if (PartHdr->Header.Signature == EFI_PTAB_HEADER_ID) {
    BlockSize = 8192;
    goto Done;
  }
  BlockSize = 16384;
  
Done:
  *RtnBlockSize = BlockSize;
}
#endif

UINTN
EFIAPI
GetMaxBufferSize (
  VOID
  )
{
  return TOTAL_SIZE;
}

VOID
EFIAPI
RunTestHarness(
  IN VOID  *TestBuffer,
  IN UINTN TestBufferSize
  )
{
  UINT32                 BlockSize;
  
#ifdef TEST_WITH_KLEE
  BlockSize = BLOCK_SIZE;
#else
  GetBlockSize(TestBuffer, TestBufferSize, &BlockSize);
  FixBuffer (TestBuffer, TestBufferSize, BlockSize);
#endif
  TestGpt(TestBuffer, TestBufferSize, BlockSize, 63, 0);
  TestGpt(TestBuffer, TestBufferSize, BlockSize, 0, 64);
  TestGpt(TestBuffer, TestBufferSize, BlockSize, 0, 0);
}
