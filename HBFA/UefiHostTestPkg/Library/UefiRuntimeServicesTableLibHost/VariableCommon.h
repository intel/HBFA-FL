/** @file
  The header of common Variable.c

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __VARIABLE_COMMON_H__
#define __VARIABLE_COMMON_H__

typedef enum {
  VariableTypeNormal,
  VariableTypeCountBasedAuth,
  VariableTypeTimeBasedAuth,
  VariableTypeUnknown,
} VARIABLE_TYPE;

EFI_STATUS
CreateVariableList (
  IN  LIST_ENTRY     *StorageListHead,
  IN  CHAR16         *Name,
  IN  EFI_GUID       *Guid,
  IN  UINT32         Attributes,
  IN  EFI_TIME       *TimeStamp,
  IN  UINTN          Size,
  IN  UINT8          *Buffer,
  IN  BOOLEAN        Append
  );

EFI_STATUS
DeleteVariableList (
  IN  LIST_ENTRY     *StorageListHead,
  IN  CHAR16         *Name,
  IN  EFI_GUID       *Guid
  );

LIST_ENTRY *
FindVariableList (
  IN  LIST_ENTRY     *StorageListHead,
  IN  CHAR16         *Name,
  IN  EFI_GUID       *Guid
  );

VARIABLE_INFO_PRIVATE*
FindVariableInfoPtr(
  IN    CHAR16        *VariableName,
  IN    EFI_GUID      *VendorGuid
  );

#endif // _EFI_VARIABLE_COMMON_H_
