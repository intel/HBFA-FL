/** @file
  Instance of SMM memory check library.

  SMM memory check library library implementation. This library consumes SMM_ACCESS2_PROTOCOL
  to get SMRAM information. In order to use this library instance, the platform should produce
  all SMRAM range via SMM_ACCESS2_PROTOCOL, including the range for firmware (like SMM Core
  and SMM driver) and/or specific dedicated hardware.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <PiSmm.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmMemLib.h>

typedef struct {
  EFI_PHYSICAL_ADDRESS  Address;
  UINT64                Size;
} SMM_COMMUNICATION_BUFFER_DESCRIPTOR;

UINTN                               mSmmCommBufferDescCount;
SMM_COMMUNICATION_BUFFER_DESCRIPTOR *mSmmCommBufferDesc;

/**
  This function check if the buffer is valid per processor architecture and not overlap with SMRAM.

  @param Buffer  The buffer start address to be checked.
  @param Length  The buffer length to be checked.

  @retval TRUE  This buffer is valid per processor architecture and not overlap with SMRAM.
  @retval FALSE This buffer is not valid per processor architecture or overlap with SMRAM.
**/
BOOLEAN
EFIAPI
SmmIsBufferOutsideSmmValid (
  IN EFI_PHYSICAL_ADDRESS  Buffer,
  IN UINT64                Length
  )
{
  UINTN                                Index;
  SMM_COMMUNICATION_BUFFER_DESCRIPTOR  *SmmCommBufferDesc;
  BOOLEAN                              InValidCommunicationRegion;

  InValidCommunicationRegion = FALSE;
  SmmCommBufferDesc = mSmmCommBufferDesc;
  for (Index = 0; Index < mSmmCommBufferDescCount; Index++) {
    if ((Buffer >= SmmCommBufferDesc->Address) &&
        (Buffer + Length <= SmmCommBufferDesc->Address + SmmCommBufferDesc->Size)) {
      InValidCommunicationRegion = TRUE;
    }
    SmmCommBufferDesc++;
  }

  if (!InValidCommunicationRegion) {
    DEBUG ((
      EFI_D_ERROR,
      "SmmIsBufferOutsideSmmValid: Not in ValidCommunicationRegion: Buffer (0x%lx) - Length (0x%lx)\n",
      Buffer,
      Length
      ));
    return FALSE;
  }

  return TRUE;
}

/**
  Copies a source buffer (non-SMRAM) to a destination buffer (SMRAM).

  This function copies a source buffer (non-SMRAM) to a destination buffer (SMRAM).
  It checks if source buffer is valid per processor architecture and not overlap with SMRAM.
  If the check passes, it copies memory and returns EFI_SUCCESS.
  If the check fails, it return EFI_SECURITY_VIOLATION.
  The implementation must be reentrant.

  @param  DestinationBuffer   The pointer to the destination buffer of the memory copy.
  @param  SourceBuffer        The pointer to the source buffer of the memory copy.
  @param  Length              The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @retval EFI_SECURITY_VIOLATION The SourceBuffer is invalid per processor architecture or overlap with SMRAM.
  @retval EFI_SUCCESS            Memory is copied.

**/
EFI_STATUS
EFIAPI
SmmCopyMemToSmram (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  if (!SmmIsBufferOutsideSmmValid ((EFI_PHYSICAL_ADDRESS)(UINTN)SourceBuffer, Length)) {
    DEBUG ((EFI_D_ERROR, "SmmCopyMemToSmram: Security Violation: Source (0x%x), Length (0x%x)\n", SourceBuffer, Length));
    return EFI_SECURITY_VIOLATION;
  }
  CopyMem (DestinationBuffer, SourceBuffer, Length);
  return EFI_SUCCESS;
}

/**
  Copies a source buffer (SMRAM) to a destination buffer (NON-SMRAM).

  This function copies a source buffer (non-SMRAM) to a destination buffer (SMRAM).
  It checks if destination buffer is valid per processor architecture and not overlap with SMRAM.
  If the check passes, it copies memory and returns EFI_SUCCESS.
  If the check fails, it returns EFI_SECURITY_VIOLATION.
  The implementation must be reentrant.

  @param  DestinationBuffer   The pointer to the destination buffer of the memory copy.
  @param  SourceBuffer        The pointer to the source buffer of the memory copy.
  @param  Length              The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @retval EFI_SECURITY_VIOLATION The DesinationBuffer is invalid per processor architecture or overlap with SMRAM.
  @retval EFI_SUCCESS            Memory is copied.

**/
EFI_STATUS
EFIAPI
SmmCopyMemFromSmram (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  if (!SmmIsBufferOutsideSmmValid ((EFI_PHYSICAL_ADDRESS)(UINTN)DestinationBuffer, Length)) {
    DEBUG ((EFI_D_ERROR, "SmmCopyMemFromSmram: Security Violation: Destination (0x%x), Length (0x%x)\n", DestinationBuffer, Length));
    return EFI_SECURITY_VIOLATION;
  }
  CopyMem (DestinationBuffer, SourceBuffer, Length);
  return EFI_SUCCESS;
}

/**
  Copies a source buffer (NON-SMRAM) to a destination buffer (NON-SMRAM).

  This function copies a source buffer (non-SMRAM) to a destination buffer (SMRAM).
  It checks if source buffer and destination buffer are valid per processor architecture and not overlap with SMRAM.
  If the check passes, it copies memory and returns EFI_SUCCESS.
  If the check fails, it returns EFI_SECURITY_VIOLATION.
  The implementation must be reentrant, and it must handle the case where source buffer overlaps destination buffer.

  @param  DestinationBuffer   The pointer to the destination buffer of the memory copy.
  @param  SourceBuffer        The pointer to the source buffer of the memory copy.
  @param  Length              The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @retval EFI_SECURITY_VIOLATION The DesinationBuffer is invalid per processor architecture or overlap with SMRAM.
  @retval EFI_SECURITY_VIOLATION The SourceBuffer is invalid per processor architecture or overlap with SMRAM.
  @retval EFI_SUCCESS            Memory is copied.

**/
EFI_STATUS
EFIAPI
SmmCopyMem (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  if (!SmmIsBufferOutsideSmmValid ((EFI_PHYSICAL_ADDRESS)(UINTN)DestinationBuffer, Length)) {
    DEBUG ((EFI_D_ERROR, "SmmCopyMem: Security Violation: Destination (0x%x), Length (0x%x)\n", DestinationBuffer, Length));
    return EFI_SECURITY_VIOLATION;
  }
  if (!SmmIsBufferOutsideSmmValid ((EFI_PHYSICAL_ADDRESS)(UINTN)SourceBuffer, Length)) {
    DEBUG ((EFI_D_ERROR, "SmmCopyMem: Security Violation: Source (0x%x), Length (0x%x)\n", SourceBuffer, Length));
    return EFI_SECURITY_VIOLATION;
  }
  CopyMem (DestinationBuffer, SourceBuffer, Length);
  return EFI_SUCCESS;
}

/**
  Fills a target buffer (NON-SMRAM) with a byte value.

  This function fills a target buffer (non-SMRAM) with a byte value.
  It checks if target buffer is valid per processor architecture and not overlap with SMRAM.
  If the check passes, it fills memory and returns EFI_SUCCESS.
  If the check fails, it returns EFI_SECURITY_VIOLATION.

  @param  Buffer    The memory to set.
  @param  Length    The number of bytes to set.
  @param  Value     The value with which to fill Length bytes of Buffer.

  @retval EFI_SECURITY_VIOLATION The Buffer is invalid per processor architecture or overlap with SMRAM.
  @retval EFI_SUCCESS            Memory is set.

**/
EFI_STATUS
EFIAPI
SmmSetMem (
  OUT VOID  *Buffer,
  IN UINTN  Length,
  IN UINT8  Value
  )
{
  if (!SmmIsBufferOutsideSmmValid ((EFI_PHYSICAL_ADDRESS)(UINTN)Buffer, Length)) {
    DEBUG ((EFI_D_ERROR, "SmmSetMem: Security Violation: Source (0x%x), Length (0x%x)\n", Buffer, Length));
    return EFI_SECURITY_VIOLATION;
  }
  SetMem (Buffer, Length, Value);
  return EFI_SUCCESS;
}

VOID
EFIAPI
SmmMemLibInitialize (
  IN UINTN                               SmmCommBufferDescCount,
  IN SMM_COMMUNICATION_BUFFER_DESCRIPTOR *SmmCommBufferDesc
  )
{
  mSmmCommBufferDescCount = SmmCommBufferDescCount;
  mSmmCommBufferDesc = SmmCommBufferDesc;
}

