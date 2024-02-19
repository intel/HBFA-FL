/**
  Header file for trusted certificate database Management.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _AUTH_CERTDB_H
#define _AUTH_CERTDB_H

#include "VariableCommon.h"

///
///  "certdb" variable stores the signer's certificates for non PK/KEK/DB/DBX
///  variables with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS|EFI_VARIABLE_NON_VOLATILE set.
///
/// GUID: gEfiCertDbGuid
///
/// We need maintain atomicity.
///
/// Format:
/// +----------------------------+
/// | UINT32                     | <-- CertDbListSize, including this UINT32
/// +----------------------------+
/// | AUTH_CERT_DB_DATA          | <-- First CERT
/// +----------------------------+
/// | ........                   |
/// +----------------------------+
/// | AUTH_CERT_DB_DATA          | <-- Last CERT
/// +----------------------------+
///

#define EFI_CERT_DB                 L"certdb"

#pragma pack(1)
typedef struct {
  EFI_GUID    VendorGuid;
  UINT32      CertNodeSize;
  UINT32      NameSize;
  UINT32      CertDataSize;
  /// CHAR16  VariableName[NameSize];
  /// UINT8   CertData[CertDataSize];
} AUTH_CERT_DB_DATA;
#pragma pack()

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
  );

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
  );

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
  );

#endif
