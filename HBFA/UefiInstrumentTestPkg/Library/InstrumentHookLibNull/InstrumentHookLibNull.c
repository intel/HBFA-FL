/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/InstrumentHookLib.h>

UINTN
EFIAPI
FunctionEnter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  return 0;
}

VOID
EFIAPI
FunctionExit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  return ;
}
