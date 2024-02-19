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
#include <Library/Usb2HcStubLib.h>

#include "UsbBus.h"

#define TOTAL_SIZE (512 * 1024)

EFI_STATUS
UsbBuildDescTable (
  IN USB_DEVICE           *UsbDev
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
  USB_DEVICE           UsbDev;
  USB_BUS              Bus;
  EFI_USB2_HC_PROTOCOL *Usb2Hc;
  
  Usb2HcStubInitialize (NULL, 0, TestBuffer, TestBufferSize, NULL, 0, &Usb2Hc);

  UsbDev.Bus = &Bus;
  Bus.Usb2Hc = Usb2Hc;

  UsbBuildDescTable (&UsbDev);
}


