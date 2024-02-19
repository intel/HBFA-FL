/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseCryptLib.h>

VOID *
EFIAPI
RsaNew (
  VOID
  )
{
  return NULL;
}

VOID
EFIAPI
RsaFree (
  IN  VOID  *RsaContext
  )
{
}

BOOLEAN
EFIAPI
RsaSetKey (
  IN OUT  VOID         *RsaContext,
  IN      RSA_KEY_TAG  KeyTag,
  IN      CONST UINT8  *BigNumber,
  IN      UINTN        BnSize
  )
{
  return FALSE;
}

BOOLEAN
EFIAPI
RsaPkcs1Verify (
  IN  VOID         *RsaContext,
  IN  CONST UINT8  *MessageHash,
  IN  UINTN        HashSize,
  IN  CONST UINT8  *Signature,
  IN  UINTN        SigSize
  )
{
  return FALSE;
}

UINTN
EFIAPI
Sha256GetContextSize (
  VOID
  )
{
  return 0;
}

BOOLEAN
EFIAPI
Sha256Init (
  OUT  VOID  *Sha256Context
  )
{
  return FALSE;
}

BOOLEAN
EFIAPI
Sha256Update (
  IN OUT  VOID        *Sha256Context,
  IN      CONST VOID  *Data,
  IN      UINTN       DataSize
  )
{
  return FALSE;
}

BOOLEAN
EFIAPI
Sha256Final (
  IN OUT  VOID   *Sha256Context,
  OUT     UINT8  *HashValue
  )
{
  return FALSE;
}
