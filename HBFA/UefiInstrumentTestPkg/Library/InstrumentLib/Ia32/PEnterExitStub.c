/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/InstrumentHookLib.h>

UINTN
GetFunctionAddress (
  IN UINTN                   ReturnAddress
  )
{
  UINTN  FunctionAddress;

  FunctionAddress = (ReturnAddress - 5);

  return FunctionAddress;
}

UINTN
EFIAPI
Cpenter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   ReturnAddress
  )
{
  UINTN  FunctionAddress;
  UINTN  CallerAddress;

  FunctionAddress = GetFunctionAddress (ReturnAddress);

  EntryContext.Ia32->Esp += 4; // the caller to the caller to _penter

  CallerAddress = *(UINTN *)(UINTN)(EntryContext.Ia32->Esp);

  return FunctionEnter (EntryContext, FunctionAddress, CallerAddress);
}

VOID
EFIAPI
Cpexit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   ReturnAddress
  )
{
  UINTN  FunctionAddress;
  UINTN  CallerAddress;

  FunctionAddress = GetFunctionAddress (ReturnAddress);

  ExitContext.Ia32->Esp += 4; // the caller to the caller to _penter

  CallerAddress = *(UINTN *)((UINTN)ExitContext.Ia32->Esp);

  FunctionExit (ExitContext, FunctionAddress, CallerAddress);
}
