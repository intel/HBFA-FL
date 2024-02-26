/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/InstrumentHookLib.h>

GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR9Template[]  = {0x4C, 0x89, 0x4C, 0x24, 0x20};       // mov         qword ptr [rsp+20h],r9
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR9dTemplate[] = {0x44, 0x89, 0x4C, 0x24, 0x20};       // mov         dword ptr [rsp+20h],r9d
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR9wTemplate[] = {0x66, 0x44, 0x89, 0x4C, 0x24, 0x20}; // mov         word ptr [rsp+20h],r9w
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR9bTemplate[] = {0x44, 0x88, 0x4C, 0x24, 0x20};       // mov         byte ptr [rsp+20h],r9b

GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR8Template[]  = {0x4C, 0x89, 0x44, 0x24, 0x18};       // mov         qword ptr [rsp+18h],r8
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR8dTemplate[] = {0x44, 0x89, 0x44, 0x24, 0x18};       // mov         dword ptr [rsp+18h],r8d
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR8wTemplate[] = {0x66, 0x44, 0x89, 0x44, 0x24, 0x18}; // mov         word ptr [rsp+18h],r8w
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovR8bTemplate[] = {0x44, 0x88, 0x44, 0x24, 0x18};       // mov         byte ptr [rsp+18h],r8b

GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovRdxTemplate[] = {0x48, 0x89, 0x54, 0x24, 0x10}; // mov         qword ptr [rsp+10h],rdx
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovEdxTemplate[] = {0x89, 0x54, 0x24, 0x10};       // mov         dword ptr [rsp+10h],edx
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovDxTemplate[]  = {0x66, 0x89, 0x54, 0x24, 0x10}; // mov         word ptr [rsp+10h],dx
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovDlTemplate[]  = {0x88, 0x54, 0x24, 0x10};       // mov         byte ptr [rsp+10h],dl

GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovRcxTemplate[] = {0x48, 0x89, 0x4C, 0x24, 0x08}; // mov         qword ptr [rsp+8],rcx
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovEcxTemplate[] = {0x89, 0x4C, 0x24, 0x08};       // mov         dword ptr [rsp+8],ecx
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovCxTemplate[]  = {0x66, 0x89, 0x4C, 0x24, 0x08}; // mov         word ptr [rsp+8],cx
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mMovClTemplate[]  = {0x88, 0x4C, 0x24, 0x08};       // mov         byte ptr [rsp+8],cl

GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mSubRspTemplate[] = {0x48, 0x83, 0xEC, 0x00};       // sub         rsp,28h
GLOBAL_REMOVE_IF_UNREFERENCED UINT8  mAddRspTemplate[] = {0x48, 0x83, 0xC4, 0x00};       // add         rsp,28h

typedef struct {
  UINT8    *Instruction;
  UINTN    InstructionLength;
} INSTRUCTION_TEMPLATE;

GLOBAL_REMOVE_IF_UNREFERENCED INSTRUCTION_TEMPLATE mInstructionTemplate[] = {
  // 1st group
  {mMovRcxTemplate, sizeof(mMovRcxTemplate)},
  {mMovEcxTemplate, sizeof(mMovEcxTemplate)},
  {mMovCxTemplate,  sizeof(mMovCxTemplate)},
  {mMovClTemplate,  sizeof(mMovClTemplate)},

  // 2nd group
  {mMovRdxTemplate, sizeof(mMovRdxTemplate)},
  {mMovEdxTemplate, sizeof(mMovEdxTemplate)},
  {mMovDxTemplate,  sizeof(mMovDxTemplate)},
  {mMovDlTemplate,  sizeof(mMovDlTemplate)},

  // 3rd group
  {mMovR8Template,  sizeof(mMovR8Template)},
  {mMovR8dTemplate, sizeof(mMovR8dTemplate)},
  {mMovR8wTemplate, sizeof(mMovR8wTemplate)},
  {mMovR8bTemplate, sizeof(mMovR8bTemplate)},

  // 4th group
  {mMovR9Template,  sizeof(mMovR9Template)},
  {mMovR9dTemplate, sizeof(mMovR9dTemplate)},
  {mMovR9wTemplate, sizeof(mMovR9wTemplate)},
  {mMovR9bTemplate, sizeof(mMovR9bTemplate)},
};

UINTN
GetFunctionAddress (
  IN OUT UINTN               *Rsp,
  IN UINTN                   ReturnAddress
  )
{
  UINTN  FunctionAddress;
  UINTN  Index;

  FunctionAddress = (ReturnAddress - 5);
  if (CompareMem (
        (UINT8 *)FunctionAddress - sizeof(mSubRspTemplate),
        mSubRspTemplate,
        sizeof(mSubRspTemplate) - sizeof(UINT8)) == 0) {
    *Rsp += *(UINT8 *)(FunctionAddress - sizeof(UINT8));
    FunctionAddress -= sizeof(mSubRspTemplate);
  }

  for (Index = 0; Index < ARRAY_SIZE(mInstructionTemplate); Index++) {
    if (CompareMem (
          (UINT8 *)FunctionAddress - mInstructionTemplate[Index].InstructionLength,
          mInstructionTemplate[Index].Instruction,
          mInstructionTemplate[Index].InstructionLength) == 0) {
      FunctionAddress -= mInstructionTemplate[Index].InstructionLength;
    }
  }

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

  FunctionAddress = GetFunctionAddress (&EntryContext.X64->Rsp, ReturnAddress);

  EntryContext.X64->Rsp += 8; // the caller to the caller to _penter

  CallerAddress = *(UINTN *)(UINTN)(EntryContext.X64->Rsp);

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

  FunctionAddress = GetFunctionAddress (&ExitContext.X64->Rsp, ReturnAddress);

  ExitContext.X64->Rsp += 8; // the caller to the caller to _penter

  CallerAddress = *(UINTN *)(UINTN)(ExitContext.X64->Rsp);

  FunctionExit (ExitContext, FunctionAddress, CallerAddress);
}
