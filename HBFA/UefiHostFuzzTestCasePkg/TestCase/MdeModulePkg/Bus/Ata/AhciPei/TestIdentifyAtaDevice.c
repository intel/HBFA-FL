/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef TEST_WITH_LIBFUZZER
#include <stdint.h>
#include <stddef.h>
#endif

#include <Uefi.h>
#include "AhciPei.h"

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include "AhciPei.h"

#define TOTAL_SIZE (512 * 1024)
VOID FixBuffer(
    UINT8 *TestBuffer,
    UINTN TestBufferSize)
{
}

UINTN
EFIAPI
GetMaxBufferSize(
    VOID)
{
  return TOTAL_SIZE;
}

VOID
    EFIAPI
    RunTestHarness(
        IN VOID *TestBuffer,
        IN UINTN TestBufferSize,
        PEI_AHCI_ATA_DEVICE_DATA *DeviceDataPtr)
{
  EFI_STATUS                          Status;
  PEI_AHCI_CONTROLLER_PRIVATE_DATA    *Private;
  ATA_IDENTIFY_DATA                   *IdentifyData1;
  UINTN                               DeviceIndex;
  UINTN                               DataBufferSize;
  VOID                                *DataBuffer;
  UINTN                               DataTransferSize;

  FixBuffer(TestBuffer, TestBufferSize);
  
  IdentifyData1 = (ATA_IDENTIFY_DATA *)TestBuffer;

  Private = AllocateZeroPool (sizeof (PEI_AHCI_CONTROLLER_PRIVATE_DATA));
  ASSERT (Private != NULL);

  Private->Signature        = AHCI_PEI_CONTROLLER_PRIVATE_DATA_SIGNATURE;
  Private->PortBitMap       = 0x1;
  InitializeListHead (&Private->DeviceList);

  Status = AhciCreateTransferDescriptor (Private);
  ASSERT_EFI_ERROR (Status);

  Private->AtaPassThruMode.Attributes   = EFI_ATA_PASS_THRU_ATTRIBUTES_PHYSICAL |
                                          EFI_ATA_PASS_THRU_ATTRIBUTES_LOGICAL;
  Private->AtaPassThruMode.IoAlign      = sizeof (UINTN);
  Private->AtaPassThruPpi.Revision      = EDKII_PEI_ATA_PASS_THRU_PPI_REVISION;
  Private->AtaPassThruPpi.Mode          = &Private->AtaPassThruMode;
  Private->AtaPassThruPpi.PassThru      = AhciAtaPassThruPassThru;
  Private->AtaPassThruPpi.GetNextPort   = AhciAtaPassThruGetNextPort;
  Private->AtaPassThruPpi.GetNextDevice = AhciAtaPassThruGetNextDevice;
  Private->AtaPassThruPpi.GetDevicePath = AhciAtaPassThruGetDevicePath;
  
  DeviceIndex = 1;
  Status = CreateNewDevice (Private, DeviceIndex, 1, 0xFFFF, 0, IdentifyData1);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Test #1 for Storage Security Command PPI, ReceiveData()
  //
  DataBufferSize = 0x10;
  DataBuffer     = AllocatePool (DataBufferSize);
  ASSERT (DataBuffer != NULL);
  AhciStorageSecurityReceiveData (
    &Private->StorageSecurityPpi,
    DeviceIndex,
    ATA_TIMEOUT,
    0,
    0,
    DataBufferSize,
    DataBuffer,
    &DataTransferSize
    );

  //
  // Test #2 for Storage Security Command PPI, SendData()
  //
  DataBufferSize = 0x10;
  AhciStorageSecuritySendData (
    &Private->StorageSecurityPpi,
    DeviceIndex,
    ATA_TIMEOUT,
    0,
    0,
    DataBufferSize,
    DataBuffer
    );
  FreePool (DataBuffer);

  //
  // Test #1 for Block IO PPI, ReadBlocks() - with small DataBufferSize.
  //
  DataBuffer     = AllocatePool (0x200000);
  ASSERT (DataBuffer != NULL);
  DataBufferSize = 0x800;  
  AhciBlockIoReadBlocks (
    NULL,
    &Private->BlkIoPpi,
    DeviceIndex,
    0,
    DataBufferSize,
    DataBuffer
    );

  //
  // Test #2 for Block IO PPI, ReadBlocks() - with large DataBufferSize.
  //
  DataBufferSize = 0x200000;  
  AhciBlockIoReadBlocks (
    NULL,
    &Private->BlkIoPpi,
    DeviceIndex,
    0,
    DataBufferSize,
    DataBuffer
    );
  FreePool (DataBuffer);

  return;
}
