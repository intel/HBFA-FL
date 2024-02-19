/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _DISK_STUB_LIB_H_
#define _DISK_STUB_LIB_H_

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>

EFI_STATUS
EFIAPI
DiskStubInitialize (
  IN  VOID                   *Buffer,
  IN  UINTN                  BufferSize,
  IN  UINT32                 BlockSize,
  IN  UINT32                 IoAlign,
  OUT EFI_BLOCK_IO_PROTOCOL  **BlockIo,
  OUT EFI_DISK_IO_PROTOCOL   **DiskIo
  );
  
EFI_STATUS
EFIAPI
DiskStubDestory (
  VOID
  );

#endif
