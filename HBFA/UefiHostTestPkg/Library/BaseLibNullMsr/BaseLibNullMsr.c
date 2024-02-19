/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

UINT64
EFIAPI
AsmReadMsr64 (
  IN UINT32  Index
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT32
EFIAPI
AsmReadMsr32 (
  IN      UINT32                    Index
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT64
EFIAPI
AsmWriteMsr64 (
  IN      UINT32                    Index,
  IN      UINT64                    Value
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT32
EFIAPI
AsmWriteMsr32 (
  IN      UINT32                    Index,
  IN      UINT32                    Value
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT64
EFIAPI
AsmMsrOr64 (
  IN      UINT32                    Index,
  IN      UINT64                    OrData
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT64
EFIAPI
AsmMsrAnd64 (
  IN      UINT32                    Index,
  IN      UINT64                    AndData
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT64
EFIAPI
AsmMsrAndThenOr64 (
  IN      UINT32                    Index,
  IN      UINT64                    AndData,
  IN      UINT64                    OrData
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT64
EFIAPI
AsmMsrBitFieldWrite64 (
  IN      UINT32                    Index,
  IN      UINTN                     StartBit,
  IN      UINTN                     EndBit,
  IN      UINT64                    Value
  )
{
  ASSERT (FALSE);
  return 0;
}