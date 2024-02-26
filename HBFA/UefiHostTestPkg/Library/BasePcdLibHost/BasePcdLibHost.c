/** @file
  A emptry template implementation of PCD Library.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

#define PCD_INFO_PRIVATE_SIGNATURE  SIGNATURE_32 ('P', 'I', 'P', 'S')

typedef enum {
  PcdTypeUnknown,
  PcdTypeBool,
  PcdTypeUint8,
  PcdTypeUint16,
  PcdTypeUint32,
  PcdTypeUint64,
  PcdTypePtr,
} PCD_TYPE_PRIVATE;

typedef struct {
  UINTN            Signature;
  LIST_ENTRY       Link;

  UINTN            TokenNumber;
  GUID             Guid;
  PCD_TYPE_PRIVATE Type;
  UINTN            Size;
  UINT8            *Buffer;
} PCD_INFO_PRIVATE;

#define PCD_INFO_PRIVATE_FROM_LINK(a)  CR (a, PCD_INFO_PRIVATE, Link, PCD_INFO_PRIVATE_SIGNATURE)

LIST_ENTRY                  mPcdListEntry = INITIALIZE_LIST_HEAD_VARIABLE(mPcdListEntry);
LIST_ENTRY                  mPcdExListEntry = INITIALIZE_LIST_HEAD_VARIABLE(mPcdExListEntry);

LIST_ENTRY *
FindPcdList (
  IN LIST_ENTRY       *StorageListHead,
  IN GUID             *Guid,
  IN UINTN            TokenNumber,
  IN PCD_TYPE_PRIVATE Type
  )
{
  LIST_ENTRY         *Link;
  PCD_INFO_PRIVATE   *Storage;

  if (StorageListHead->ForwardLink != NULL) {
    Link = GetFirstNode (StorageListHead);
    while (!IsNull (StorageListHead, Link)) {
      Storage = PCD_INFO_PRIVATE_FROM_LINK (Link);
      if ((Storage->TokenNumber == TokenNumber) &&
          ((Guid == NULL) || (CompareGuid (&Storage->Guid, Guid))) ) {
        if (Type != PcdTypeUnknown) {
          ASSERT (Storage->Type == Type);
          if (Storage->Type != Type) {
            return NULL;
          }
        }
        return Link;
      }
      Link = GetNextNode (StorageListHead, Link);
    }
  }
  return NULL;
}

PCD_INFO_PRIVATE*
FindPcdInfoPtr(
  IN UINTN            TokenNumber,
  IN PCD_TYPE_PRIVATE Type
  )
{
  LIST_ENTRY          *ListEntry;

  //
  //  Try to get auth variable by name and GUID.
  //
  ListEntry = FindPcdList (&mPcdListEntry, NULL, TokenNumber, Type);
  if (ListEntry != NULL) {
    return PCD_INFO_PRIVATE_FROM_LINK (ListEntry);
  }

  return NULL;
}

PCD_INFO_PRIVATE*
FindPcdExInfoPtr(
  IN CONST GUID       *Guid,
  IN UINTN            TokenNumber,
  IN PCD_TYPE_PRIVATE Type
  )
{
  LIST_ENTRY          *ListEntry;

  //
  //  Try to get auth variable by name and GUID.
  //
  ListEntry = FindPcdList (&mPcdExListEntry, (GUID *)Guid, TokenNumber, Type);
  if (ListEntry != NULL) {
    return PCD_INFO_PRIVATE_FROM_LINK (ListEntry);
  }

  return NULL;
}

RETURN_STATUS
CreatePcdInfo (
  IN GUID              *Guid,
  IN UINTN             TokenNumber,
  IN PCD_TYPE_PRIVATE  Type,
  IN VOID              *Buffer,
  IN UINTN             Size,
  OUT PCD_INFO_PRIVATE **PcdInfoPrivate
  )
{
  PCD_INFO_PRIVATE  *PcdInfo;

  switch (Type) {
  case PcdTypeUnknown:
  default:
    ASSERT(FALSE);
    return RETURN_INVALID_PARAMETER;
    break;
  case PcdTypeUint8:
  case PcdTypeBool:
    ASSERT(Size == sizeof(UINT8));
    if (Size != sizeof(UINT8)) {
      return RETURN_INVALID_PARAMETER;
    }
    break;
  case PcdTypeUint16:
    ASSERT(Size == sizeof(UINT16));
    if (Size != sizeof(UINT16)) {
      return RETURN_INVALID_PARAMETER;
    }
    break;
  case PcdTypeUint32:
    ASSERT(Size == sizeof(UINT32));
    if (Size != sizeof(UINT32)) {
      return RETURN_INVALID_PARAMETER;
    }
    break;
  case PcdTypeUint64:
    ASSERT(Size == sizeof(UINT64));
    if (Size != sizeof(UINT64)) {
      return RETURN_INVALID_PARAMETER;
    }
    break;
  case PcdTypePtr:
    break;
  }

  if (Guid == NULL) {
    PcdInfo = FindPcdInfoPtr(TokenNumber, Type);
  } else {
    PcdInfo = FindPcdExInfoPtr(Guid, TokenNumber, Type);
  }

  if (PcdInfo == NULL) {
    PcdInfo = malloc (sizeof(PCD_INFO_PRIVATE));
    if (PcdInfo == NULL) {
      return RETURN_OUT_OF_RESOURCES;
    }
    ZeroMem (PcdInfo, sizeof(PCD_INFO_PRIVATE));
    PcdInfo->Signature = PCD_INFO_PRIVATE_SIGNATURE;
    if (Guid != NULL) {
      CopyMem (&PcdInfo->Guid, Guid, sizeof(GUID));
    }
    PcdInfo->TokenNumber = TokenNumber;
    PcdInfo->Type = Type;
    PcdInfo->Size = Size;
    PcdInfo->Buffer = malloc(Size);
    if (PcdInfo->Buffer == NULL) {
      free (PcdInfo);
      return RETURN_OUT_OF_RESOURCES;
    }
    CopyMem (PcdInfo->Buffer, Buffer, Size);
    *PcdInfoPrivate = PcdInfo;
    return RETURN_SUCCESS;
  } else {
    if (Type != PcdInfo->Type) {
      ASSERT(FALSE);
      return RETURN_INVALID_PARAMETER;
    }
    if (Type != PcdTypePtr && (Size != PcdInfo->Size)) {
      ASSERT(FALSE);
      return RETURN_INVALID_PARAMETER;
    }
    if (Type == PcdTypePtr) {
      VOID *NewBuf;
      NewBuf = malloc(Size);
      if (NewBuf == NULL) {
        return RETURN_OUT_OF_RESOURCES;
      }
      free (PcdInfo->Buffer);
      PcdInfo->Buffer = NewBuf;
      PcdInfo->Size = Size;
    }
    CopyMem (PcdInfo->Buffer, Buffer, Size);
    *PcdInfoPrivate = NULL;
    return RETURN_SUCCESS;
  }
}

RETURN_STATUS
InsertPcd (
  IN UINTN            TokenNumber,
  IN PCD_TYPE_PRIVATE Type,
  IN VOID             *Buffer,
  IN UINTN            Size
  )
{
  PCD_INFO_PRIVATE    *PcdInfo;
  RETURN_STATUS       Status;

  Status = CreatePcdInfo (NULL, TokenNumber, Type, Buffer, Size, &PcdInfo);
  if (Status != RETURN_SUCCESS) {
    ASSERT(FALSE);
    return Status;
  }
  if (PcdInfo != NULL) {
    InsertTailList(&mPcdListEntry, &PcdInfo->Link);
  }

  return RETURN_SUCCESS;
}

RETURN_STATUS
InsertPcdEx (
  IN CONST GUID       *Guid,
  IN UINTN            TokenNumber,
  IN PCD_TYPE_PRIVATE Type,
  IN VOID             *Buffer,
  IN UINTN            Size
  )
{
  PCD_INFO_PRIVATE    *PcdInfo;
  RETURN_STATUS       Status;

  Status = CreatePcdInfo ((GUID *)Guid, TokenNumber, Type, Buffer, Size, &PcdInfo);
  if (Status != RETURN_SUCCESS) {
    ASSERT(FALSE);
    return RETURN_OUT_OF_RESOURCES;
  }
  if (PcdInfo != NULL) {
    InsertTailList(&mPcdExListEntry, &PcdInfo->Link);
  }

  return RETURN_SUCCESS;
}

/**
  This function provides a means by which SKU support can be established in the PCD infrastructure.

  Sets the current SKU in the PCD database to the value specified by SkuId.  SkuId is returned.

  @param[in]  SkuId The SKU value that will be used when the PCD service will retrieve and
                    set values associated with a PCD token.

  @return Return the SKU ID that just be set.

**/
UINTN
EFIAPI
LibPcdSetSku (
  IN UINTN   SkuId
  )
{
  ASSERT (FALSE);

  return 0;
}

/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 8-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 8-bit value for the token specified by TokenNumber.

**/
UINT8
EFIAPI
LibPcdGet8 (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypeUint8);
  if (PcdInfo != NULL) {
    return *(UINT8 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 16-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 16-bit value for the token specified by TokenNumber.

**/
UINT16
EFIAPI
LibPcdGet16 (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypeUint16);
  if (PcdInfo != NULL) {
    return *(UINT16 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 32-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 32-bit value for the token specified by TokenNumber.

**/
UINT32
EFIAPI
LibPcdGet32 (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypeUint32);
  if (PcdInfo != NULL) {
    return *(UINT32 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 64-bit value for the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the 64-bit value for the token specified by TokenNumber.

**/
UINT64
EFIAPI
LibPcdGet64 (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypeUint64);
  if (PcdInfo != NULL) {
    return *(UINT64 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the pointer to the buffer of the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the pointer to the token specified by TokenNumber.

**/
VOID *
EFIAPI
LibPcdGetPtr (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypePtr);
  if (PcdInfo != NULL) {
    return PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the Boolean value of the token specified by TokenNumber.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the Boolean value of the token specified by TokenNumber.

**/
BOOLEAN
EFIAPI
LibPcdGetBool (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypeBool);
  if (PcdInfo != NULL) {
    return *(BOOLEAN *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve the size of a given PCD token.

  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Returns the size of the token specified by TokenNumber.

**/
UINTN
EFIAPI
LibPcdGetSize (
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdInfoPtr (TokenNumber, PcdTypeUnknown);
  if (PcdInfo != NULL) {
    return PcdInfo->Size;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 8-bit value for the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid The pointer to a 128-bit unique value that designates
              which namespace to retrieve a value from.
  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Return the UINT8.

**/
UINT8
EFIAPI
LibPcdGetEx8 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypeUint8);
  if (PcdInfo != NULL) {
    return *(UINT8 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 16-bit value for the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid The pointer to a 128-bit unique value that designates
              which namespace to retrieve a value from.
  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Return the UINT16.

**/
UINT16
EFIAPI
LibPcdGetEx16 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypeUint16);
  if (PcdInfo != NULL) {
    return *(UINT16 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  Returns the 32-bit value for the token specified by TokenNumber and Guid.
  If Guid is NULL, then ASSERT().

  @param[in]  Guid The pointer to a 128-bit unique value that designates
              which namespace to retrieve a value from.
  @param[in]  TokenNumber The PCD token number to retrieve a current value for.

  @return Return the UINT32.

**/
UINT32
EFIAPI
LibPcdGetEx32 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypeUint32);
  if (PcdInfo != NULL) {
    return *(UINT32 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the 64-bit value for the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the UINT64.

**/
UINT64
EFIAPI
LibPcdGetEx64 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypeUint64);
  if (PcdInfo != NULL) {
    return *(UINT64 *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the pointer to the buffer of token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the VOID* pointer.

**/
VOID *
EFIAPI
LibPcdGetExPtr (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypePtr);
  if (PcdInfo != NULL) {
    return PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve a value for a given PCD token.

  Returns the Boolean value of the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the BOOLEAN.

**/
BOOLEAN
EFIAPI
LibPcdGetExBool (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypeBool);
  if (PcdInfo != NULL) {
    return *(BOOLEAN *)PcdInfo->Buffer;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to retrieve the size of a given PCD token.

  Returns the size of the token specified by TokenNumber and Guid.

  If Guid is NULL, then ASSERT().

  @param[in]  Guid          The pointer to a 128-bit unique value that designates
                            which namespace to retrieve a value from.
  @param[in]  TokenNumber   The PCD token number to retrieve a current value for.

  @return Return the size.

**/
UINTN
EFIAPI
LibPcdGetExSize (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber
  )
{
  PCD_INFO_PRIVATE     *PcdInfo;

  PcdInfo = FindPcdExInfoPtr (Guid, TokenNumber, PcdTypeUnknown);
  if (PcdInfo != NULL) {
    return PcdInfo->Size;
  }

  ASSERT (PcdInfo != NULL);
  return 0;
}



/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 8-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet8S (
  IN UINTN          TokenNumber,
  IN UINT8          Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcd (TokenNumber, PcdTypeUint8, &Value, sizeof(UINT8));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 16-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet16S (
  IN UINTN          TokenNumber,
  IN UINT16         Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcd (TokenNumber, PcdTypeUint16, &Value, sizeof(UINT16));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 32-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet32S (
  IN UINTN          TokenNumber,
  IN UINT32         Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcd (TokenNumber, PcdTypeUint32, &Value, sizeof(UINT32));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 64-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSet64S (
  IN UINTN          TokenNumber,
  IN UINT64         Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcd (TokenNumber, PcdTypeUint64, &Value, sizeof(UINT64));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value specified
  by Buffer and SizeOfBuffer. If SizeOfBuffer is greater than the maximum size
  support by TokenNumber, then set SizeOfBuffer to the maximum size supported by
  TokenNumber and return EFI_INVALID_PARAMETER to indicate that the set operation
  was not actually performed.

  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to the
  maximum size supported by TokenName and EFI_INVALID_PARAMETER must be returned.

  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetPtrS (
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN CONST VOID     *Buffer
  )
{
  RETURN_STATUS Status;

  Status = InsertPcd (TokenNumber, PcdTypePtr, (VOID *)Buffer, *SizeOfBuffer);
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the boolean value for the token specified by TokenNumber
  to the value specified by Value.

  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The boolean value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetBoolS (
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcd (TokenNumber, PcdTypeBool, &Value, sizeof(BOOLEAN));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 8-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 8-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx8S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT8          Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcdEx (Guid, TokenNumber, PcdTypeUint8, &Value, sizeof(UINT8));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 16-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 16-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx16S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT16         Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcdEx (Guid, TokenNumber, PcdTypeUint16, &Value, sizeof(UINT16));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 32-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 32-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx32S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT32         Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcdEx (Guid, TokenNumber, PcdTypeUint32, &Value, sizeof(UINT32));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the 64-bit value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The 64-bit value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetEx64S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT64         Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcdEx (Guid, TokenNumber, PcdTypeUint64, &Value, sizeof(UINT64));
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets a buffer for the token specified by TokenNumber to the value specified by
  Buffer and SizeOfBuffer. If SizeOfBuffer is greater than the maximum size
  support by TokenNumber, then set SizeOfBuffer to the maximum size supported by
  TokenNumber and return EFI_INVALID_PARAMETER to indicate that the set operation
  was not actually performed.

  If Guid is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[in]      Guid          Pointer to a 128-bit unique value that
                                designates which namespace to set a value from.
  @param[in]      TokenNumber   The PCD token number to set a current value for.
  @param[in, out] SizeOfBuffer  The size, in bytes, of Buffer.
  @param[in]      Buffer        A pointer to the buffer to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetExPtrS (
  IN CONST GUID     *Guid,
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN       VOID     *Buffer
  )
{
  RETURN_STATUS Status;

  Status = InsertPcdEx (Guid, TokenNumber, PcdTypePtr, Buffer, *SizeOfBuffer);
  return Status;
}

/**
  This function provides a means by which to set a value for a given PCD token.

  Sets the boolean value for the token specified by TokenNumber
  to the value specified by Value.

  If Guid is NULL, then ASSERT().

  @param[in] Guid           The pointer to a 128-bit unique value that
                            designates which namespace to set a value from.
  @param[in] TokenNumber    The PCD token number to set a current value for.
  @param[in] Value          The boolean value to set.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPcdSetExBoolS (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  )
{
  RETURN_STATUS Status;

  Status = InsertPcdEx (Guid, TokenNumber, PcdTypeBool, &Value, sizeof(BOOLEAN));
  return Status;
}

/**
  Set up a notification function that is called when a specified token is set.

  When the token specified by TokenNumber and Guid is set,
  then notification function specified by NotificationFunction is called.
  If Guid is NULL, then the default token space is used.

  If NotificationFunction is NULL, then ASSERT().

  @param[in]  Guid      The pointer to a 128-bit unique value that designates which
                        namespace to set a value from.  If NULL, then the default
                        token space is used.
  @param[in]  TokenNumber   The PCD token number to monitor.
  @param[in]  NotificationFunction  The function to call when the token
                                    specified by Guid and TokenNumber is set.

**/
VOID
EFIAPI
LibPcdCallbackOnSet (
  IN CONST GUID               *Guid,       OPTIONAL
  IN UINTN                    TokenNumber,
  IN PCD_CALLBACK             NotificationFunction
  )
{
  ASSERT (FALSE);
}



/**
  Disable a notification function that was established with LibPcdCallbackonSet().

  Disable a notification function that was previously established with LibPcdCallbackOnSet().

  If NotificationFunction is NULL, then ASSERT().
  If LibPcdCallbackOnSet() was not previously called with Guid, TokenNumber,
  and NotificationFunction, then ASSERT().

  @param[in]  Guid          Specify the GUID token space.
  @param[in]  TokenNumber   Specify the token number.
  @param[in]  NotificationFunction The callback function to be unregistered.

**/
VOID
EFIAPI
LibPcdCancelCallback (
  IN CONST GUID               *Guid,       OPTIONAL
  IN UINTN                    TokenNumber,
  IN PCD_CALLBACK             NotificationFunction
  )
{
  ASSERT (FALSE);
}



/**
  Retrieves the next token in a token space.

  Retrieves the next PCD token number from the token space specified by Guid.
  If Guid is NULL, then the default token space is used.  If TokenNumber is 0,
  then the first token number is returned.  Otherwise, the token number that
  follows TokenNumber in the token space is returned.  If TokenNumber is the last
  token number in the token space, then 0 is returned.

  If TokenNumber is not 0 and is not in the token space specified by Guid, then ASSERT().

  @param[in]  Guid        The pointer to a 128-bit unique value that designates which namespace
                          to set a value from.  If NULL, then the default token space is used.
  @param[in]  TokenNumber The previous PCD token number.  If 0, then retrieves the first PCD
                          token number.

  @return The next valid token number.

**/
UINTN
EFIAPI
LibPcdGetNextToken (
  IN CONST GUID               *Guid,       OPTIONAL
  IN UINTN                    TokenNumber
  )
{
  ASSERT (FALSE);

  return 0;
}



/**
  Used to retrieve the list of available PCD token space GUIDs.

  Returns the PCD token space GUID that follows TokenSpaceGuid in the list of token spaces
  in the platform.
  If TokenSpaceGuid is NULL, then a pointer to the first PCD token spaces returned.
  If TokenSpaceGuid is the last PCD token space GUID in the list, then NULL is returned.

  @param  TokenSpaceGuid  The pointer to a PCD token space GUID.

  @return The next valid token namespace.

**/
GUID *
EFIAPI
LibPcdGetNextTokenSpace (
  IN CONST GUID  *TokenSpaceGuid
  )
{
  ASSERT (FALSE);

  return NULL;
}


/**
  Sets a value of a patchable PCD entry that is type pointer.

  Sets the PCD entry specified by PatchVariable to the value specified by Buffer
  and SizeOfBuffer.  Buffer is returned.  If SizeOfBuffer is greater than
  MaximumDatumSize, then set SizeOfBuffer to MaximumDatumSize and return
  NULL to indicate that the set operation was not actually performed.
  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to
  MaximumDatumSize and NULL must be returned.

  If PatchVariable is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[out] PatchVariable     A pointer to the global variable in a module that is
                                the target of the set operation.
  @param[in] MaximumDatumSize   The maximum size allowed for the PCD entry specified by PatchVariable.
  @param[in, out] SizeOfBuffer  A pointer to the size, in bytes, of Buffer.
  @param[in] Buffer             A pointer to the buffer to used to set the target variable.

  @return Return the pointer to the buffer that was set.

**/
VOID *
EFIAPI
LibPatchPcdSetPtr (
  OUT       VOID        *PatchVariable,
  IN        UINTN       MaximumDatumSize,
  IN OUT    UINTN       *SizeOfBuffer,
  IN CONST  VOID        *Buffer
  )
{
  ASSERT (PatchVariable != NULL);
  ASSERT (SizeOfBuffer  != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  if ((*SizeOfBuffer > MaximumDatumSize) ||
      (*SizeOfBuffer == MAX_ADDRESS)) {
    *SizeOfBuffer = MaximumDatumSize;
    return NULL;
  }

  CopyMem (PatchVariable, Buffer, *SizeOfBuffer);

  return (VOID *) Buffer;
}

/**
  Sets a value of a patchable PCD entry that is type pointer.

  Sets the PCD entry specified by PatchVariable to the value specified
  by Buffer and SizeOfBuffer. If SizeOfBuffer is greater than MaximumDatumSize,
  then set SizeOfBuffer to MaximumDatumSize and return RETURN_INVALID_PARAMETER
  to indicate that the set operation was not actually performed.
  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to
  MaximumDatumSize and RETURN_INVALID_PARAMETER must be returned.

  If PatchVariable is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[out] PatchVariable     A pointer to the global variable in a module that is
                                the target of the set operation.
  @param[in] MaximumDatumSize   The maximum size allowed for the PCD entry specified by PatchVariable.
  @param[in, out] SizeOfBuffer  A pointer to the size, in bytes, of Buffer.
  @param[in] Buffer             A pointer to the buffer to used to set the target variable.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPatchPcdSetPtrS (
  OUT      VOID     *PatchVariable,
  IN       UINTN    MaximumDatumSize,
  IN OUT   UINTN    *SizeOfBuffer,
  IN CONST VOID     *Buffer
  )
{
  ASSERT (PatchVariable != NULL);
  ASSERT (SizeOfBuffer  != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  if ((*SizeOfBuffer > MaximumDatumSize) ||
      (*SizeOfBuffer == MAX_ADDRESS)) {
    *SizeOfBuffer = MaximumDatumSize;
    return RETURN_INVALID_PARAMETER;
  }

  CopyMem (PatchVariable, Buffer, *SizeOfBuffer);

  return RETURN_SUCCESS;
}

/**
  Sets a value and size of a patchable PCD entry that is type pointer.

  Sets the PCD entry specified by PatchVariable to the value specified by Buffer
  and SizeOfBuffer.  Buffer is returned.  If SizeOfBuffer is greater than
  MaximumDatumSize, then set SizeOfBuffer to MaximumDatumSize and return
  NULL to indicate that the set operation was not actually performed.
  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to
  MaximumDatumSize and NULL must be returned.

  If PatchVariable is NULL, then ASSERT().
  If SizeOfPatchVariable is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[out] PatchVariable     A pointer to the global variable in a module that is
                                the target of the set operation.
  @param[out] SizeOfPatchVariable A pointer to the size, in bytes, of PatchVariable.
  @param[in] MaximumDatumSize   The maximum size allowed for the PCD entry specified by PatchVariable.
  @param[in, out] SizeOfBuffer  A pointer to the size, in bytes, of Buffer.
  @param[in] Buffer             A pointer to the buffer to used to set the target variable.

  @return Return the pointer to the buffer been set.

**/
VOID *
EFIAPI
LibPatchPcdSetPtrAndSize (
  OUT       VOID        *PatchVariable,
  OUT       UINTN       *SizeOfPatchVariable,
  IN        UINTN       MaximumDatumSize,
  IN OUT    UINTN       *SizeOfBuffer,
  IN CONST  VOID        *Buffer
  )
{
  ASSERT (PatchVariable != NULL);
  ASSERT (SizeOfPatchVariable != NULL);
  ASSERT (SizeOfBuffer  != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  if ((*SizeOfBuffer > MaximumDatumSize) ||
      (*SizeOfBuffer == MAX_ADDRESS)) {
    *SizeOfBuffer = MaximumDatumSize;
    return NULL;
  }

  CopyMem (PatchVariable, Buffer, *SizeOfBuffer);
  *SizeOfPatchVariable = *SizeOfBuffer;

  return (VOID *) Buffer;
}

/**
  Sets a value and size of a patchable PCD entry that is type pointer.

  Sets the PCD entry specified by PatchVariable to the value specified
  by Buffer and SizeOfBuffer. If SizeOfBuffer is greater than MaximumDatumSize,
  then set SizeOfBuffer to MaximumDatumSize and return RETURN_INVALID_PARAMETER
  to indicate that the set operation was not actually performed.
  If SizeOfBuffer is set to MAX_ADDRESS, then SizeOfBuffer must be set to
  MaximumDatumSize and RETURN_INVALID_PARAMETER must be returned.

  If PatchVariable is NULL, then ASSERT().
  If SizeOfPatchVariable is NULL, then ASSERT().
  If SizeOfBuffer is NULL, then ASSERT().
  If SizeOfBuffer > 0 and Buffer is NULL, then ASSERT().

  @param[out] PatchVariable     A pointer to the global variable in a module that is
                                the target of the set operation.
  @param[out] SizeOfPatchVariable A pointer to the size, in bytes, of PatchVariable.
  @param[in] MaximumDatumSize   The maximum size allowed for the PCD entry specified by PatchVariable.
  @param[in, out] SizeOfBuffer  A pointer to the size, in bytes, of Buffer.
  @param[in] Buffer             A pointer to the buffer to used to set the target variable.

  @return The status of the set operation.

**/
RETURN_STATUS
EFIAPI
LibPatchPcdSetPtrAndSizeS (
  OUT      VOID     *PatchVariable,
  OUT      UINTN    *SizeOfPatchVariable,
  IN       UINTN    MaximumDatumSize,
  IN OUT   UINTN    *SizeOfBuffer,
  IN CONST VOID     *Buffer
  )
{
  ASSERT (PatchVariable != NULL);
  ASSERT (SizeOfPatchVariable != NULL);
  ASSERT (SizeOfBuffer  != NULL);

  if (*SizeOfBuffer > 0) {
    ASSERT (Buffer != NULL);
  }

  if ((*SizeOfBuffer > MaximumDatumSize) ||
      (*SizeOfBuffer == MAX_ADDRESS)) {
    *SizeOfBuffer = MaximumDatumSize;
    return RETURN_INVALID_PARAMETER;
  }

  CopyMem (PatchVariable, Buffer, *SizeOfBuffer);
  *SizeOfPatchVariable = *SizeOfBuffer;

  return RETURN_SUCCESS;
}

/**
  Retrieve additional information associated with a PCD token.

  This includes information such as the type of value the TokenNumber is associated with as well as possible
  human readable name that is associated with the token.

  If TokenNumber is not in the default token space specified, then ASSERT().

  @param[in]    TokenNumber The PCD token number.
  @param[out]   PcdInfo     The returned information associated with the requested TokenNumber.
                            The caller is responsible for freeing the buffer that is allocated by callee for PcdInfo->PcdName.
**/
VOID
EFIAPI
LibPcdGetInfo (
  IN        UINTN           TokenNumber,
  OUT       PCD_INFO        *PcdInfo
  )
{
  ASSERT (FALSE);
}

/**
  Retrieve additional information associated with a PCD token.

  This includes information such as the type of value the TokenNumber is associated with as well as possible
  human readable name that is associated with the token.

  If TokenNumber is not in the token space specified by Guid, then ASSERT().

  @param[in]    Guid        The 128-bit unique value that designates the namespace from which to extract the value.
  @param[in]    TokenNumber The PCD token number.
  @param[out]   PcdInfo     The returned information associated with the requested TokenNumber.
                            The caller is responsible for freeing the buffer that is allocated by callee for PcdInfo->PcdName.
**/
VOID
EFIAPI
LibPcdGetInfoEx (
  IN CONST  GUID            *Guid,
  IN        UINTN           TokenNumber,
  OUT       PCD_INFO        *PcdInfo
  )
{
  ASSERT (FALSE);
}

/**
  Retrieve the currently set SKU Id.

  @return   The currently set SKU Id. If the platform has not set at a SKU Id, then the
            default SKU Id value of 0 is returned. If the platform has set a SKU Id, then the currently set SKU
            Id is returned.
**/
UINTN
EFIAPI
LibPcdGetSku (
  VOID
  )
{
  ASSERT (FALSE);

  return 0;
}

