/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#define MAX_FILENAME_LEN 4096

CHAR16 *
MangleFileName (
  IN CHAR16        *FileName
  );

UINTN
EFIAPI
GetMaxBufferSize (
  VOID
  )
{
  return MAX_FILENAME_LEN;
}

VOID
EFIAPI
RunTestHarness(
  IN VOID  *TestBuffer,
  IN UINTN TestBufferSize
  )
{
  MangleFileName((CHAR16*)TestBuffer);
}
