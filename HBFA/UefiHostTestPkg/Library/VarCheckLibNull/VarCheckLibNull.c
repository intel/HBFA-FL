/** @file
  Implementation functions and structures for var check services.

Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/VarCheckLib.h>

/**
  Var check initialize at END_OF_DXE.

  This function needs to be called at END_OF_DXE.
  Address pointers may be returned,
  and caller needs to ConvertPointer() for the pointers.

  @param[in, out] AddressPointerCount   Output pointer to address pointer count.

  @return Address pointer buffer, NULL if input AddressPointerCount is NULL.

**/
VOID ***
EFIAPI
VarCheckLibInitializeAtEndOfDxe (
  IN OUT UINTN                  *AddressPointerCount OPTIONAL
  )
{
  return NULL;
}

EFI_STATUS
EFIAPI
VarCheckLibSetVariableCheck (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid,
  IN UINT32                     Attributes,
  IN UINTN                      DataSize,
  IN VOID                       *Data,
  IN VAR_CHECK_REQUEST_SOURCE   RequestSource
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
VarCheckLibVariablePropertyGet (
  IN CHAR16                         *Name,
  IN EFI_GUID                       *Guid,
  OUT VAR_CHECK_VARIABLE_PROPERTY   *VariableProperty
  )
{
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
VarCheckLibVariablePropertySet (
  IN CHAR16                         *Name,
  IN EFI_GUID                       *Guid,
  IN VAR_CHECK_VARIABLE_PROPERTY    *VariableProperty
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
VarCheckLibRegisterSetVariableCheckHandler (
  IN VAR_CHECK_SET_VARIABLE_CHECK_HANDLER   Handler
  )
{
  return EFI_SUCCESS;
}

