/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _USB_IO_PPI_STUB_LIB_H_
#define _USB_IO_PPI_STUB_LIB_H_

#include <Uefi.h>
#include <Ppi/UsbIo.h>

EFI_STATUS
EFIAPI
UsbIoPpiStubInitialize (
  IN  VOID                         *DeviceDescBuffer,
  IN  UINTN                        DeviceDescBufferSize,
  IN  VOID                         *ConfigDescBuffer,
  IN  UINTN                        ConfigDescBufferSize,
  IN  VOID                         *StringDescBuffer,
  IN  UINTN                        StringDescBufferSize,
  OUT PEI_USB_IO_PPI               **UsbIoPpi
  );

#endif
