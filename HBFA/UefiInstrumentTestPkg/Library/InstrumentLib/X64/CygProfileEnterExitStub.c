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
  EntryContext.X64->Rsp = EntryContext.X64->Rbp;
  EntryContext.X64->Rsp += 8; // the caller to the caller to __cyg_profile_func_enter

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
  ExitContext.X64->Rsp = ExitContext.X64->Rbp;
  ExitContext.X64->Rsp += 8; // the caller to the caller to __cyg_profile_func_enter

  FunctionExit (ExitContext, FunctionAddress, CallerAddress);
}
