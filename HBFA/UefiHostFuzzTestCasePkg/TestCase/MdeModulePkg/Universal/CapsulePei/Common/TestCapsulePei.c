/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>
#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "CommonHeader.h"

#define TOTAL_SIZE (512 * 1024)

EFI_STATUS
EFIAPI
CapsuleDataCoalesce (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PHYSICAL_ADDRESS            *BlockListBuffer,
  IN MEMORY_RESOURCE_DESCRIPTOR      *MemoryResource,
  IN OUT VOID                        **MemoryBase,
  IN OUT UINTN                       *MemorySize
  );

VOID
FixBuffer (
  UINT8                   *TestBuffer,
  UINTN                   TestBufferSize
  )
{
  ((EFI_CAPSULE_HEADER *)TestBuffer)->CapsuleImageSize = (UINT32)TestBufferSize;
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
  MEMORY_RESOURCE_DESCRIPTOR     MemoryResource[3];
  VOID                           *MemoryBase;
  UINTN                          MemorySize;
  VOID                           *CapsuleMemoryBase;
  UINTN                          CapsuleMemorySize;

  MemorySize = TOTAL_SIZE;
  MemoryBase = AllocatePool (MemorySize);

  MemoryResource[0].PhysicalStart  = (UINT64)(UINTN)TestBuffer;
  MemoryResource[0].ResourceLength = (UINT64)(UINTN)TOTAL_SIZE;
  MemoryResource[1].PhysicalStart  = (UINT64)(UINTN)MemoryBase;
  MemoryResource[1].ResourceLength = (UINT64)(UINTN)MemorySize;
  MemoryResource[2].PhysicalStart  = 0;
  MemoryResource[2].ResourceLength = 0;

  FixBuffer (TestBuffer, TestBufferSize);

  CapsuleMemoryBase = MemoryBase;
  CapsuleMemorySize = MemorySize;
  CapsuleDataCoalesce(NULL, TestBuffer, MemoryResource, &CapsuleMemoryBase, &CapsuleMemorySize);

  FreePool (MemoryBase);
}

