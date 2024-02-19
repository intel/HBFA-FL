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
#include <Library/UsbIoPpiStubLib.h>

#include "UsbPeim.h"
#include "HubPeim.h"
#include "PeiUsbLib.h"

#define TOTAL_SIZE (512 * 1024)

UINT32
GetUsbTransferTimeoutValue (
  VOID
  )
{
  return 1;
}

EFI_STATUS
PeiUsbGetAllConfiguration (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN PEI_USB_DEVICE     *PeiUsbDevice
  );

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
  PEI_USB_DEVICE                PeiUsbDevice;
  PEI_USB_IO_PPI                *UsbIoPpi;
  
  ZeroMem(&PeiUsbDevice, sizeof(PeiUsbDevice));

  UsbIoPpiStubInitialize(NULL, 0, TestBuffer, TestBufferSize, NULL, 0, &UsbIoPpi);

  CopyMem(&PeiUsbDevice.UsbIoPpi, UsbIoPpi, sizeof(*UsbIoPpi));

  PeiUsbGetAllConfiguration(NULL, &PeiUsbDevice);
}


