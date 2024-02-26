/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/InstrumentHookLib.h>

/*
  GCC calling convention: rdi/rsi/rdx/rcx/r8/r9
*/

VOID
EFIAPI
SetReturnValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   RetVal
  )
{
  ExitContext.X64->Rbx = RetVal;
}

VOID
EFIAPI
SetReturnValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINT64                  RetVal
  )
{
  SetReturnValue (ExitContext, RetVal);
}

UINTN
EFIAPI
GetReturnValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext
  )
{
  return ExitContext.X64->Rbx;
}

UINT64
EFIAPI
GetReturnValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext
  )
{
  return GetReturnValue (ExitContext);
}

UINTN
EFIAPI
GetParameterValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index
  )
{
  UINTN   *StackPointer;

  ASSERT(EntryContext.X64->Rsp != 0);
  if (Index == 1) {
    return EntryContext.X64->Rcx;
  } else if (Index == 2) {
    return EntryContext.X64->Rdx;
  } else if (Index == 3) {
    return EntryContext.X64->R8;
  } else if (Index == 4) {
    return EntryContext.X64->R9;
  } else {
    StackPointer = (UINTN *)(UINTN)(EntryContext.X64->Rsp);
    return *(StackPointer + Index);
  }
}

UINT64
EFIAPI
GetParameterValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index
  )
{
  return GetParameterValue (EntryContext, Index);
}

UINTN
EFIAPI
GetReturnParameterValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   Index
  )
{
  UINTN   *StackPointer;

  ASSERT(ExitContext.X64->Rsp != 0);

  StackPointer = (UINTN *)(UINTN)(ExitContext.X64->Rsp);
  return *(StackPointer + Index);
}

UINT64
EFIAPI
GetReturnParameterValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   Index
  )
{
  return GetReturnParameterValue (ExitContext, Index);
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

  ASSERT(EntryContext.X64->Rsp != 0);
  if (Index == 1) {
    EntryContext.X64->Rcx = Data;
  } else if (Index == 2) {
    EntryContext.X64->Rdx = Data;
  } else if (Index == 3) {
    EntryContext.X64->R8 = Data;
  } else if (Index == 4) {
    EntryContext.X64->R9 = Data;
  }
  if (EntryContext.X64->Rsp != (UINTN)-1) {
    // Update both register and stack.
    StackPointer = (UINTN *)(UINTN)(EntryContext.X64->Rsp);
    *(StackPointer + Index) = Data;
  }
}

VOID
EFIAPI
SetParameterValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index,
  IN UINT64                  Data
  )
{
  SetParameterValue (EntryContext, Index, Data);
}

VOID
EFIAPI
SetSkipReturnValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   RetVal
  )
{
  // unsupported
}

VOID
EFIAPI
SetSkipReturnValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINT64                  RetVal
  )
{
  // unsupported
}
