/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/InstrumentHookLib.h>

VOID
EFIAPI
SetReturnValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   RetVal
  )
{
  ExitContext.Ia32->Eax = RetVal;
}

VOID
EFIAPI
SetReturnValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINT64                  RetVal
  )
{
  ExitContext.Ia32->Eax = (UINT32)RetVal;
  ExitContext.Ia32->Edx = (UINT32)RShiftU64(RetVal, 32);
}

UINTN
EFIAPI
GetReturnValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext
  )
{
  return ExitContext.Ia32->Eax;
}

UINT64
EFIAPI
GetReturnValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext
  )
{
  return (UINT64)ExitContext.Ia32->Eax + RShiftU64(ExitContext.Ia32->Edx, 32);
}

UINTN
EFIAPI
GetParameterValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index
  )
{
  UINTN   *StackPointer;

  ASSERT(EntryContext.Ia32->Esp != 0);
  StackPointer = (UINTN *)(UINTN)(EntryContext.Ia32->Esp);
  return *(StackPointer + Index);
}

UINT64
EFIAPI
GetParameterValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index
  )
{
  UINTN   *StackPointer;

  ASSERT(EntryContext.Ia32->Esp != 0);
  StackPointer = (UINTN *)(UINTN)(EntryContext.Ia32->Esp);
  return *(UINT64 *)(StackPointer + Index);
}

UINTN
EFIAPI
GetReturnParameterValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   Index
  )
{
  UINTN   *StackPointer;

  ASSERT(ExitContext.Ia32->Esp != 0);
  StackPointer = (UINTN *)(UINTN)(ExitContext.Ia32->Esp);
  return *(StackPointer + Index);
}

UINT64
EFIAPI
GetReturnParameterValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   Index
  )
{
  UINTN   *StackPointer;

  ASSERT(ExitContext.Ia32->Esp != 0);
  StackPointer = (UINTN *)(UINTN)(ExitContext.Ia32->Esp);
  return *(UINT64 *)(StackPointer + Index);
}

VOID
EFIAPI
SetParameterValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index,
  IN UINTN                   Data
  )
{
  UINTN   *StackPointer;

  ASSERT(EntryContext.Ia32->Esp != 0);
  StackPointer = (UINTN *)(UINTN)(EntryContext.Ia32->Esp);
  *(StackPointer + Index) = Data;
}

VOID
EFIAPI
SetParameterValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index,
  IN UINT64                  Data
  )
{
  UINTN   *StackPointer;

  ASSERT(EntryContext.Ia32->Esp != 0);
  StackPointer = (UINTN *)(UINTN)(EntryContext.Ia32->Esp);
  *(UINT64 *)(StackPointer + Index) = Data;
}

VOID
EFIAPI
SetSkipReturnValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   RetVal
  )
{
  EntryContext.Ia32->Eax = RetVal;
}

VOID
EFIAPI
SetSkipReturnValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINT64                  RetVal
  )
{
  EntryContext.Ia32->Eax = (UINT32)RetVal;
  EntryContext.Ia32->Edx = (UINT32)RShiftU64(RetVal, 32);
}
