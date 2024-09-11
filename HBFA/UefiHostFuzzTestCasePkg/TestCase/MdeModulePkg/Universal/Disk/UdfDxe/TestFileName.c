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
  UINT8 *_TestBuffer = (UINT8*)TestBuffer;
  UINTN Index;

  // Only caller in edk2 ensures string is properly NULL-terminated
  if ( TestBufferSize > MAX_FILENAME_LEN-sizeof(L'\0') )
    Index = MAX_FILENAME_LEN - sizeof(L'\0');
  else
    Index = TestBufferSize;

  _TestBuffer[Index] = L'\0';

  MangleFileName((CHAR16*)TestBuffer);
}
