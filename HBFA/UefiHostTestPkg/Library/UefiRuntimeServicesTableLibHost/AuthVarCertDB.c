/**
  Manage trusted certificate database.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsVariable.h"
#include "AuthVarCertDB.h"

extern LIST_ENTRY mVarListEntry;
CONST UINT32      gEfiCertDBAttr  = (UINT32)(EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |
                                    EFI_VARIABLE_NON_VOLATILE |
                                    EFI_VARIABLE_RUNTIME_ACCESS |
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS);

/*
  Function to get "certdb" variable according to Attributes parameter.

  @param[in]    Attributes              Attributes of authenticated variable.
  @param[out]   CertDBPtr               Point to "certdb" variable buffer.

  @retval       EFI_SUCCESS             Find matching certs and output parameters.
  @retval       EFI_INVALID_PARAMETER   CertDBPtr is NULL or Attributes is invalid.
  @retval       EFI_SECURITY_VIOLATION  CertDB structure has been corrupted.
  @retval       EFI_NOT_FOUND           Fail to find matching certs.
*/
EFI_STATUS
GetCertDB (
  IN    UINT32              Attributes,
  OUT   VOID                **CertDBPtr
  )
{
  CHAR16                    *CertDBName;
  VARIABLE_INFO_PRIVATE     *VariableInfo;

  if (CertDBPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
    CertDBName = EFI_CERT_DB;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  VariableInfo = FindVariableInfoPtr (CertDBName, &gEfiCertDbGuid);
  if (VariableInfo == NULL || VariableInfo->Size == 0) {
    return EFI_NOT_FOUND;
  }

  if (*(UINT32 *)(VariableInfo->Buffer) != VariableInfo->Size) {
    return EFI_SECURITY_VIOLATION;
  }

  *CertDBPtr = VariableInfo;
  return EFI_SUCCESS;
}

/**
  Find matching signer's certificates for common authenticated variable
  by corresponding VariableName and VendorGuid from "certdb".

  @param[in]    VariableName            Name of authenticated Variable.
  @param[in]    VendorGuid              Vendor GUID of authenticated Variable.
  @param[in]    Data                    Pointer to variable "certdb".
  @param[in]    DataSize                Size of variable "certdb".
  @param[out]   CertNodeOffset          Offset of matching AUTH_CERT_DB_DATA, from starting of Data.
  @param[out]   CertNodeSize            Length of AUTH_CERT_DB_DATA in bytes.
  @param[out]   CertDataPtr             Pointer to the matching CertData.
  @param[out]   CertDataSize            Lenght of CertData in bytes.

  @retval       EFI_INVALID_PARAMETER   Any input parameter is invalid.
  @retval       EFI_NOT_FOUND           Fail to find matching certs.
  @retval       EFI_SUCCESS             Find matching certs and output parameters.
**/
EFI_STATUS
FindCertsFromDb (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  IN      UINT32            Attributes,
  OUT     UINT32            *CertNodeOffset   OPTIONAL,
  OUT     UINT32            *CertNodeSize     OPTIONAL,
  OUT     UINT8             **CertDataPtr     OPTIONAL,
  OUT     UINT32            *CertDataSize     OPTIONAL
  )
{
  EFI_STATUS              Status;
  VARIABLE_INFO_PRIVATE   *VariableInfo;
  AUTH_CERT_DB_DATA       *Ptr;
  UINT32                  CertDBListSize;
  UINT32                  Offset;
  UINT32                  NodeSize;
  UINT32                  NameSize;
  UINT32                  DataSize;

  Status                  = EFI_SUCCESS;

  if ((VariableName == NULL) || (VendorGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetCertDB (Attributes, (VOID **)&VariableInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (VariableInfo->Buffer == NULL || VariableInfo->Size == 0) {
    return EFI_NOT_FOUND;
  }

  if (VariableInfo->Size != *((UINT32 *)VariableInfo->Buffer)) {
    return EFI_NOT_FOUND;
  }

  CertDBListSize = (UINT32)VariableInfo->Size;
  Offset = sizeof (UINT32);

  //
  //  Traverse CertDB and try to matching certs.
  //
  while (Offset < CertDBListSize) {
    Ptr = (AUTH_CERT_DB_DATA *) ((UINT8 *)VariableInfo->Buffer + Offset);

    NodeSize = ReadUnaligned32 (&Ptr->CertNodeSize);
    NameSize = ReadUnaligned32 (&Ptr->NameSize);
    DataSize = ReadUnaligned32 (&Ptr->CertDataSize);

    if (NodeSize != sizeof (EFI_GUID) + sizeof (UINT32) * 3 + DataSize + NameSize * sizeof (CHAR16)) {
        return EFI_NOT_FOUND;
    }

    Offset = Offset + sizeof (EFI_GUID) + sizeof (UINT32) * 3;
    if ((CompareGuid (&Ptr->VendorGuid, VendorGuid) == 0) &&
        (NameSize == StrLen (VariableName)) &&
        (CompareMem (VariableName, VariableInfo->Buffer + Offset, NameSize * sizeof (CHAR16)) == 0)
       ) {
      if (CertNodeOffset != NULL) {
        *CertNodeOffset = (UINT32) ((UINT8 *)Ptr - VariableInfo->Buffer);
      }
      if (CertNodeSize != NULL) {
        *CertNodeSize = NodeSize;
      }
      if (CertDataPtr != NULL) {
        *CertDataPtr = (UINT8 *)Ptr + sizeof (EFI_GUID) + sizeof (UINT32) * 3 + NameSize * sizeof (CHAR16);
      }
      if (CertDataSize != NULL) {
        *CertDataSize = DataSize;
      }
      return EFI_SUCCESS;
    }
    Offset = Offset + NameSize * sizeof (CHAR16) + DataSize;
  }

  return EFI_NOT_FOUND;
}

/**
  Delete matching signer's certificates when deleting common authenticated
  variable by corresponding VariableName and VendorGuid from "certdb".

  @param[in]    VariableName            Name of authenticated Variable.
  @param[in]    VendorGuid              Vendor GUID of authenticated Variable.
  @param[in]    Attributes              Attributes of authenticated variable.

  @retval       EFI_INVALID_PARAMETER   Any input parameter is invalid.
  @retval       EFI_NOT_FOUND           Fail to find "certdb" or matching certs.
  @retval       EFI_OUT_OF_RESOURCES    The operation is failed due to lack of resources.
  @retval       EFI_SUCCESS             The operation is completed successfully.
**/
EFI_STATUS
DeleteCertsFromDb (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  IN      UINT32            Attributes
  )
{
  EFI_STATUS              Status;
  VARIABLE_INFO_PRIVATE   *VariableInfo;
  UINT32                  CertNodeOffset;
  UINT32                  CertNodeSize;
  UINT8                   *NewBuffer;
  UINTN                   NewBufferSize;

  Status                  = EFI_SUCCESS;
  VariableInfo            = NULL;
  NewBuffer               = NULL;
  CertNodeOffset          = 0;
  CertNodeSize            = 0;

  if (VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetCertDB (Attributes, (VOID **)&VariableInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FindCertsFromDb (
             VariableName,
             VendorGuid,
             Attributes,
             &CertNodeOffset,
             &CertNodeSize,
             NULL,
             NULL
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NewBufferSize = VariableInfo->Size - CertNodeSize;

  //
  //  If the NewBufferSize == 4, it means no AUTH_CERT_DB_DATA in "certDB".
  //
  if (NewBufferSize == 4) {
    return DeleteVariableList (&mVarListEntry, VariableInfo->Name, &gEfiCertDbGuid);
  }

  NewBuffer = (UINT8 *)malloc (NewBufferSize);
  if (NewBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  //  Copy first-part from old buffer to new buffer.
  //
  CopyMem (NewBuffer, VariableInfo->Buffer, CertNodeOffset);
  //
  //  Copy second-part to new buffer.
  //
  CopyMem (
    NewBuffer + CertNodeOffset,
    VariableInfo->Buffer + CertNodeOffset + CertNodeSize,
    VariableInfo->Size - CertNodeOffset - CertNodeSize
    );

  Status = CreateVariableList (
             &mVarListEntry,
             VariableInfo->Name,
             &gEfiCertDbGuid,
             gEfiCertDBAttr,
             NULL,
             NewBufferSize,
             NewBuffer,
             FALSE
             );

  FREE_NON_NULL_PTR (NewBuffer);
  return Status;
}

/**
  Insert signer's certificates for common authenticated variable
  with VariableName and VendorGuid in AUTH_CERT_DB_DATA to "certdb".

  @param[in]  VariableName      Name of authenticated Variable.
  @param[in]  VendorGuid        Vendor GUID of authenticated Variable.
  @param[in]  Attributes        Attributes of authenticated variable.
  @param[in]  SignerCert        Signer certificate data.
  @param[in]  SignerCertSize    Length of signer certificate.
  @param[in]  TopLevelCert      Top-level certificate data.
  @param[in]  TopLevelCertSize  Length of top-level certificate.


  @retval  EFI_INVALID_PARAMETER Any input parameter is invalid.
  @retval  EFI_ACCESS_DENIED     An AUTH_CERT_DB_DATA entry with same VariableName
                                 and VendorGuid already exists.
  @retval  EFI_OUT_OF_RESOURCES  The operation is failed due to lack of resources.
  @retval  EFI_SUCCESS           Insert an AUTH_CERT_DB_DATA entry to "certdb" or "certdbv"
**/
EFI_STATUS
InsertCertsToDb (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  IN      UINT32            Attributes,
  IN      UINT8             *SignerCert,
  IN      UINTN             SignerCertSize,
  IN      UINT8             *TopLevelCert,
  IN      UINTN             TopLevelCertSize
  )
{
  EFI_STATUS                Status;
  UINT8                     Sha256Digest[SHA256_DIGEST_SIZE];
  UINT8                     *NewBuffer;
  UINTN                     NewBufferSize;
  UINT32                    NameSize;
  UINT32                    NodeSize;
  UINT32                    DataSize;
  UINT32                    Offset;
  CHAR16                    *CertDBName;

  Status                    = EFI_SUCCESS;
  NewBuffer                 = NULL;

  if (VariableName == NULL || VendorGuid == NULL || SignerCert == NULL || TopLevelCert == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
    CertDBName = EFI_CERT_DB;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  Status = FindCertsFromDb (VariableName, VendorGuid, Attributes, NULL, NULL, NULL, NULL);
  if (Status == EFI_SUCCESS) {
    return EFI_SECURITY_VIOLATION;
  } else if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
    return Status;
  }

#if 0
  Status = CalculatePrivAuthVarSignChainSHA256Digest (
             SignerCert,
             SignerCertSize,
             TopLevelCert,
             TopLevelCertSize,
             Sha256Digest
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif

  ///
  /// AUTH_CERT_DB_DATA format:
  ///
  /// EFI_GUID    VendorGuid;
  /// UINT32      CertNodeSize;
  /// UINT32      NameSize;
  /// UINT32      CertDataSize;
  /// CHAR16      VariableName[NameSize];
  /// UINT8       CertData[CertDataSize];
  ///

  NameSize = (UINT32)(StrLen (VariableName));
  DataSize = SHA256_DIGEST_SIZE;
  NodeSize = sizeof (AUTH_CERT_DB_DATA) + NameSize * sizeof (CHAR16) + DataSize;

  NewBufferSize = sizeof (UINT32) + NodeSize;

  NewBuffer = (UINT8 *)malloc (NewBufferSize);
  if (NewBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Offset = 0;
  CopyMem (NewBuffer + Offset, &NewBufferSize, sizeof (UINT32));
  Offset = Offset + sizeof (UINT32);

  CopyMem (NewBuffer + Offset, VendorGuid, sizeof (EFI_GUID));
  Offset = Offset + sizeof (EFI_GUID);

  CopyMem (NewBuffer + Offset, &NodeSize, sizeof (UINT32));
  Offset = Offset + sizeof (UINT32);

  CopyMem (NewBuffer + Offset, &NameSize, sizeof (UINT32));
  Offset = Offset + sizeof (UINT32);

  CopyMem (NewBuffer + Offset, &DataSize, sizeof (UINT32));
  Offset = Offset + sizeof (UINT32);

  CopyMem (NewBuffer + Offset, VariableName, NameSize * sizeof (CHAR16));
  Offset = Offset + NameSize * sizeof (CHAR16);

  CopyMem (NewBuffer + Offset, Sha256Digest, DataSize);

  Status = CreateVariableList (
             &mVarListEntry,
             CertDBName,
             &gEfiCertDbGuid,
             gEfiCertDBAttr,
             NULL,
             NewBufferSize,
             NewBuffer,
             FALSE
             );

  FREE_NON_NULL_PTR (NewBuffer);
  return Status;
}
