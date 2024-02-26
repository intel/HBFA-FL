/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _OS_VARIABLE_H_
#define _OS_VARIABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/ImageAuthentication.h>
#include <Library/AuthVariableLib.h>

//
// ASN.1
//
#define TWO_BYTE_ENCODE       0x82

#define SHA256_DIGEST_SIZE  32

#define VARIABLE_INFO_PRIVATE_SIGNATURE  SIGNATURE_32 ('V', 'K', 'E', 'S')

typedef struct {
  UINTN            Signature;
  LIST_ENTRY       Link;

  EFI_GUID         Guid;
  CHAR16           *Name;
  UINT32           Attributes;
  EFI_TIME         TimeStamp;
  UINTN            Size;
  UINT8            *Buffer;
} VARIABLE_INFO_PRIVATE;

#define VARIABLE_INFO_PRIVATE_FROM_LINK(a)  CR (a, VARIABLE_INFO_PRIVATE, Link, VARIABLE_INFO_PRIVATE_SIGNATURE)

#define FREE_NON_NULL_PTR(Pointer)      \
  do {                                  \
    if ((Pointer) != NULL) {            \
      free ((Pointer));                 \
      (Pointer) = NULL;                 \
    }                                   \
  } while(FALSE)

//
//  Command operation type.
//
typedef enum {
  VariableTypeAdd,                // Indicate to create variable.
  VariableTypeDel,                // Indicate to delete.
  VariableTypeAppend,             // Indicate to append.
  VariableTypeMax                 // Invalid type
} EFI_COMMAND_OPER_TYPE;

BOOLEAN
CompareTimeStamp (
  IN EFI_TIME               *FirstTime,
  IN EFI_TIME               *SecondTime
  );

#endif
