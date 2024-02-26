/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/InstrumentHookLib.h>

UINTN
EFIAPI
CygProfileEnter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   CallerAddress,
  IN UINTN                   FunctionAddress
  )
{
  EntryContext.Ia32->Esp = EntryContext.Ia32->Ebp;
  EntryContext.Ia32->Esp += 4; // the caller to the caller to __cyg_profile_func_enter

  return FunctionEnter (EntryContext, FunctionAddress, CallerAddress);
}

VOID
EFIAPI
CygProfileExit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   CallerAddress,
  IN UINTN                   FunctionAddress
  )
{
  ExitContext.Ia32->Esp = ExitContext.Ia32->Ebp;
  ExitContext.Ia32->Esp += 4; // the caller to the caller to __cyg_profile_func_enter

  FunctionExit (ExitContext, FunctionAddress, CallerAddress);
}
