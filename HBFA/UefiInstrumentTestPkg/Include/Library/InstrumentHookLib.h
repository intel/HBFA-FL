/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _INSTRUMENT_HOOK_LIB_
#define _INSTRUMENT_HOOK_LIB_

typedef struct {
  UINT32   Edi;
  UINT32   Esi;
  UINT32   Ebp;
  UINT32   Esp; // this is ESP after call instruction
  UINT32   Ebx;
  UINT32   Edx;
  UINT32   Ecx;
  UINT32   Eax;
} FUNC_CONTEXT_IA32;

typedef struct {
  UINT64   R15;
  UINT64   R14;
  UINT64   R13;
  UINT64   R12;
  UINT64   R11;
  UINT64   R10;
  UINT64   R9;
  UINT64   R8;
  UINT64   Rdi;
  UINT64   Rsi;
  UINT64   Rbp;
  UINT64   Rsp; // this is RSP after call instruction
  UINT64   Rbx;
  UINT64   Rdx;
  UINT64   Rcx;
  UINT64   Rax;
} FUNC_CONTEXT_X64;

typedef union {
  FUNC_CONTEXT_IA32 *Ia32;
  FUNC_CONTEXT_X64  *X64;
} FUNC_ENTER_CONTEXT;

typedef union {
  FUNC_CONTEXT_IA32 *Ia32;
  FUNC_CONTEXT_X64  *X64;
} FUNC_EXIT_CONTEXT;

/*
  0 - continue
  1 - skip this function
*/
UINTN
EFIAPI
FunctionEnter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  );

VOID
EFIAPI
FunctionExit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  );

VOID
EFIAPI
InstrumentHookLibInit (
  IN  UINT8  *DataBuffer,
  IN  UINTN  BufferSize
  );

//
// external func
//
VOID
EFIAPI
SetReturnValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   RetVal
  );

VOID
EFIAPI
SetReturnValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINT64                  RetVal
  );

UINTN
EFIAPI
GetReturnValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext
  );

UINT64
EFIAPI
GetReturnValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext
  );

/*
  Index is 1 based.
*/
UINTN
EFIAPI
GetParameterValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index
  );

/*
  Index is 1 based.
*/
UINT64
EFIAPI
GetParameterValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index
  );

/*
  Index is 1 based.
*/
UINTN
EFIAPI
GetReturnParameterValue (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   Index
  );

/*
  Index is 1 based.
*/
UINT64
EFIAPI
GetReturnParameterValue64 (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   Index
  );

/*
  Index is 1 based.
*/
VOID
EFIAPI
SetParameterValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index,
  IN UINTN                   Data
  );

/*
  Index is 1 based.
*/
VOID
EFIAPI
SetParameterValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   Index,
  IN UINT64                  Data
  );

VOID
EFIAPI
SetSkipReturnValue (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   RetVal
  );

VOID
EFIAPI
SetSkipReturnValue64 (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINT64                  RetVal
  );

#endif