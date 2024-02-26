/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef TEST_WITH_LIBFUZZER
#include <stdint.h>
#include <stddef.h>
#endif

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmMemLibStubLib.h>

extern BOOLEAN                      mEndOfDxe;

#define TOTAL_SIZE (512 * 1024)

extern UINT8                                                *mVariableBufferPayload;
extern UINTN                                                mVariableBufferPayloadSize;

EFI_STATUS
EFIAPI
SmmVariableHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  );

VOID
FixBuffer (
  UINT8                   *TestBuffer,
  UINTN                   TestBufferSize
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
  SMM_COMMUNICATION_BUFFER_DESCRIPTOR  SmmCommBufferDesc[1];

  SmmCommBufferDesc[0].Address = (UINTN)TestBuffer;
  SmmCommBufferDesc[0].Size = TOTAL_SIZE;

  FixBuffer (TestBuffer, TestBufferSize);

  SmmMemLibInitialize (ARRAY_SIZE(SmmCommBufferDesc), SmmCommBufferDesc);
  mEndOfDxe = TRUE;

  mVariableBufferPayloadSize = GetMaxBufferSize();
  mVariableBufferPayload = AllocatePool(mVariableBufferPayloadSize);
  if (mVariableBufferPayload == NULL)
	  return;

  SmmVariableHandler(NULL, NULL, TestBuffer, &TestBufferSize);

  if (mVariableBufferPayload != NULL)
	  FreePool(mVariableBufferPayload);
}
