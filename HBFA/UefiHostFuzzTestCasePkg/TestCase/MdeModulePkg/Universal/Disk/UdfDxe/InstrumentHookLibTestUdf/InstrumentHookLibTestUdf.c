/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/InstrumentHookLib.h>
#include <Library/IniParsingLib.h>
#include "../Udf.h"

typedef struct _FUNC_HOOK FUNC_HOOK;

typedef
UINTN
(EFIAPI *HOOK_FUNC_ENTER) (
  IN FUNC_HOOK               *FuncHook,
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  );

typedef
VOID
(EFIAPI *HOOK_FUNC_EXIT) (
  IN FUNC_HOOK               *FuncHook,
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  );

struct _FUNC_HOOK {
  CHAR8  *Name;
  UINTN  Func;
  UINTN  HookFuncEnter;
  UINTN  HookFuncExit;
  UINTN  CallErrorCount;
  UINTN  ReturnValue;
  UINTN  CurrentCallCount;
} ;

BOOLEAN mInitDone;

UINTN
EFIAPI
CommonEnter (
  IN FUNC_HOOK               *FuncHook,
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  FuncHook->CurrentCallCount++;
  if (FuncHook->CurrentCallCount == FuncHook->CallErrorCount) {
    //SetSkipReturnValue (EntryContext, FuncHook->ReturnValue);
    if (FuncHook->Func == (UINTN)AllocateZeroPool) {
      SetParameterValue (EntryContext, 1, (UINTN)-1);
    } else if (FuncHook->Func == (UINTN)AllocatePool) {
      SetParameterValue (EntryContext, 1, (UINTN)-1);
    } else if (FuncHook->Func == (UINTN)ReallocatePool) {
      SetParameterValue (EntryContext, 1, (UINTN)-1);
    }
    return 1;
  }
  return 0;
}

VOID
EFIAPI
CommonExit (
  IN FUNC_HOOK               *FuncHook,
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  if (FuncHook->CurrentCallCount == FuncHook->CallErrorCount) {
    SetReturnValue (ExitContext, FuncHook->ReturnValue);
  }
  return ;
}

GLOBAL_REMOVE_IF_UNREFERENCED FUNC_HOOK mFuncHook[] = {
  {"AllocateZeroPool",   (UINTN)AllocateZeroPool,   (UINTN)CommonEnter, (UINTN)CommonExit},
  {"AllocatePool",       (UINTN)AllocatePool,       (UINTN)CommonEnter, (UINTN)CommonExit},
  {"ReallocatePool",     (UINTN)ReallocatePool,     (UINTN)CommonEnter, (UINTN)CommonExit},
};

FUNC_HOOK *
GetFuncHook (
  IN UINTN  FuncAddr
  )
{
  UINTN  Index;
  for (Index = 0; Index < ARRAY_SIZE(mFuncHook); Index++) {
    if (FuncAddr == mFuncHook[Index].Func) {
      return &mFuncHook[Index];
    }
  }
  return NULL;
}

UINTN
EFIAPI
FunctionEnter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  FUNC_HOOK  *FuncHook;
  HOOK_FUNC_ENTER  HookFunc;

  if (!mInitDone) {
    return 0;
  }
  
  FuncHook = GetFuncHook (FunctionAddress);
  if (FuncHook == NULL) {
    return 0;
  }
  if (FuncHook->HookFuncEnter == 0) {
    return 0;
  }
  HookFunc = (HOOK_FUNC_ENTER)(FuncHook->HookFuncEnter);
  return HookFunc (FuncHook, EntryContext, FunctionAddress, CallerAddress);
}

VOID
EFIAPI
FunctionExit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  FUNC_HOOK  *FuncHook;
  HOOK_FUNC_EXIT  HookFunc;

  if (!mInitDone) {
    return ;
  }
  
  FuncHook = GetFuncHook (FunctionAddress);
  if (FuncHook == NULL) {
    return ;
  }
  if (FuncHook->HookFuncExit == 0) {
    return ;
  }
  HookFunc = (HOOK_FUNC_EXIT)(FuncHook->HookFuncExit);
  HookFunc (FuncHook, ExitContext, FunctionAddress, CallerAddress);
}

VOID
EFIAPI
InstrumentHookLibInit (
  IN  UINT8  *DataBuffer,
  IN  UINTN  BufferSize
  )
{
  VOID        *Context;
  UINTN       Index;
  EFI_STATUS  Status;

  Context = OpenIniFile (DataBuffer, BufferSize);

  for (Index = 0; Index < ARRAY_SIZE(mFuncHook); Index++) {
    Status = GetDecimalUintnFromDataFile (
               Context,
               mFuncHook[Index].Name,
               "CallErrorCount",
               &mFuncHook[Index].CallErrorCount
               );
    if (EFI_ERROR(Status)) {
      continue ;
    }

    Status = GetEfiStatusFromDataFile (
               Context,
               mFuncHook[Index].Name,
               "ReturnValue",
               &mFuncHook[Index].ReturnValue
               );
    if (EFI_ERROR(Status)) {
      Status = GetHexUintnFromDataFile (
                 Context,
                 mFuncHook[Index].Name,
                 "ReturnValue",
                 &mFuncHook[Index].ReturnValue
                 );
    }
  }

  CloseIniFile (Context);

  mInitDone = TRUE;
}