/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

UINT32
EFIAPI
AsmCpuidEx (
  IN      UINT32                    Index,
  IN      UINT32                    SubIndex,
  OUT     UINT32                    *RegisterEax,  OPTIONAL
  OUT     UINT32                    *RegisterEbx,  OPTIONAL
  OUT     UINT32                    *RegisterEcx,  OPTIONAL
  OUT     UINT32                    *RegisterEdx   OPTIONAL
  )
{
  ASSERT (FALSE);
  return 0;
}

UINT32
EFIAPI
AsmCpuid (
  IN      UINT32                    Index,
  OUT     UINT32                    *RegisterEax,  OPTIONAL
  OUT     UINT32                    *RegisterEbx,  OPTIONAL
  OUT     UINT32                    *RegisterEcx,  OPTIONAL
  OUT     UINT32                    *RegisterEdx   OPTIONAL
  )
{
  ASSERT (FALSE);
  return 0;
}
