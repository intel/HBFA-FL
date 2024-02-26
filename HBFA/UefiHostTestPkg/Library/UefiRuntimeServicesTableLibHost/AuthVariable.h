/**
  Header file to Manage & verify Timebased Authenticated varaibles.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _AUTH_VARIABLE_H
#define _AUTH_VARIABLE_H

#include "OsVariable.h"
#include "VariableCommon.h"
#include "AuthVarCertDB.h"

/*
  Process variable with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set.

  @param[in]    Name                      Name of Variable to be processed.
  @param[in]    Guid                      Variable vendor GUID.
  @param[in]    Data                      Data pointer.
  @param[in]    DataSize                  The size of Data in bytes.
  @param[in]    Attributes                Attribute value of the variable.
  @param[in]    OperType                  The operation type about variable.

  @retval       EFI_SUCCESS               The operation is finished successfully.
  @retval       EFI_INVALID_PARAMETER     Invalid input parameters, Name, Guid or Data is NULL.
  @retval       EFI_OUT_OF_RESOURCES      No more memory for allocation.
  @retval       Others                    Other errors as indicated.
*/
EFI_STATUS
ProcessAuthVar (
  IN    CHAR16                      *Name,
  IN    EFI_GUID                    *Guid,
  IN    VOID                        *Data,
  IN    UINTN                       DataSize,
  IN    UINT32                      Attributes,
  IN    EFI_COMMAND_OPER_TYPE       OperType
  );

#endif
