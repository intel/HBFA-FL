/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DiskStubLib.h>

#include "Udf.h"

extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL gUdfSimpleFsTemplate;

EFI_STATUS
FindUdfFileSystem (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_DISK_IO_PROTOCOL   *DiskIo,
  OUT EFI_LBA               *StartingLBA,
  OUT EFI_LBA               *EndingLBA
  );

#define TOTAL_SIZE   (1 * 1024 * 1024)
#define BLOCK_SIZE   (1024)
#define IO_ALIGN     (1)
#define MAX_FILENAME_LEN   (4096)

VOID
FixBuffer (
  UINT8                   *TestBuffer
  )
{
}

PRIVATE_UDF_SIMPLE_FS_DATA       *PrivFsData = NULL;

EFI_FILE_PROTOCOL *
TestSimpleFileSystem (
  EFI_BLOCK_IO_PROTOCOL   *PartitionBlockIo,
  EFI_DISK_IO_PROTOCOL    *PartitionDiskIo,
  UINT8                   *TestBuffer
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *SimpleFs;
  EFI_FILE_PROTOCOL                *Root;

  
  PrivFsData->Signature = PRIVATE_UDF_SIMPLE_FS_DATA_SIGNATURE;
  PrivFsData->BlockIo   = PartitionBlockIo;
  PrivFsData->DiskIo    = PartitionDiskIo;
  //
  // Set up SimpleFs protocol
  //
  CopyMem ((VOID *)&PrivFsData->SimpleFs, (VOID *)&gUdfSimpleFsTemplate, sizeof (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL));

  SimpleFs = &PrivFsData->SimpleFs;
  
  // Test SimpleFs == NULL or &Root == NULL
  Status = SimpleFs->OpenVolume (
                       NULL,
                       &Root
                       );
  ASSERT (EFI_ERROR(Status));
  Status = SimpleFs->OpenVolume (
                       SimpleFs,
                       NULL
                       );
  ASSERT (EFI_ERROR(Status));
  
  // Test normal situation
  Status = SimpleFs->OpenVolume (
                       SimpleFs,
                       &Root
                       );
  if (!EFI_ERROR(Status)) {
    ASSERT (Root != NULL);
    return Root;
  } else {
    return NULL;
  }
}

VOID
TestFile (
  EFI_FILE_PROTOCOL                *Root,
  CHAR16                           *FileName
  )
{
  EFI_STATUS                       Status;
  EFI_FILE_HANDLE                  FileHandle;
  UINTN                            BufferSize;
  UINTN                            FileBufferSize;
  UINT64                           SourceFileSize;
  EFI_FILE_INFO                    *FileInfo = NULL;
  VOID                             *Buffer = NULL;

  FileHandle = NULL;
  Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR(Status)) {
    Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
      goto Done;
    }
  }
  ASSERT (FileHandle != NULL);
  
  //
  // Test UdfSetPosition
  //
  Status = FileHandle->SetPosition (FileHandle, (UINT64) -1);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
  //
  // Test UdfGetPosition
  //
  // Test FileHandle == NULL or &SourceFileSize == NULL
  Status = FileHandle->GetPosition (NULL, &SourceFileSize);
  ASSERT(EFI_ERROR(Status));
  
  Status = FileHandle->GetPosition (FileHandle, NULL);
  ASSERT(EFI_ERROR(Status));
  
  // Test normal situation
  Status = FileHandle->GetPosition (FileHandle, &SourceFileSize);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ASSERT (SourceFileSize <= MAX_UINTN);
  
  
  //
  // Test UdfSetPosition
  //
  // Test FileHandle == NULL
  Status = FileHandle->SetPosition (NULL, 0);
  ASSERT(EFI_ERROR(Status));
  
  // Test normal situation
  Status = FileHandle->SetPosition (FileHandle, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
  
  //
  // Test UdfRead
  //
  // Test FileHandle == NULL or &BufferSize == NULL
  BufferSize = (UINTN) SourceFileSize;
  Buffer =  AllocateZeroPool(BufferSize);
  if (Buffer == NULL) {
    goto Done;
  }

  BufferSize = (UINTN) SourceFileSize;
  
  Status = FileHandle->Read (NULL, &BufferSize, Buffer);
  ASSERT(EFI_ERROR(Status));
  Status = FileHandle->Read (FileHandle, NULL, Buffer);
  ASSERT(EFI_ERROR(Status));
  
  // Test Position > FileSize
  Status = FileHandle->SetPosition (FileHandle, SourceFileSize + 1);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = FileHandle->Read (FileHandle, &BufferSize, Buffer);
  ASSERT(EFI_ERROR(Status));
  
  // Test normal situation
  Status = FileHandle->SetPosition (FileHandle, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = FileHandle->Read (FileHandle, &BufferSize, Buffer);
  if (EFI_ERROR (Status) || BufferSize != (UINTN) SourceFileSize) {
    goto Done;
  }
  
  
  //
  // Test UdfGetInfo
  //
  // Test FileHandle == NULL or InfomationType == NULL or FileBufferSize == NULL
  FileBufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 1024;
  FileInfo =  AllocateZeroPool(FileBufferSize);
  if (FileInfo == NULL) {
    goto Done;
  }
  
  Status = FileHandle->GetInfo (NULL, &gEfiFileSystemInfoGuid, &FileBufferSize, FileInfo);
  ASSERT(EFI_ERROR(Status));
  
  Status = FileHandle->GetInfo (FileHandle, NULL, &FileBufferSize, FileInfo);
  ASSERT(EFI_ERROR(Status));
  
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemInfoGuid, NULL, FileInfo);
  ASSERT(EFI_ERROR(Status));
  
  // Test normal situation
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemInfoGuid, &FileBufferSize, FileInfo);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemVolumeLabelInfoIdGuid, &FileBufferSize, FileInfo);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
  //
  // Test UdfWrite
  //
  Status = FileHandle->Write (FileHandle, &FileBufferSize, FileInfo);
  ASSERT(EFI_ERROR (Status));
  
  
  //
  // Test SetInfo
  //
  Status = FileHandle-> SetInfo (FileHandle, &gEfiFileSystemInfoGuid, FileBufferSize, FileInfo);
  ASSERT(EFI_ERROR (Status));
  
Done:
  if (FileHandle != NULL) {
    Status = FileHandle->Close (NULL);
    Status = FileHandle->Close (FileHandle);
  }
  
  if (Buffer != NULL) {
    FreePool(Buffer);
    Buffer = NULL;
  }
  
  if (FileInfo != NULL) {
    FreePool(FileInfo);
    FileInfo = NULL;
  }
  return ;
}

VOID
TestDetele (
  EFI_FILE_PROTOCOL                *Root,
  CHAR16                           *FileName
  )
{
  EFI_STATUS                       Status;
  EFI_FILE_HANDLE                  FileHandle;

  FileHandle = NULL;
  Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR(Status)) {
    Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
      goto Done;
    }
  }
  ASSERT (FileHandle != NULL);
  
  Status = FileHandle->Delete (NULL);
  Status = FileHandle->Delete (FileHandle);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
Done:
  return ;
}

VOID
TestFlush (
  EFI_FILE_PROTOCOL                *Root,
  CHAR16                           *FileName
  )
{
  EFI_STATUS                       Status;
  EFI_FILE_HANDLE                  FileHandle;

  FileHandle = NULL;
  Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR(Status)) {
    Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
      goto Done;
    }
  }
  ASSERT (FileHandle != NULL);
  
  Status = FileHandle->Flush (FileHandle);
  ASSERT(EFI_ERROR (Status));
  
Done:
  if (FileHandle != NULL) {
    Status = FileHandle->Close (FileHandle);
  }
  return ;
}

VOID
TestDir (
  EFI_FILE_PROTOCOL                *Root,
  CHAR16                           *FileName
  )
{
  EFI_STATUS                       Status;
  EFI_FILE_HANDLE                  FileHandle;
  UINTN                            DirBufferSize;
  UINTN                            FileBufferSize;
  EFI_FILE_INFO                    *DirInfo = NULL;
  EFI_FILE_INFO                    *FileInfo = NULL;
  UINT64                           SourceFileSize;

  FileHandle = NULL;
  Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR(Status)) {
    Status = Root->Open (Root, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
      goto Done ;
    }
  }
  ASSERT (FileHandle != NULL);
  
  //
  // Test UdfRead
  //
  FileBufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 1024;
  FileInfo =  AllocateZeroPool(FileBufferSize);
  if (FileInfo == NULL) {
    goto Done ;
  }
  
  FileBufferSize = 0;
  Status = FileHandle->Read (FileHandle, &FileBufferSize, FileInfo);
  
  FileBufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 1024;
  Status = FileHandle->Read (FileHandle, &FileBufferSize, FileInfo);
  if (EFI_ERROR (Status)) {
    goto Done ;
  }
  
  //
  // Test UdfGetPosition
  //
  // As per UEFI spec, if the file handle is a directory, then the current file
  // position has no meaning and the operation is not supported.
  Status = FileHandle->GetPosition (FileHandle, &SourceFileSize);
  
  
  //
  // Test UdfSetPosition
  //
  Status = FileHandle->SetPosition (FileHandle, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
  //
  // Test UdfGetInfo
  //
  DirBufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 1024;
  DirInfo =  AllocateZeroPool(DirBufferSize);
  if (DirInfo == NULL) {
    goto Done ;
  }

  Status = FileHandle->GetInfo (FileHandle, &gEfiFileInfoGuid, &DirBufferSize, DirInfo);
  if (EFI_ERROR (Status)) {
    goto Done ;
  }
  
  DirBufferSize = 0;
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemInfoGuid, &DirBufferSize, DirInfo);
  ASSERT (EFI_ERROR (Status));
  
  DirBufferSize = 0;
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemVolumeLabelInfoIdGuid, &DirBufferSize, DirInfo);
  ASSERT (EFI_ERROR (Status));

  DirBufferSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 1024;
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemInfoGuid, &DirBufferSize, DirInfo);
  if (EFI_ERROR (Status)) {
    goto Done ;
  }
  
  Status = FileHandle->GetInfo (FileHandle, &gEfiFileSystemVolumeLabelInfoIdGuid, &DirBufferSize, DirInfo);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  
Done:
  if (FileHandle != NULL) {
    Status = FileHandle-> Close (NULL);
    Status = FileHandle-> Close (FileHandle);
  }
  
  if (DirInfo != NULL) {
    FreePool(DirInfo);
    DirInfo = NULL;
  }

  if (FileInfo != NULL) {
    FreePool(FileInfo);
    FileInfo = NULL;
  }

  return ;
}


VOID
TestUdfFileSystem (
  EFI_BLOCK_IO_PROTOCOL   *PartitionBlockIo,
  EFI_DISK_IO_PROTOCOL    *PartitionDiskIo,
  UINT8                   *TestBuffer,
  CHAR16                  *FileName
  )
{
  EFI_FILE_PROTOCOL                *Root;
  
  PrivFsData = (PRIVATE_UDF_SIMPLE_FS_DATA *)AllocateZeroPool (sizeof (PRIVATE_UDF_SIMPLE_FS_DATA));
  if (PrivFsData == NULL) {
    goto Done ;
  };

  Root = TestSimpleFileSystem (PartitionBlockIo, PartitionDiskIo, TestBuffer);
  if (Root == NULL) {
    goto Done ;
  }
  
  if (FileName == NULL) {
    TestFile (Root, L"udf.bin");
    TestFile (Root, L".\\\\b\\\\a");
    TestFile (Root, L"..\\a");
    TestFile (Root, L"b_link\\      a");
    TestFile (Root, L"...a");
    TestFile (Root, NULL);
    
    TestDir (Root, L"\\");
    TestDir (Root, L".");
    TestDir (Root, L"..");
    TestDir (Root, L"b_link");
    TestDir (Root, L"lost+found");
    TestDir (Root, L"b\\c\\");

    TestDetele (Root, L"b_link\\c");
    TestFlush (Root, L"b_link\\a_link");
    goto Done;
  } else {
    TestFile (Root, FileName);
    TestDir (Root, FileName);

    TestDetele (Root, FileName);
    TestFlush (Root, FileName);
    goto Done;
  }
Done:
  if (PrivFsData != NULL) {
    FreePool(PrivFsData);
    PrivFsData = NULL;
  };
  return;
}

UINTN
EFIAPI
GetMaxBufferSize (
  VOID
  )
{
  return TOTAL_SIZE;
}

VOID
EFIAPI
RunTestHarness(
  IN VOID  *TestBuffer,
  IN UINTN TestBufferSize
  )
{
  EFI_BLOCK_IO_PROTOCOL   *PartitionBlockIo;
  EFI_DISK_IO_PROTOCOL    *PartitionDiskIo;
  CHAR16                  *FileName;

  FileName = NULL;
  FixBuffer (TestBuffer);

  //
  // NOTE: There are 2 partitions: 0x101, and 0x202.
  // The files are updated in 0x202.
  //
  DiskStubInitialize ((UINT8 *)TestBuffer + 0x202 * BLOCK_SIZE, TOTAL_SIZE - 0x202 * BLOCK_SIZE, BLOCK_SIZE, IO_ALIGN, &PartitionBlockIo, &PartitionDiskIo);
  
  TestUdfFileSystem (PartitionBlockIo, PartitionDiskIo, TestBuffer, FileName);
  
  DiskStubDestory();
}
