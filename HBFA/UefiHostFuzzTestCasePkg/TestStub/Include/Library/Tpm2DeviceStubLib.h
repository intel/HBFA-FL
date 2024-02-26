/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TPM2_DEVICE_STUB_LIB_H_
#define _TPM2_DEVICE_STUB_LIB_H_

#include <Uefi.h>

EFI_STATUS
EFIAPI
Tpm2ResponseInitialize (
  IN  VOID                   *Buffer,
  IN  UINTN                  BufferSize
  );

#endif
