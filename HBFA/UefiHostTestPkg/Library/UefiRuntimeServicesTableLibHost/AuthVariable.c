/**
  Manage & verify Timebased Authenticated varaibles.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "AuthVariable.h"
#include <Library/BaseCryptLib.h>

extern LIST_ENTRY mVarListEntry;

/*
  Function will split the "Buffer" parameter and getting the Auth2
  descriptor, and doing verification.

  @param[in]  Attributes                  Attribute value of the variable.
  @param[in]  Buffer                      Data pointer.
  @param[in]  BufferSize                  The size of Data in bytes.
  @param[in]  OrgTimeStamp                Pointer to original time stamp,
                                          original variable is not found if NULL.

  @retval     EFI_SECURITY_VIOLATION      Verification fail.
  @retval     EFI_SUCCESS                 Operation success.
*/
EFI_STATUS
VerifyAuth2Descriptor (
  IN    UINT32                        Attributes,
  IN    VOID                          *Buffer,
  IN    UINTN                         BufferSize,
  IN    EFI_TIME                      *OrgTimeStamp,
  OUT   UINT8                         **CertDataPtr,
  OUT   UINT32                        *CertDataSize
  )
{
  EFI_VARIABLE_AUTHENTICATION_2   *Auth2Ptr;
  EFI_TIME                        *Auth2TimeStamp;

  if (Buffer == NULL || CertDataPtr == NULL || CertDataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Auth2Ptr = (EFI_VARIABLE_AUTHENTICATION_2 *)Buffer;

  Auth2TimeStamp = &Auth2Ptr->TimeStamp;
  if ( (Auth2TimeStamp->Pad1 != 0) ||
       (Auth2TimeStamp->Pad2 != 0) ||
       (Auth2TimeStamp->Nanosecond != 0) ||
       (Auth2TimeStamp->TimeZone != 0) ||
       (Auth2TimeStamp->Daylight !=0)
     ) {
    return EFI_SECURITY_VIOLATION;
  }

  if ((OrgTimeStamp != NULL) && ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0)) {
    //
    // TimeStamp check fail, suspicious replay attack, return EFI_SECURITY_VIOLATION.
    //
    if (CompareTimeStamp (Auth2TimeStamp, OrgTimeStamp)) {
      return EFI_SECURITY_VIOLATION;
    }
  }

  if ( (Auth2Ptr->AuthInfo.Hdr.wCertificateType != WIN_CERT_TYPE_EFI_GUID) ||
       (!CompareGuid (&Auth2Ptr->AuthInfo.CertType, &gEfiCertPkcs7Guid))
     ) {
    //
    // Invalid AuthInfo type, return EFI_SECURITY_VIOLATION.
    //
    return EFI_SECURITY_VIOLATION;
  }

  *CertDataPtr = ((UINT8 *)&Auth2Ptr->AuthInfo.CertType) + sizeof (EFI_GUID);
  *CertDataSize = Auth2Ptr->AuthInfo.Hdr.dwLength - (UINT32)(OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertType)) - sizeof (EFI_GUID);

  return EFI_SUCCESS;
}

/*
  This function will according to the opeartion type to processing the
  certs of auth variable.

  @param[in]  VariableName                Name of Variable to be found.
  @param[in]  VendorGuid                  Variable vendor GUID.
  @param[in]  Attributes                  Attribute value of the variable.
  @param[in]  OperationType               Indicate the operation of auth variable.
  @param[in]  PayloadSize                 Indicate the size of payload in bytes.
  @param[in]  SignerCert                  A pointer to SignerCert data.
  @param[in]  SignerCertSize              Length of SignerCert data.
  @param[in]  TopLevelCert                A pointer to TopLevelCert data.
  @param[in]  TopLevelCertSize            Length of TopLevelCert data.

  @retval     EFI_INVALID_PARAMETER       Invalid input parameters.
  @retval     EFI_SECURITY_VIOLATION      Verify or update certs failed.
  @retval     EFI_SUCCESS                 The operation is finished successfully.
  @retval     Others                      Other errors as indicated.
*/
EFI_STATUS
VerifyCertsAndUpdate (
  IN    CHAR16                        *VariableName,
  IN    EFI_GUID                      *VendorGuid,
  IN    UINT32                        Attributes,
  IN    EFI_COMMAND_OPER_TYPE         OperationType,
  IN    UINT32                        PayloadSize,
  IN    UINT8                         *SignerCert,
  IN    UINT32                        SignerCertSize,
  IN    UINT8                         *TopLevelCert,
  IN    UINT32                        TopLevelCertSize
  )
{
  EFI_STATUS                          Status;
  EFI_CERT_DATA                       *TempPtr;
  UINT8                               *CertDataPtr;
  UINT32                              CertDataSize;
  VARIABLE_INFO_PRIVATE               *PriVarInfo;

  Status                              = EFI_SUCCESS;
  TempPtr                             = NULL;
  CertDataPtr                         = NULL;
  CertDataSize                        = 0;

  if ( OperationType >= VariableTypeMax ||
       VariableName == NULL ||
       VendorGuid == NULL ||
       SignerCert == NULL ||
       TopLevelCert == NULL
     ) {
    return EFI_INVALID_PARAMETER;
  }

  PriVarInfo = FindVariableInfoPtr (VariableName, VendorGuid);

  /*
    Add:
      1. Create: certdb, variable.    OrgTimeStamp == NULL, payloadsize != 0.
      2. Modify: variable.            OrgTimeStamp != NULL, payloadsize != 0.

    Del:
      1. Delete: certdb, variable.    payloadsize == 0.

    Append:(No update timestamp)
      1. Create: certdb, variable.    OrgTimeStamp == NULL, payloadsize != 0.
      2. Modify: variable.            OrgTimeStamp != NULL, payloadsize != 0.
  */

  //
  //  Specified behavior is not matching the payload buffer.
  //  In this situation will delete certs from certdb but add variable.
  //
  if (OperationType == VariableTypeDel && PayloadSize != 0) {
    return EFI_SECURITY_VIOLATION;
  }

  //
  //  Specified behavior is not matching the payload buffer.
  //  In this situation will add certs into certdb but delete variable.
  //
  if ( (OperationType == VariableTypeAdd || OperationType == VariableTypeAppend) &&
       (PayloadSize == 0)
     ) {
    return EFI_SECURITY_VIOLATION;
  }

  if ((OperationType == VariableTypeAdd || OperationType == VariableTypeAppend) && (PriVarInfo == NULL)) {
     TempPtr = (EFI_CERT_DATA *)(SignerCert + 1);
     Status = InsertCertsToDb (
                VariableName,
                VendorGuid,
                Attributes,
                TempPtr->CertDataBuffer,
                ReadUnaligned32 ((UINT32 *)&TempPtr->CertDataLength),
                TopLevelCert,
                TopLevelCertSize
                );
      if (EFI_ERROR (Status)) {
        return Status;
      }
  } else {
    Status = FindCertsFromDb (
               VariableName,
               VendorGuid,
               Attributes,
               NULL,
               NULL,
               &CertDataPtr,
               &CertDataSize
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (CertDataSize == SHA256_DIGEST_SIZE) {
#if 0
      UINT8  Sha256Digest[SHA256_DIGEST_SIZE];
      TempPtr = (EFI_CERT_DATA *)(SignerCert + 1);
      Status = CalculatePrivAuthVarSignChainSHA256Digest (
                 TempPtr->CertDataBuffer,
                 ReadUnaligned32 ((UINT32 *)&TempPtr->CertDataLength),
                 TopLevelCert,
                 TopLevelCertSize,
                 Sha256Digest
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if (CompareMem (Sha256Digest, CertDataPtr, CertDataSize) != 0) {
        return EFI_SECURITY_VIOLATION;
      }
#endif
    } else {
      if (CertDataSize != TopLevelCertSize) {
        return EFI_SECURITY_VIOLATION;
      }

      if (CompareMem (SignerCert, CertDataPtr, CertDataSize) != 0) {
        return EFI_SECURITY_VIOLATION;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Process variable with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set

  @param[in]  VariableName                Name of Variable to be found.
  @param[in]  VendorGuid                  Variable vendor GUID.
  @param[in]  Attributes                  Attribute value of the variable.
  @param[in]  Buffer                      Data pointer.
  @param[in]  BufferSize                  Size of Data found. If size is less than the
                                          data, this value contains the required size.
  @param[in]  OrgTimeStamp                Pointer to original time stamp,
                                          original variable is not found if NULL.
  @param[in]  OperationType               Indicates how to process the auth variable.
  @param[out] VarPayloadPtr               Pointer to variable payload address.
  @param[out] VarPayloadSize              Pointer to variable payload size.
  @param[out] CertTimeStamp               Pointer to timestamp of auth2 structure.

  @retval EFI_INVALID_PARAMETER           Invalid parameter.
  @retval EFI_SECURITY_VIOLATION          The variable does NOT pass the validation
                                          check carried out by the firmware.
  @retval EFI_OUT_OF_RESOURCES            Failed to process variable due to lack
                                          of resources.
  @retval EFI_SUCCESS                     Variable pass validation successfully.
**/
EFI_STATUS
Auth2VerifyAndUpdate (
  IN    CHAR16                        *VariableName,
  IN    EFI_GUID                      *VendorGuid,
  IN    UINT32                        Attributes,
  IN    VOID                          *Buffer,
  IN    UINTN                         BufferSize,
  IN    EFI_TIME                      *OrgTimeStamp,
  IN    EFI_COMMAND_OPER_TYPE         OperationType,
  OUT   UINT8                         **VarPayloadPtr,
  OUT   UINTN                         *VarPayloadSize,
  OUT   EFI_TIME                      **CertTimeStamp
  )
{
#if 0
  EFI_STATUS                      Status;
  BOOLEAN                         VerifyStatus;
  EFI_VARIABLE_AUTHENTICATION_2   *Auth2Ptr;
  UINT8                           *CertDataPtr;
  UINT32                          CertDataSize;
  UINT8                           *PayloadPtr;
  UINT32                          PayloadSize;
  UINT8                           *Digest;
  UINT32                          DigestSize;
  UINT8                           *SignerCert;
  UINT32                          SignerCertSize;
  UINT8                           *TopLevelCert;
  UINT32                          TopLevelCertSize;
  UINT32                          Offset;
  VARIABLE_INFO_PRIVATE           *PriVarInfo;

  Status                          = EFI_SUCCESS;
  Digest                          = NULL;
  Offset                          = 0;

  PriVarInfo = FindVariableInfoPtr (VariableName, VendorGuid);

  Status = VerifyAuth2Descriptor (
             Attributes,
             Buffer,
             BufferSize,
             PriVarInfo == NULL ? NULL : &PriVarInfo->TimeStamp,
             &CertDataPtr,
             &CertDataSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Auth2Ptr = (EFI_VARIABLE_AUTHENTICATION_2 *)Buffer;

  //
  // Find out the new data payload which follows Pkcs7 SignedData directly.
  //
  PayloadPtr = CertDataPtr + CertDataSize;
  PayloadSize = (UINT32)(BufferSize - sizeof (EFI_TIME) - Auth2Ptr->AuthInfo.Hdr.dwLength);

  DigestSize =  (UINT32)(PayloadSize +
                StrLen (VariableName) * sizeof (CHAR16) +
                sizeof (UINT32) +
                sizeof (EFI_TIME) +
                sizeof (EFI_GUID));
  Digest = (UINT8 *)malloc (DigestSize);
  if (Digest == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (Digest + Offset, VariableName, StrLen (VariableName) * sizeof (CHAR16));
  Offset = (UINT32)(Offset + StrLen (VariableName) * sizeof (CHAR16));

  CopyMem (Digest + Offset, VendorGuid, sizeof (EFI_GUID));
  Offset = Offset + sizeof (EFI_GUID);

  CopyMem (Digest + Offset, &Attributes, sizeof (UINT32));
  Offset = Offset + sizeof (UINT32);

  CopyMem (Digest + Offset, &Auth2Ptr->TimeStamp, sizeof (EFI_TIME));
  Offset = Offset + sizeof (EFI_TIME);

  CopyMem (Digest + Offset, PayloadPtr, PayloadSize);

  VerifyStatus = Pkcs7GetSigners (
                   CertDataPtr,
                   CertDataSize,
                   &SignerCert,
                   (UINTN *)&SignerCertSize,
                   &TopLevelCert,
                   (UINTN *)&TopLevelCertSize
                   );
  if (!VerifyStatus) {
    goto ON_EXIT;
  }

  VerifyStatus = Pkcs7Verify (
                   CertDataPtr,
                   CertDataSize,
                   TopLevelCert,
                   TopLevelCertSize,
                   Digest,
                   DigestSize
                   );
  if (!VerifyStatus) {
    goto ON_EXIT;
  }

  Status = VerifyCertsAndUpdate (
             VariableName,
             VendorGuid,
             Attributes,
             OperationType,
             PayloadSize,
             SignerCert,
             SignerCertSize,
             TopLevelCert,
             TopLevelCertSize
             );

  if (EFI_ERROR (Status)) {
    VerifyStatus = FALSE;
  }

ON_EXIT:
  Pkcs7FreeSigners (TopLevelCert);
  Pkcs7FreeSigners (SignerCert);

  FREE_NON_NULL_PTR (Digest);

  if (!VerifyStatus) {
    return EFI_SECURITY_VIOLATION;
  }

  //
  // Find out the new data payload which follows Pkcs7 SignedData directly.
  //
  *VarPayloadPtr = CertDataPtr + CertDataSize;
  *VarPayloadSize = BufferSize - sizeof (EFI_TIME) - Auth2Ptr->AuthInfo.Hdr.dwLength;
  *CertTimeStamp = &Auth2Ptr->TimeStamp;

  return EFI_SUCCESS;
#else 
  return EFI_UNSUPPORTED;
#endif
}

/*
  Process variable with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set.

  @param[in]    Name                      Name of Variable to be processed.
  @param[in]    Guid                      Variable vendor GUID.
  @param[in]    Data                      Data pointer.
  @param[in]    DataSize                  The size of Data in bytes.
  @param[in]    Attributes                Attribute value of the variable.
  @param[in]    OperType                  The operation type about variable.

  @retval       EFI_SUCCESS               The operation is finished successfully.
  @retval       EFI_INVALID_PARAMETER     Invalid input parameters, Name, Guid or Data is NULL.
  @retval       EFI_OUT_OF_RESOURCES      No more memory for allocation.
  @retval       Others                    Other errors as indicated.
*/
EFI_STATUS
ProcessAuthVar (
  IN    CHAR16                      *Name,
  IN    EFI_GUID                    *Guid,
  IN    VOID                        *Data,
  IN    UINTN                       DataSize,
  IN    UINT32                      Attributes,
  IN    EFI_COMMAND_OPER_TYPE       OperType
  )
{
  EFI_STATUS                      Status;
  EFI_TIME                        *CertTimeStamp;
  LIST_ENTRY                      *ListEntry;
  VARIABLE_INFO_PRIVATE           *PriVarInfo;
  UINT8                           *PayloadPtr;
  UINTN                           PayloadSize;

  Status                          = EFI_SUCCESS;
  CertTimeStamp                   = NULL;
  ListEntry                       = NULL;
  PriVarInfo                      = NULL;
  PayloadPtr                      = NULL;
  PayloadSize                     = 0;

  if (Name == NULL || Guid == NULL || Data == NULL || OperType == VariableTypeMax) {
    return EFI_INVALID_PARAMETER;
  }

  //
  //  Try to get auth variable.
  //
  ListEntry = FindVariableList (&mVarListEntry, Name, Guid);

  if (ListEntry != NULL) {
    PriVarInfo = VARIABLE_INFO_PRIVATE_FROM_LINK (ListEntry);
  }

  Status = Auth2VerifyAndUpdate (
             Name,
             Guid,
             Attributes,
             Data,
             DataSize,
             PriVarInfo == NULL ? NULL : &PriVarInfo->TimeStamp,
             OperType,
             &PayloadPtr,
             &PayloadSize,
             &CertTimeStamp
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (OperType == VariableTypeDel) {
    Status = DeleteVariableList (&mVarListEntry, Name, Guid);
    if (Status == EFI_SUCCESS) {
      Status = DeleteCertsFromDb (Name, Guid, Attributes);
    }
  } else {
    Status = CreateVariableList (
               &mVarListEntry,
               Name,
               Guid,
               Attributes,
               CertTimeStamp,
               PayloadSize,
               PayloadPtr,
               OperType == VariableTypeAppend
               );
  }

  return Status;
}
