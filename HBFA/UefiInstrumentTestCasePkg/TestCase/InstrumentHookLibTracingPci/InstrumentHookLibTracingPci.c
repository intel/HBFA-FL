/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/InstrumentHookLib.h>
#include <Library/IoLib.h>
#include <IndustryStandard/Pci.h>

#define PcdPciExpressRegionLength  SIZE_1MB
#define PcdPciReservedIobase       0x2000

typedef
UINTN
(EFIAPI *HOOK_FUNC_ENTER) (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  );

typedef
VOID
(EFIAPI *HOOK_FUNC_EXIT) (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  );

typedef struct {
  CHAR16 *Name;
  UINTN  Func;
  UINTN  HookFuncEnter;
  UINTN  HookFuncExit;
} FUNC_HOOK;

BOOLEAN
ProcessMmioAddress (
  IN UINTN  Address
  )
{
  // Skip below:
  //   IoApic    (FEC00000)
  //   HPET      (FED00000)
  //   TPM       (FED40000)
  //   LocalApic (FEE00000)
  if (Address >= 0xFEC00000) {
    return FALSE;
  }
  // Skip normal MMIO
  if (Address <= PcdGet64(PcdPciExpressBaseAddress)) {
    return FALSE;
  }
  return TRUE;
}

BOOLEAN
ProcessIoAddress (
  IN UINTN  Address
  )
{
  // Skip the ones in DEBUG path.
  //  Serial port
  if (Address >= 0x3F8 && Address <= 0x3FF) {
    return FALSE;
  }
  //  CMOS-RTC
  if (Address >= 0x70 && Address <= 0x77) {
    return FALSE;
  }
  // Skip normal IO
  if (Address > PcdPciReservedIobase) {
    return FALSE;
  }
  return TRUE;
}

VOID
CheckPcieBme (
  IN UINTN MmioAddress
  )
{
  UINTN  Offset;
  if (MmioAddress >= PcdGet64(PcdPciExpressBaseAddress) + PcdPciExpressRegionLength) {
    return ;
  }
  if (MmioAddress < PcdGet64(PcdPciExpressBaseAddress)) {
    return ;
  }
  // BME is in the first byte of CMD
  Offset = MmioAddress & 0xFFF;
  if (Offset != PCI_COMMAND_OFFSET) {
    return ;
  }
  DEBUG ((DEBUG_INFO, " (*** TOUCH PCIE BME ***)"));
}

VOID
CheckPciBme (
  IN UINTN IoAddress,
  IN UINTN IoValue
  )
{
  UINTN  Offset;
  if (IoAddress != 0xCF8) {
    return ;
  }
  // BME is in the first byte of CMD
  Offset = IoValue & 0xFF;
  if (Offset != PCI_COMMAND_OFFSET) {
    return ;
  }
  DEBUG ((DEBUG_INFO, " (*** TOUCH PCI BME ***)"));
}

UINTN
EFIAPI
MmioRead8Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioRead8(0x%x", Address));
  return 0;
}

VOID
EFIAPI
MmioRead8Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT8  Value;

  Value = (UINT8)GetReturnValue (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%02x) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
MmioRead16Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioRead16(0x%x", Address));
  return 0;
}

VOID
EFIAPI
MmioRead16Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT16 Value;

  Value = (UINT16)GetReturnValue (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%04x) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
MmioRead32Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioRead32(0x%x", Address));
  return 0;
}

VOID
EFIAPI
MmioRead32Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT32 Value;

  Value = (UINT32)GetReturnValue (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%08x) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
MmioRead64Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioRead64(0x%x", Address));
  return 0;
}

VOID
EFIAPI
MmioRead64Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT64 Value;

  Value = GetReturnValue64 (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%016lx) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
MmioWrite8Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT8  Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  Value = (UINT8)GetParameterValue (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioWrite8(0x%x<-0x%02x) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
MmioWrite8Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

UINTN
EFIAPI
MmioWrite16Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT16 Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  Value = (UINT16)GetParameterValue (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioWrite16(0x%x<-0x%04x) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
MmioWrite16Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

UINTN
EFIAPI
MmioWrite32Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT32 Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  Value = (UINT32)GetParameterValue (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioWrite32(0x%x<-0x%08x) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
MmioWrite32Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

UINTN
EFIAPI
MmioWrite64Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT64 Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessMmioAddress (Address)) {
    return 0;
  }
  Value = GetParameterValue64 (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: MmioWrite64(0x%x<-0x%016lx) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPcieBme (Address);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
MmioWrite64Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

UINTN
EFIAPI
IoRead8Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessIoAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: IoRead8(0x%x", Address));
  return 0;
}

VOID
EFIAPI
IoRead8Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT8  Value;

  Value = (UINT8)GetReturnValue (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessIoAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%02x) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPciBme (Address, Value);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
IoRead16Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessIoAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: IoRead16(0x%x", Address));
  return 0;
}

VOID
EFIAPI
IoRead16Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT16 Value;

  Value = (UINT16)GetReturnValue (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessIoAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%04x) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPciBme (Address, Value);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
IoRead32Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessIoAddress (Address)) {
    return 0;
  }
  DEBUG((DEBUG_INFO, "RSC_TRACE: IoRead32(0x%x", Address));
  return 0;
}

VOID
EFIAPI
IoRead32Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT32 Value;

  Value = (UINT32)GetReturnValue (ExitContext);
  Address = GetReturnParameterValue (ExitContext, 1);
  if (!ProcessIoAddress (Address)) {
    return ;
  }
  DEBUG((DEBUG_INFO, "->0x%08x) - %x from %x", Value, FunctionAddress, CallerAddress));
  CheckPciBme (Address, Value);
  DEBUG((DEBUG_INFO, "\n"));
  return ;
}

UINTN
EFIAPI
IoWrite8Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT8  Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessIoAddress (Address)) {
    return 0;
  }
  Value = (UINT8)GetParameterValue (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: IoWrite8(0x%x<-0x%02x) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPciBme (Address, Value);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
IoWrite8Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

UINTN
EFIAPI
IoWrite16Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT16 Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessIoAddress (Address)) {
    return 0;
  }
  Value = (UINT16)GetParameterValue (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: IoWrite16(0x%x<-0x%04x) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPciBme (Address, Value);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
IoWrite16Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

UINTN
EFIAPI
IoWrite32Enter (
  IN OUT FUNC_ENTER_CONTEXT  EntryContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
  UINTN  Address;
  UINT32 Value;

  Address = GetParameterValue (EntryContext, 1);
  if (!ProcessIoAddress (Address)) {
    return 0;
  }
  Value = (UINT32)GetParameterValue (EntryContext, 2);
  DEBUG((DEBUG_INFO, "RSC_TRACE: IoWrite32(0x%x<-0x%08x) - %x from %x", Address, Value, FunctionAddress, CallerAddress));
  CheckPciBme (Address, Value);
  DEBUG((DEBUG_INFO, "\n"));
  return 0;
}

VOID
EFIAPI
IoWrite32Exit (
  IN OUT FUNC_EXIT_CONTEXT   ExitContext,
  IN UINTN                   FunctionAddress,
  IN UINTN                   CallerAddress
  )
{
}

GLOBAL_REMOVE_IF_UNREFERENCED FUNC_HOOK mFuncHook[] = {
  {L"MmioRead8",   (UINTN)MmioRead8,    (UINTN)MmioRead8Enter,   (UINTN)MmioRead8Exit},
  {L"MmioRead16",  (UINTN)MmioRead16,   (UINTN)MmioRead16Enter,  (UINTN)MmioRead16Exit},
  {L"MmioRead32",  (UINTN)MmioRead32,   (UINTN)MmioRead32Enter,  (UINTN)MmioRead32Exit},
  {L"MmioRead64",  (UINTN)MmioRead64,   (UINTN)MmioRead64Enter,  (UINTN)MmioRead64Exit},
  {L"MmioWrite8",  (UINTN)MmioWrite8,   (UINTN)MmioWrite8Enter,  (UINTN)MmioWrite8Exit},
  {L"MmioWrite16", (UINTN)MmioWrite16,  (UINTN)MmioWrite16Enter, (UINTN)MmioWrite16Exit},
  {L"MmioWrite32", (UINTN)MmioWrite32,  (UINTN)MmioWrite32Enter, (UINTN)MmioWrite32Exit},
  {L"MmioWrite64", (UINTN)MmioWrite64,  (UINTN)MmioWrite64Enter, (UINTN)MmioWrite64Exit},
  {L"IoRead8",     (UINTN)IoRead8,      (UINTN)IoRead8Enter,     (UINTN)IoRead8Exit},
  {L"IoRead16",    (UINTN)IoRead16,     (UINTN)IoRead16Enter,    (UINTN)IoRead16Exit},
  {L"IoRead32",    (UINTN)IoRead32,     (UINTN)IoRead32Enter,    (UINTN)IoRead32Exit},
  {L"IoWrite8",    (UINTN)IoWrite8,     (UINTN)IoWrite8Enter,    (UINTN)IoWrite8Exit},
  {L"IoWrite16",   (UINTN)IoWrite16,    (UINTN)IoWrite16Enter,   (UINTN)IoWrite16Exit},
  {L"IoWrite32",   (UINTN)IoWrite32,    (UINTN)IoWrite32Enter,   (UINTN)IoWrite32Exit},
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
  
  FuncHook = GetFuncHook (FunctionAddress);
  if (FuncHook == NULL) {
    return 0;
  }
  if (FuncHook->HookFuncEnter == 0) {
    return 0;
  }
  HookFunc = (HOOK_FUNC_ENTER)(FuncHook->HookFuncEnter);
  return HookFunc (EntryContext, FunctionAddress, CallerAddress);
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
  
  FuncHook = GetFuncHook (FunctionAddress);
  if (FuncHook == NULL) {
    return ;
  }
  if (FuncHook->HookFuncExit == 0) {
    return ;
  }
  HookFunc = (HOOK_FUNC_EXIT)(FuncHook->HookFuncExit);
  HookFunc (ExitContext, FunctionAddress, CallerAddress);
}
