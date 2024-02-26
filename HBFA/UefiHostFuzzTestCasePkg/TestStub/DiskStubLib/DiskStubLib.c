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

typedef struct {
  UINTN                   Signature;
  EFI_BLOCK_IO_PROTOCOL   BlockIo;
  EFI_BLOCK_IO_MEDIA      Media;
  EFI_DISK_IO_PROTOCOL    DiskIo;
  UINT64                  StartingAddr;
  UINT64                  Size;
} UDF_TEST_PRIVATE;

UDF_TEST_PRIVATE *UdfTestPrivate = NULL;

#define DATA_BUFFER_BLOCK_NUM           (64)

#define UDF_TEST_PRIVATE_SIGNATURE          SIGNATURE_32 ('U', 'D', 'F', 'T')

#define UDF_TEST_PRIVATE_FROM_BLOCK_IO(a)      CR (a, UDF_TEST_PRIVATE, BlockIo, UDF_TEST_PRIVATE_SIGNATURE)
#define UDF_TEST_PRIVATE_FROM_DISK_IO(a)       CR (a, UDF_TEST_PRIVATE, DiskIo, UDF_TEST_PRIVATE_SIGNATURE)

/**
  Reset the Block Device.

  @param  This                 Indicates a pointer to the calling context.
  @param  ExtendedVerification Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS          The device was reset.
  @retval EFI_DEVICE_ERROR     The device is not functioning properly and could
                               not be reset.

**/
EFI_STATUS
EFIAPI
Reset (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  return EFI_SUCCESS;
}

/**
  Read BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Lba        The starting Logical Block Address to read from
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the destination buffer for the data. The caller is
                     responsible for either having implicit or explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
ReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  )
{
  UDF_TEST_PRIVATE                *PrivateData;
  UINTN                           NumberOfBlocks;

  PrivateData = UDF_TEST_PRIVATE_FROM_BLOCK_IO (This);

  if (MediaId != PrivateData->Media.MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if ((BufferSize % PrivateData->Media.BlockSize) != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Lba > PrivateData->Media.LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  NumberOfBlocks = BufferSize / PrivateData->Media.BlockSize;
  if ((Lba + NumberOfBlocks - 1) > PrivateData->Media.LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (
    Buffer,
    (VOID *)(UINTN)(PrivateData->StartingAddr + MultU64x32 (Lba, PrivateData->Media.BlockSize)),
    BufferSize
    );

  return EFI_SUCCESS;
}

/**
  Write BufferSize bytes from Lba into Buffer.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written. The caller is
                     responsible for writing to only legitimate locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.

**/
EFI_STATUS
EFIAPI
WriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  IN VOID                           *Buffer
  )
{
  UDF_TEST_PRIVATE                *PrivateData;
  UINTN                           NumberOfBlocks;

  PrivateData = UDF_TEST_PRIVATE_FROM_BLOCK_IO (This);

  if (MediaId != PrivateData->Media.MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (TRUE == PrivateData->Media.ReadOnly) {
    return EFI_WRITE_PROTECTED;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if ((BufferSize % PrivateData->Media.BlockSize) != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Lba > PrivateData->Media.LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  NumberOfBlocks = BufferSize / PrivateData->Media.BlockSize;
  if ((Lba + NumberOfBlocks - 1) > PrivateData->Media.LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (
    (VOID *)(UINTN)(PrivateData->StartingAddr + MultU64x32 (Lba, PrivateData->Media.BlockSize)),
    Buffer,
    BufferSize
    );

  return EFI_SUCCESS;
}

/**
  Flush the Block Device.

  @param  This              Indicates a pointer to the calling context.

  @retval EFI_SUCCESS       All outstanding data was written to the device
  @retval EFI_DEVICE_ERROR  The device reported an error while writting back the data
  @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS
EFIAPI
FlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

/**
  Read BufferSize bytes from Offset into Buffer.

  @param  This                  Protocol instance pointer.
  @param  MediaId               Id of the media, changes every time the media is replaced.
  @param  Offset                The starting byte offset to read from
  @param  BufferSize            Size of Buffer
  @param  Buffer                Buffer containing read data

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_INVALID_PARAMETER The read request contains device addresses that are not
                                valid for the device.

**/
EFI_STATUS
EFIAPI
ReadDisk (
  IN EFI_DISK_IO_PROTOCOL         *This,
  IN UINT32                       MediaId,
  IN UINT64                       Offset,
  IN UINTN                        BufferSize,
  OUT VOID                        *Buffer
  )
{
  EFI_STATUS            Status;
  UDF_TEST_PRIVATE      *Private;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  EFI_BLOCK_IO_MEDIA    *Media;
  UINT32                BlockSize;
  UINT64                Lba;
  UINT64                OverRunLba;
  UINT32                UnderRun;
  UINT32                OverRun;
  BOOLEAN               TransactionComplete;
  UINTN                 WorkingBufferSize;
  UINT8                 *WorkingBuffer;
  UINTN                 Length;
  UINT8                 *Data;
  UINT8                 *PreData;
  UINTN                 IsBufferAligned;
  UINTN                 DataBufferSize;
  BOOLEAN               LastRead;

  Private   = UDF_TEST_PRIVATE_FROM_DISK_IO (This);

  BlockIo   = &Private->BlockIo;
  Media     = BlockIo->Media;
  BlockSize = Media->BlockSize;

  if (Media->MediaId != MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  WorkingBuffer     = Buffer;
  WorkingBufferSize = BufferSize;

  //
  // Allocate a temporary buffer for operation
  //
  DataBufferSize = BlockSize * DATA_BUFFER_BLOCK_NUM;

  if (Media->IoAlign > 1) {
    PreData = AllocatePool (DataBufferSize + Media->IoAlign);
    Data    = PreData - ((UINTN) PreData & (Media->IoAlign - 1)) + Media->IoAlign;
  } else {
    PreData = AllocatePool (DataBufferSize);
    Data    = PreData;
  }

  if (PreData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Lba                 = DivU64x32Remainder (Offset, BlockSize, &UnderRun);

  Length              = BlockSize - UnderRun;
  TransactionComplete = FALSE;

  Status              = EFI_SUCCESS;
  if (UnderRun != 0) {
    //
    // Offset starts in the middle of an Lba, so read the entire block.
    //
    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        MediaId,
                        Lba,
                        BlockSize,
                        Data
                        );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Length > BufferSize) {
      Length              = BufferSize;
      TransactionComplete = TRUE;
    }

    CopyMem (WorkingBuffer, Data + UnderRun, Length);

    WorkingBuffer += Length;

    WorkingBufferSize -= Length;
    if (WorkingBufferSize == 0) {
      goto Done;
    }

    Lba += 1;
  }

  OverRunLba = Lba + DivU64x32Remainder (WorkingBufferSize, BlockSize, &OverRun);

  if (!TransactionComplete && WorkingBufferSize >= BlockSize) {
    //
    // If the DiskIo maps directly to a BlockIo device do the read.
    //
    if (OverRun != 0) {
      WorkingBufferSize -= OverRun;
    }
    //
    // Check buffer alignment
    //
    IsBufferAligned = (UINTN) WorkingBuffer & (UINTN) (Media->IoAlign - 1);

    if (Media->IoAlign <= 1 || IsBufferAligned == 0) {
      //
      // Alignment is satisfied, so read them together
      //
      Status = BlockIo->ReadBlocks (
                          BlockIo,
                          MediaId,
                          Lba,
                          WorkingBufferSize,
                          WorkingBuffer
                          );

      if (EFI_ERROR (Status)) {
        goto Done;
      }

      WorkingBuffer += WorkingBufferSize;

    } else {
      //
      // Use the allocated buffer instead of the original buffer
      // to avoid alignment issue.
      // Here, the allocated buffer (8-byte align) can satisfy the alignment
      //
      LastRead = FALSE;
      do {
        if (WorkingBufferSize <= DataBufferSize) {
          //
          // It is the last calling to readblocks in this loop
          //
          DataBufferSize  = WorkingBufferSize;
          LastRead        = TRUE;
        }

        Status = BlockIo->ReadBlocks (
                            BlockIo,
                            MediaId,
                            Lba,
                            DataBufferSize,
                            Data
                            );
        if (EFI_ERROR (Status)) {
          goto Done;
        }

        CopyMem (WorkingBuffer, Data, DataBufferSize);
        WorkingBufferSize -= DataBufferSize;
        WorkingBuffer += DataBufferSize;
        Lba += DATA_BUFFER_BLOCK_NUM;
      } while (!LastRead);
    }
  }

  if (!TransactionComplete && OverRun != 0) {
    //
    // Last read is not a complete block.
    //
    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        MediaId,
                        OverRunLba,
                        BlockSize,
                        Data
                        );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    CopyMem (WorkingBuffer, Data, OverRun);
  }

Done:
  if (PreData != NULL) {
    FreePool (PreData);
  }

  return Status;
}

/**
  Writes a specified number of bytes to a device.

  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    ID of the medium to be written.
  @param  Offset     The starting byte offset on the logical block I/O device to write.
  @param  BufferSize The size in bytes of Buffer. The number of bytes to write to the device.
  @param  Buffer     A pointer to the buffer containing the data to be written.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_INVALID_PARAMETER The write request contains device addresses that are not
                                 valid for the device.

**/
EFI_STATUS
EFIAPI
WriteDisk (
  IN EFI_DISK_IO_PROTOCOL         *This,
  IN UINT32                       MediaId,
  IN UINT64                       Offset,
  IN UINTN                        BufferSize,
  IN VOID                         *Buffer
  )
{
  EFI_STATUS            Status;
  UDF_TEST_PRIVATE      *Private;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  EFI_BLOCK_IO_MEDIA    *Media;
  UINT32                BlockSize;
  UINT64                Lba;
  UINT64                OverRunLba;
  UINT32                UnderRun;
  UINT32                OverRun;
  BOOLEAN               TransactionComplete;
  UINTN                 WorkingBufferSize;
  UINT8                 *WorkingBuffer;
  UINTN                 Length;
  UINT8                 *Data;
  UINT8                 *PreData;
  UINTN                 IsBufferAligned;
  UINTN                 DataBufferSize;
  BOOLEAN               LastWrite;

  Private   = UDF_TEST_PRIVATE_FROM_DISK_IO (This);

  BlockIo   = &Private->BlockIo;
  Media     = BlockIo->Media;
  BlockSize = Media->BlockSize;

  if (Media->ReadOnly) {
    return EFI_WRITE_PROTECTED;
  }

  if (Media->MediaId != MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  DataBufferSize = BlockSize * DATA_BUFFER_BLOCK_NUM;

  if (Media->IoAlign > 1) {
    PreData = AllocatePool (DataBufferSize + Media->IoAlign);
    Data    = PreData - ((UINTN) PreData & (Media->IoAlign - 1)) + Media->IoAlign;
  } else {
    PreData = AllocatePool (DataBufferSize);
    Data    = PreData;
  }

  if (PreData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  WorkingBuffer       = Buffer;
  WorkingBufferSize   = BufferSize;

  Lba                 = DivU64x32Remainder (Offset, BlockSize, &UnderRun);

  Length              = BlockSize - UnderRun;
  TransactionComplete = FALSE;

  Status              = EFI_SUCCESS;
  if (UnderRun != 0) {
    //
    // Offset starts in the middle of an Lba, so do read modify write.
    //
    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        MediaId,
                        Lba,
                        BlockSize,
                        Data
                        );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (Length > BufferSize) {
      Length              = BufferSize;
      TransactionComplete = TRUE;
    }

    CopyMem (Data + UnderRun, WorkingBuffer, Length);

    Status = BlockIo->WriteBlocks (
                        BlockIo,
                        MediaId,
                        Lba,
                        BlockSize,
                        Data
                        );
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    WorkingBuffer += Length;
    WorkingBufferSize -= Length;
    if (WorkingBufferSize == 0) {
      goto Done;
    }

    Lba += 1;
  }

  OverRunLba = Lba + DivU64x32Remainder (WorkingBufferSize, BlockSize, &OverRun);

  if (!TransactionComplete && WorkingBufferSize >= BlockSize) {
    //
    // If the DiskIo maps directly to a BlockIo device do the write.
    //
    if (OverRun != 0) {
      WorkingBufferSize -= OverRun;
    }
    //
    // Check buffer alignment
    //
    IsBufferAligned = (UINTN) WorkingBuffer & (UINTN) (Media->IoAlign - 1);

    if (Media->IoAlign <= 1 || IsBufferAligned == 0) {
      //
      // Alignment is satisfied, so write them together
      //
      Status = BlockIo->WriteBlocks (
                          BlockIo,
                          MediaId,
                          Lba,
                          WorkingBufferSize,
                          WorkingBuffer
                          );

      if (EFI_ERROR (Status)) {
        goto Done;
      }

      WorkingBuffer += WorkingBufferSize;

    } else {
      //
      // The buffer parameter is not aligned with the request
      // So use the allocated instead.
      // It can fit almost all the cases.
      //
      LastWrite = FALSE;
      do {
        if (WorkingBufferSize <= DataBufferSize) {
          //
          // It is the last calling to writeblocks in this loop
          //
          DataBufferSize  = WorkingBufferSize;
          LastWrite       = TRUE;
        }

        CopyMem (Data, WorkingBuffer, DataBufferSize);
        Status = BlockIo->WriteBlocks (
                            BlockIo,
                            MediaId,
                            Lba,
                            DataBufferSize,
                            Data
                            );
        if (EFI_ERROR (Status)) {
          goto Done;
        }

        WorkingBufferSize -= DataBufferSize;
        WorkingBuffer += DataBufferSize;
        Lba += DATA_BUFFER_BLOCK_NUM;
      } while (!LastWrite);
    }
  }

  if (!TransactionComplete && OverRun != 0) {
    //
    // Last bit is not a complete block, so do a read modify write.
    //
    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        MediaId,
                        OverRunLba,
                        BlockSize,
                        Data
                        );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    CopyMem (Data, WorkingBuffer, OverRun);

    Status = BlockIo->WriteBlocks (
                        BlockIo,
                        MediaId,
                        OverRunLba,
                        BlockSize,
                        Data
                        );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

Done:
  if (PreData != NULL) {
    FreePool (PreData);
  }

  return Status;
}

UDF_TEST_PRIVATE mUdfTestPrivate = {
  UDF_TEST_PRIVATE_SIGNATURE,
  {
    EFI_BLOCK_IO_PROTOCOL_REVISION,
    &mUdfTestPrivate.Media,
    Reset,
    ReadBlocks,
    WriteBlocks,
    FlushBlocks,
  },
  {
    0,     // MediaId;
    FALSE, // RemovableMedia;
    TRUE,  // MediaPresent;
    FALSE, // LogicalPartition;
    FALSE, // ReadOnly;
    FALSE, // WriteCaching;
    0,     // BlockSize;
    1,     // IoAlign;
    0,     // LastBlock;
    0, // LowestAlignedLba;
    0, // LogicalBlocksPerPhysicalBlock;
    0, // OptimalTransferLengthGranularity;
  },
  {
    EFI_DISK_IO_PROTOCOL_REVISION,
    ReadDisk,
    WriteDisk,
  },
  0, // StartingAddr
  0, // Size
};

EFI_STATUS
EFIAPI
DiskStubInitialize (
  IN  VOID                   *Buffer,
  IN  UINTN                  BufferSize,
  IN  UINT32                 BlockSize,
  IN  UINT32                 IoAlign,
  OUT EFI_BLOCK_IO_PROTOCOL  **BlockIo,
  OUT EFI_DISK_IO_PROTOCOL   **DiskIo
  )
{
  UdfTestPrivate = AllocatePool (sizeof(UDF_TEST_PRIVATE));
  CopyMem (UdfTestPrivate, &mUdfTestPrivate, sizeof(UDF_TEST_PRIVATE));

  UdfTestPrivate->BlockIo.Media = &UdfTestPrivate->Media;

  UdfTestPrivate->StartingAddr = (UINTN)Buffer;
  UdfTestPrivate->Size         = BufferSize;

  UdfTestPrivate->Media.IoAlign   = IoAlign;
  UdfTestPrivate->Media.BlockSize = BlockSize;
  UdfTestPrivate->Media.LastBlock = (BufferSize + BlockSize - 1) / BlockSize - 1;

  if (BlockIo != NULL) {
    *BlockIo = &UdfTestPrivate->BlockIo;
  }
  if (DiskIo != NULL) {
    *DiskIo = &UdfTestPrivate->DiskIo;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DiskStubDestory (
  VOID
  )
{
  if (UdfTestPrivate != NULL) {
    FreePool(UdfTestPrivate);
    UdfTestPrivate = NULL;
  }
  
  return EFI_SUCCESS;
}