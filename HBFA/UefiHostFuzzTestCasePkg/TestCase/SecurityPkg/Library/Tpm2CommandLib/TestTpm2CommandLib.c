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

#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm2DeviceStubLib.h>

#define TOTAL_SIZE (512 * 1024)

#pragma pack(1)

typedef struct {
  TPM2_RESPONSE_HEADER       Header;
  UINT32                     ParameterSize;
  TPML_DIGEST_VALUES         Digests;
  TPMS_AUTH_RESPONSE         AuthSessionPcr;
} TPM2_PCR_EVENT_RESPONSE;

#pragma pack()

VOID
FixBuffer (
  UINT8                   *TestBuffer
  )
{
  TPM2_PCR_EVENT_RESPONSE  *Res;

  Res = (VOID *)TestBuffer;
  Res->Header.responseCode = SwapBytes32(TPM_RC_SUCCESS);
  Res->Header.paramSize = SwapBytes32 (sizeof(TPM2_PCR_EVENT_RESPONSE));
}

VOID
TestTpm2PcrEve (
  VOID
  )
{
  TPMI_DH_PCR               PcrHandle;
  TPM2B_EVENT               EventData;
  TPML_DIGEST_VALUES        Digests;

  PcrHandle = 0;
  ZeroMem (&EventData, sizeof(EventData));
  EventData.size = 4;
  ZeroMem (&Digests, sizeof(Digests));
  Tpm2PcrEvent (PcrHandle, &EventData, &Digests);
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
  Tpm2ResponseInitialize (TestBuffer, TestBufferSize);

  TestTpm2PcrEve ();
}
