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

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PiDxe.h>
#include <Library/SafeIntLib.h>
#include <IndustryStandard/Bmp.h>
#include <Library/BmpSupportLib.h>

#define TOTAL_SIZE (1 * 1024)

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
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *GopBlt;
  UINTN                          GopBltSize;
  UINTN                          PixelHeight;
  UINTN                          PixelWidth;
  FixBuffer (TestBuffer);
  GopBlt = NULL;
  TranslateBmpToGopBlt(
            TestBuffer,
            TestBufferSize,
            &GopBlt,
            &GopBltSize,
            &PixelHeight,
            &PixelWidth
            );
  if (GopBlt != NULL)
    FreePool (GopBlt); 
}
