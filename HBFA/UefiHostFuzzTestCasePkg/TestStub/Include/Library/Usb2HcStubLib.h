/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB2_HC_STUB_LIB_H_
#define _USB2_HC_STUB_LIB_H_

#include <Uefi.h>
#include <Protocol/Usb2HostController.h>

EFI_STATUS
EFIAPI
Usb2HcStubInitialize (
  IN  VOID                  *DeviceDescBuffer,
  IN  UINTN                 DeviceDescBufferSize,
  IN  VOID                  *ConfigDescBuffer,
  IN  UINTN                 ConfigDescBufferSize,
  IN  VOID                  *StringDescBuffer,
  IN  UINTN                 StringDescBufferSize,
  OUT EFI_USB2_HC_PROTOCOL  **Usb2Hc
  );

#endif
