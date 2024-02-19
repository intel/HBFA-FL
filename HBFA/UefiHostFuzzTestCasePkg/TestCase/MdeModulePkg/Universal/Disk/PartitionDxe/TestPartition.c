/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>

#include <IndustryStandard/Udf.h>
#include <IndustryStandard/ElTorito.h>

#define MAX_CORRECTION_BLOCKS_NUM 512u

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DiskStubLib.h>

EFI_STATUS
FindUdfFileSystem (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_DISK_IO_PROTOCOL   *DiskIo,
  OUT EFI_LBA               *StartingLBA,
  OUT EFI_LBA               *EndingLBA
  );

#define TOTAL_SIZE   (512 * 1024)
#define BLOCK_SIZE   (512)
#define IO_ALIGN     (1)

VOID
FixBuffer (
  UINT8                   *TestBuffer
  )
{
}
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
  EFI_LBA                StartingLBA;
  EFI_LBA                EndingLBA;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo;
  EFI_DISK_IO_PROTOCOL   *DiskIo;

  FixBuffer (TestBuffer);

  DiskStubInitialize (TestBuffer, TestBufferSize, BLOCK_SIZE, IO_ALIGN, &BlockIo, &DiskIo);

  // fuzz function:
  // buffer overflow, crash will be detected at place.
  // only care about security, not for function bug.
  // 
  // try to separate EFI lib, use stdlib function.
  // no asm code.
  FindUdfFileSystem (
            BlockIo,
            DiskIo,
            &StartingLBA,
            &EndingLBA
            );

  DiskStubDestory();
}
