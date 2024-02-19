/** @file
  SMM Services Table Library.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "PiSmmCore.h"

extern EFI_SMM_SYSTEM_TABLE2  gSmmCoreSmst;

EFI_SMM_SYSTEM_TABLE2  *gSmst      = &gSmmCoreSmst;
EFI_MM_SYSTEM_TABLE    *gMmst      = (EFI_MM_SYSTEM_TABLE *)&gSmmCoreSmst;

EFI_STATUS
EFIAPI
SmmAllocatePages (
  IN  EFI_ALLOCATE_TYPE     Type,
  IN  EFI_MEMORY_TYPE       MemoryType,
  IN  UINTN                 NumberOfPages,
  OUT EFI_PHYSICAL_ADDRESS  *Memory
  )
{
  VOID *Buffer;
  
  if ((NumberOfPages == 0) ||
      (NumberOfPages > RShiftU64 ((UINTN)-1, EFI_PAGE_SHIFT))) {
    return EFI_NOT_FOUND;
  }

  Buffer = malloc (EFI_PAGES_TO_SIZE(NumberOfPages));
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  *Memory = (UINTN)Buffer;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmFreePages (
  IN EFI_PHYSICAL_ADDRESS  Memory,
  IN UINTN                 NumberOfPages
  )
{
  free ((VOID *)(UINTN)Memory);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmAllocatePool (
  IN EFI_MEMORY_TYPE  PoolType,
  IN UINTN            Size,
  OUT VOID            **Buffer
  )
{
  *Buffer = malloc (Size);
  if (*Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmFreePool (
  IN VOID  *Buffer
  )
{
  free (Buffer);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmiManage (
  IN     CONST EFI_GUID           *HandlerType,
  IN     CONST VOID               *Context         OPTIONAL,
  IN OUT VOID                     *CommBuffer      OPTIONAL,
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
  )
{
  return EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
SmiHandlerRegister (
  IN  EFI_SMM_HANDLER_ENTRY_POINT2  Handler,
  IN  CONST EFI_GUID                *HandlerType  OPTIONAL,
  OUT EFI_HANDLE                    *DispatchHandle
  )
{
  return EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
SmiHandlerUnRegister (
  IN EFI_HANDLE  DispatchHandle
  )
{
  return EFI_NOT_AVAILABLE_YET;
}

