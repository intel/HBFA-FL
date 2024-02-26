/** @file
  General purpose supporting routines for FAT recovery PEIM

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "FatLitePeim.h"

EFI_STATUS
FatReadBlock (
  IN  PEI_FAT_PRIVATE_DATA   *PrivateData,
  IN  UINTN                  BlockDeviceNo,
  IN  EFI_PEI_LBA            Lba,
  IN  UINTN                  BufferSize,
  OUT VOID                   *Buffer
  )
{
	PEI_FAT_BLOCK_DEVICE  *BlockDev;

	if (BlockDeviceNo > PEI_FAT_MAX_BLOCK_DEVICE - 1) {
	return EFI_DEVICE_ERROR;
	}

	BlockDev  = &(PrivateData->BlockDevice[BlockDeviceNo]);

	if (BufferSize > MultU64x32 (BlockDev->LastBlock - Lba + 1, BlockDev->BlockSize)) {
	return EFI_DEVICE_ERROR;
	}

	if (Buffer == NULL) {
		return EFI_INVALID_PARAMETER;
	}

	if (BufferSize == 0) {
		return EFI_SUCCESS;
	}

	if ((BufferSize % BlockDev->BlockSize) != 0) {
		return EFI_BAD_BUFFER_SIZE;
	}

	if (Lba > BlockDev->LastBlock) {
		return EFI_INVALID_PARAMETER;
	}

	CopyMem(
		Buffer,
		(VOID *)(UINTN)(BlockDev->StartingPos + MultU64x32(Lba, BlockDev->BlockSize)),
		BufferSize
	);

	return EFI_SUCCESS;
}