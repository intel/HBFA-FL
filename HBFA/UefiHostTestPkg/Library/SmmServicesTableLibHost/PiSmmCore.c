/** @file
  SMM Core Main Entry Point

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PiSmmCore.h"

//
// SMM Core global variable for SMM System Table.  Only accessed as a physical structure in SMRAM.
//
EFI_SMM_SYSTEM_TABLE2  gSmmCoreSmst = {
  {
    SMM_SMST_SIGNATURE,
    EFI_SMM_SYSTEM_TABLE2_REVISION,
    sizeof (gSmmCoreSmst.Hdr)
  },
  NULL,                          // SmmFirmwareVendor
  0,                             // SmmFirmwareRevision
  SmmInstallConfigurationTable,
  {
    {
      (EFI_SMM_CPU_IO2) SmmEfiNotAvailableYetArg5,       // SmmMemRead
      (EFI_SMM_CPU_IO2) SmmEfiNotAvailableYetArg5        // SmmMemWrite
    },
    {
      (EFI_SMM_CPU_IO2) SmmEfiNotAvailableYetArg5,       // SmmIoRead
      (EFI_SMM_CPU_IO2) SmmEfiNotAvailableYetArg5        // SmmIoWrite
    }
  },
  SmmAllocatePool,
  SmmFreePool,
  SmmAllocatePages,
  SmmFreePages,
  NULL,                          // SmmStartupThisAp
  0,                             // CurrentlyExecutingCpu
  0,                             // NumberOfCpus
  NULL,                          // CpuSaveStateSize
  NULL,                          // CpuSaveState
  0,                             // NumberOfTableEntries
  NULL,                          // SmmConfigurationTable
  SmmInstallProtocolInterface,
  SmmUninstallProtocolInterface,
  SmmHandleProtocol,
  SmmRegisterProtocolNotify,
  SmmLocateHandle,
  SmmLocateProtocol,
  SmiManage,
  SmiHandlerRegister,
  SmiHandlerUnRegister
};

/**
  Place holder function until all the SMM System Table Service are available.

  Note: This function is only used by SMRAM invocation.  It is never used by DXE invocation.

  @param  Arg1                   Undefined
  @param  Arg2                   Undefined
  @param  Arg3                   Undefined
  @param  Arg4                   Undefined
  @param  Arg5                   Undefined

  @return EFI_NOT_AVAILABLE_YET

**/
EFI_STATUS
EFIAPI
SmmEfiNotAvailableYetArg5 (
  UINTN Arg1,
  UINTN Arg2,
  UINTN Arg3,
  UINTN Arg4,
  UINTN Arg5
  )
{
  //
  // This function should never be executed.  If it does, then the architectural protocols
  // have not been designed correctly.
  //
  return EFI_NOT_AVAILABLE_YET;
}

