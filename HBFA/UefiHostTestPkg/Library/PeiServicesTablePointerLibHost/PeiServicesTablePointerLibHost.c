/** @file
  PEI Services Table Pointer Library.

  This library is used for PEIM which does executed from flash device directly but
  executed in memory.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#include "PeiMain.h"

extern EFI_PEI_CPU_IO_PPI mPeiDefaultCpuIoPpi;
extern EFI_PEI_PCI_CFG2_PPI mPeiDefaultPciCfg2Ppi;
extern EFI_PEI_SERVICES mPeiServices;

PEI_CORE_INSTANCE mPrivateData = {
    PEI_CORE_HANDLE_SIGNATURE,
    &mPeiServices,
};

CONST EFI_PEI_SERVICES  **gPeiServices = &mPrivateData.Ps;

/**
  Caches a pointer PEI Services Table.

  Caches the pointer to the PEI Services Table specified by PeiServicesTablePointer
  in a CPU specific manner as specified in the CPU binding section of the Platform Initialization
  Pre-EFI Initialization Core Interface Specification.

  If PeiServicesTablePointer is NULL, then ASSERT().

  @param    PeiServicesTablePointer   The address of PeiServices pointer.
**/
VOID
EFIAPI
SetPeiServicesTablePointer (
  IN CONST EFI_PEI_SERVICES ** PeiServicesTablePointer
  )
{
  ASSERT (FALSE);
  ASSERT (PeiServicesTablePointer != NULL);
  gPeiServices = PeiServicesTablePointer;
}

/**
  Retrieves the cached value of the PEI Services Table pointer.

  Returns the cached value of the PEI Services Table pointer in a CPU specific manner
  as specified in the CPU binding section of the Platform Initialization Pre-EFI
  Initialization Core Interface Specification.

  If the cached PEI Services Table pointer is NULL, then ASSERT().

  @return  The pointer to PeiServices.

**/
CONST EFI_PEI_SERVICES **
EFIAPI
GetPeiServicesTablePointer (
  VOID
  )
{
  return gPeiServices;
}

/**
  Perform CPU specific actions required to migrate the PEI Services Table
  pointer from temporary RAM to permanent RAM.

  For IA32 CPUs, the PEI Services Table pointer is stored in the 4 bytes
  immediately preceding the Interrupt Descriptor Table (IDT) in memory.
  For X64 CPUs, the PEI Services Table pointer is stored in the 8 bytes
  immediately preceding the Interrupt Descriptor Table (IDT) in memory.
  For Itanium and ARM CPUs, a the PEI Services Table Pointer is stored in
  a dedicated CPU register.  This means that there is no memory storage
  associated with storing the PEI Services Table pointer, so no additional
  migration actions are required for Itanium or ARM CPUs.

**/
VOID
EFIAPI
MigratePeiServicesTablePointer (
  VOID
  )
{
  //
  //  PEI Services Table pointer is cached in the global variable. No additional
  //  migration actions are required.
  //
  ASSERT (FALSE);
  return;
}

EFI_STATUS
EFIAPI
PeiInstallPpi (
  IN CONST EFI_PEI_SERVICES        **PeiServices,
  IN CONST EFI_PEI_PPI_DESCRIPTOR  *PpiList
  );

EFI_STATUS
EFIAPI
PeiReInstallPpi (
  IN CONST EFI_PEI_SERVICES        **PeiServices,
  IN CONST EFI_PEI_PPI_DESCRIPTOR  *OldPpi,
  IN CONST EFI_PEI_PPI_DESCRIPTOR  *NewPpi
  );

EFI_STATUS
EFIAPI
PeiLocatePpi (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN CONST EFI_GUID              *Guid,
  IN UINTN                       Instance,
  IN OUT EFI_PEI_PPI_DESCRIPTOR  **PpiDescriptor,
  IN OUT VOID                    **Ppi
  );

EFI_STATUS
EFIAPI
PeiNotifyPpi (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN CONST EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyList
  );

EFI_STATUS
EFIAPI
PeiGetBootMode (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT   EFI_BOOT_MODE     *BootMode
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiSetBootMode (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN EFI_BOOT_MODE     BootMode
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiGetHobList (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **HobList
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiCreateHob (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT16            Type,
  IN UINT16            Length,
  IN OUT VOID          **Hob
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsFindNextVolume (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN UINTN                           Instance,
  IN OUT EFI_PEI_FV_HANDLE           *VolumeHandle
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsFindNextFile (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINT8                       SearchType,
  IN EFI_PEI_FV_HANDLE           FvHandle,
  IN OUT EFI_PEI_FILE_HANDLE     *FileHandle
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsFindSectionData (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN     EFI_SECTION_TYPE      SectionType,
  IN     EFI_PEI_FILE_HANDLE   FileHandle,
  OUT VOID                     **SectionData
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiInstallPeiMemory (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PHYSICAL_ADDRESS  MemoryBegin,
  IN UINT64                MemoryLength
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiAllocatePages (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       EFI_MEMORY_TYPE      MemoryType,
  IN       UINTN                Pages,
  OUT      EFI_PHYSICAL_ADDRESS *Memory
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiAllocatePool (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN UINTN                      Size,
  OUT VOID                      **Buffer
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiReportStatusCode (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiResetSystem (
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsFindFileByName (
  IN  CONST EFI_GUID        *FileName,
  IN  EFI_PEI_FV_HANDLE     VolumeHandle,
  OUT EFI_PEI_FILE_HANDLE   *FileHandle
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsGetFileInfo (
  IN EFI_PEI_FILE_HANDLE  FileHandle,
  OUT EFI_FV_FILE_INFO    *FileInfo
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsGetVolumeInfo (
  IN EFI_PEI_FV_HANDLE  VolumeHandle,
  OUT EFI_FV_INFO       *VolumeInfo
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiRegisterForShadow (
  IN EFI_PEI_FILE_HANDLE       FileHandle
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsFindSectionData3 (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN     EFI_SECTION_TYPE      SectionType,
  IN     UINTN                 SectionInstance,
  IN     EFI_PEI_FILE_HANDLE   FileHandle,
  OUT VOID                     **SectionData,
  OUT UINT32                   *AuthenticationStatus
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_STATUS
EFIAPI
PeiFfsGetFileInfo2 (
  IN EFI_PEI_FILE_HANDLE  FileHandle,
  OUT EFI_FV_FILE_INFO2   *FileInfo
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

VOID
EFIAPI
PeiResetSystem2 (
  IN EFI_RESET_TYPE     ResetType,
  IN EFI_STATUS         ResetStatus,
  IN UINTN              DataSize,
  IN VOID               *ResetData OPTIONAL
  )
{
  ASSERT (FALSE);
}

EFI_STATUS
EFIAPI
PeiFreePages (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PHYSICAL_ADDRESS       Memory,
  IN UINTN                      Pages
  )
{
  ASSERT (FALSE);
  return  EFI_NOT_AVAILABLE_YET;
}

EFI_PEI_SERVICES mPeiServices = {
  {
    PEI_SERVICES_SIGNATURE,
    PEI_SERVICES_REVISION,
    sizeof (EFI_PEI_SERVICES),
    0,
    0
  },
  PeiInstallPpi,
  PeiReInstallPpi,
  PeiLocatePpi,
  PeiNotifyPpi,

  PeiGetBootMode,
  PeiSetBootMode,

  PeiGetHobList,
  PeiCreateHob,

  PeiFfsFindNextVolume,
  PeiFfsFindNextFile,
  PeiFfsFindSectionData,

  PeiInstallPeiMemory,
  PeiAllocatePages,
  PeiAllocatePool,
  (EFI_PEI_COPY_MEM)CopyMem,
  (EFI_PEI_SET_MEM)SetMem,

  PeiReportStatusCode,
  PeiResetSystem,

  &mPeiDefaultCpuIoPpi,
  &mPeiDefaultPciCfg2Ppi,

  PeiFfsFindFileByName,
  PeiFfsGetFileInfo,
  PeiFfsGetVolumeInfo,
  PeiRegisterForShadow,
  PeiFfsFindSectionData3,
  PeiFfsGetFileInfo2,
  PeiResetSystem2,
  PeiFreePages,
};
