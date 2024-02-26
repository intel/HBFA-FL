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

#include <Library/FmpAuthenticationLib.h>

#define TOTAL_SIZE (512 * 1024)

extern UINT8 mRsa2048Sha256PublicKeyData[];
extern UINTN mRsa2048Sha256PublicKeyDataSize;

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
  FixBuffer (TestBuffer);
  AuthenticateFmpImage (TestBuffer, TestBufferSize, mRsa2048Sha256PublicKeyData, mRsa2048Sha256PublicKeyDataSize);
}
