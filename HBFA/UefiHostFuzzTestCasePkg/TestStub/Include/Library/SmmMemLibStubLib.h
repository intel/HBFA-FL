/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SMM_MEM_LIB_LIB_LIB_H_
#define _SMM_MEM_LIB_LIB_LIB_H_

#include <Uefi.h>

typedef struct {
  EFI_PHYSICAL_ADDRESS  Address;
  UINT64                Size;
} SMM_COMMUNICATION_BUFFER_DESCRIPTOR;

VOID
EFIAPI
SmmMemLibInitialize (
  IN UINTN                               SmmCommBufferDescCount,
  IN SMM_COMMUNICATION_BUFFER_DESCRIPTOR *SmmCommBufferDesc
  );

#endif
