/** @file


**/
#include <stdio.h>
#include <Library/Dhcp6SeekStsOptionStubLib.h>

/**
  Seek the address of the first byte of the Ia option header.

  @param[in]  Buf           The pointer to the buffer.
  @param[in]  SeekLen       The length to seek.
  @param[in]  IaDesc        The pointer to the Ia descriptor.

  @retval     NULL          If it failed to seek the Ia option.
  @retval     others        The position to the Ia option.

**/
UINT8 *
Dhcp6SeekIaOption (
  IN UINT8                    *Buf,
  IN UINT32                   SeekLen,
  IN EFI_DHCP6_IA_DESCRIPTOR  *IaDesc
  )
{
  UINT8   *Cursor;
  UINT8   *Option;
  UINT16  DataLen;
  UINT16  OpCode;
  UINT32  IaId;

  //
  // The format of IA_NA and IA_TA option refers to Dhcp6AppendIaOption().
  //
  Option = NULL;
  Cursor = Buf;

  while (Cursor < Buf + SeekLen) {
    OpCode = ReadUnaligned16 ((UINT16 *)Cursor);
    IaId   = ReadUnaligned32 ((UINT32 *)(Cursor + 4));
    if ((OpCode == HTONS (IaDesc->Type)) && (IaId == HTONL (IaDesc->IaId))) {
      Option = Cursor;
      break;
    }

    DataLen = NTOHS (ReadUnaligned16 ((UINT16 *)(Cursor + 2)));
    Cursor += (DataLen + 4);
  }

  return Option;
}

/**
  Seeks the Inner Options from a DHCP6 Option

  @param[in]  IaType          The type of the IA option.
  @param[in]  Option          The pointer to the DHCP6 Option.
  @param[in]  OptionLen       The length of the DHCP6 Option.
  @param[out] IaInnerOpt      The pointer to the IA inner option.
  @param[out] IaInnerLen      The length of the IA inner option.

  @retval EFI_SUCCESS         Seek the inner option successfully.
  @retval EFI_DEVICE_ERROR    The OptionLen is invalid. On Error,
                              the pointers are not modified
**/
EFI_STATUS
Dhcp6SeekInnerOptionSafe (
  IN  UINT16  IaType,
  IN  UINT8   *Option,
  IN  UINT32  OptionLen,
  OUT UINT8   **IaInnerOpt,
  OUT UINT16  *IaInnerLen
  )
{
  UINT16  IaInnerLenTmp;
  UINT8   *IaInnerOptTmp;

  if (Option == NULL) {
    ASSERT (Option != NULL);
    return EFI_DEVICE_ERROR;
  }

  if (IaInnerOpt == NULL) {
    ASSERT (IaInnerOpt != NULL);
    return EFI_DEVICE_ERROR;
  }

  if (IaInnerLen == NULL) {
    ASSERT (IaInnerLen != NULL);
    return EFI_DEVICE_ERROR;
  }

  if (IaType == Dhcp6OptIana) {
    //
    // Verify we have a fully formed IA_NA
    //
    if (OptionLen < DHCP6_MIN_SIZE_OF_IA_NA) {
      return EFI_DEVICE_ERROR;
    }

    //
    // Get the IA Inner Option and Length
    //
    IaInnerOptTmp = DHCP6_OFFSET_OF_IA_NA_INNER_OPT (Option);

    //
    // Verify the IaInnerLen is valid.
    //
    IaInnerLenTmp = (UINT16)NTOHS (ReadUnaligned16 ((UINT16 *)DHCP6_OFFSET_OF_OPT_LEN (Option)));
    if (IaInnerLenTmp < DHCP6_SIZE_OF_COMBINED_IAID_T1_T2) {
      return EFI_DEVICE_ERROR;
    }

    IaInnerLenTmp -= DHCP6_SIZE_OF_COMBINED_IAID_T1_T2;
  } else if (IaType == Dhcp6OptIata) {
    //
    // Verify the OptionLen is valid.
    //
    if (OptionLen < DHCP6_MIN_SIZE_OF_IA_TA) {
      return EFI_DEVICE_ERROR;
    }

    IaInnerOptTmp = DHCP6_OFFSET_OF_IA_TA_INNER_OPT (Option);

    //
    // Verify the IaInnerLen is valid.
    //
    IaInnerLenTmp = (UINT16)NTOHS (ReadUnaligned16 ((UINT16 *)(DHCP6_OFFSET_OF_OPT_LEN (Option))));
    if (IaInnerLenTmp < DHCP6_SIZE_OF_IAID) {
      return EFI_DEVICE_ERROR;
    }

    IaInnerLenTmp -= DHCP6_SIZE_OF_IAID;
  } else {
    return EFI_DEVICE_ERROR;
  }

  *IaInnerOpt = IaInnerOptTmp;
  *IaInnerLen = IaInnerLenTmp;

  return EFI_SUCCESS;
}

/**
  Seek the address of the first byte of the option header.

  @param[in]  Buf           The pointer to the buffer.
  @param[in]  SeekLen       The length to seek.
  @param[in]  OptType       The option type.

  @retval     NULL          If it failed to seek the option.
  @retval     others        The position to the option.

**/
UINT8 *
Dhcp6SeekOption (
  IN UINT8   *Buf,
  IN UINT32  SeekLen,
  IN UINT16  OptType
  )
{
  UINT8   *Cursor;
  UINT8   *Option;
  UINT16  DataLen;
  UINT16  OpCode;

  Option = NULL;
  Cursor = Buf;

  //
  // The format of Dhcp6 option refers to Dhcp6AppendOption().
  //
  while (Cursor < Buf + SeekLen) {
    OpCode = ReadUnaligned16 ((UINT16 *)Cursor);
    if (OpCode == HTONS (OptType)) {
      Option = Cursor;
      break;
    }

    DataLen = NTOHS (ReadUnaligned16 ((UINT16 *)(Cursor + 2)));
    Cursor += (DataLen + 4);
  }

  return Option;
}

/**
  Seek StatusCode Option in package. A Status Code option may appear in the
  options field of a DHCP message and/or in the options field of another option.
  See details in section 22.13, RFC3315.

  @param[in]       Instance        The pointer to the Dhcp6 instance.
  @param[in]       Packet          The pointer to reply messages.
  @param[out]      Option          The pointer to status code option.

  @retval EFI_SUCCESS              Seek status code option successfully.
  @retval EFI_DEVICE_ERROR         An unexpected error.

**/
EFI_STATUS
Dhcp6SeekStsOption (
  IN     DHCP6_INSTANCE    *Instance,
  IN     EFI_DHCP6_PACKET  *Packet,
  OUT    UINT8             **Option
  )
{
  UINT8   *IaInnerOpt;
  UINT16  IaInnerLen;
  UINT16  StsCode;
  UINT32  OptionLen;

  // OptionLen is the length of the Options excluding the DHCP header.
  // Length of the EFI_DHCP6_PACKET from the first byte of the Header field to the last
  // byte of the Option[] field.
  OptionLen = Packet->Length - sizeof (Packet->Dhcp6.Header);

  //
  // Seek StatusCode option directly in DHCP message body. That is, search in
  // non-encapsulated option fields.
  //
  *Option = Dhcp6SeekOption (
              Packet->Dhcp6.Option,
              OptionLen,
              Dhcp6OptStatusCode
              );

  if (*Option != NULL) {
    StsCode = NTOHS (ReadUnaligned16 ((UINT16 *)(DHCP6_OFFSET_OF_STATUS_CODE (*Option))));
    if (StsCode != Dhcp6StsSuccess) {
      return EFI_DEVICE_ERROR;
    }
  }

  //
  // Seek in encapsulated options, IA_NA and IA_TA.
  //
  *Option = Dhcp6SeekIaOption (
              Packet->Dhcp6.Option,
              OptionLen,
              &Instance->Config->IaDescriptor
              );
  if (*Option == NULL) {
    return EFI_SUCCESS;
  }

  //
  // Calculate the distance from Packet->Dhcp6.Option to the IA option.
  //
  // Packet->Size and Packet->Length are both UINT32 type, and Packet->Size is
  // the size of the whole packet, including the DHCP header, and Packet->Length
  // is the length of the DHCP message body, excluding the DHCP header.
  //
  // (*Option - Packet->Dhcp6.Option) is the number of bytes from the start of
  // DHCP6 option area to the start of the IA option.
  //
  // Dhcp6SeekInnerOptionSafe() is searching starting from the start of the
  // IA option to the end of the DHCP6 option area, thus subtract the space
  // up until this option
  //
  OptionLen = OptionLen - (UINT32)(*Option - Packet->Dhcp6.Option);

  //
  // Seek the inner option
  //
  if (EFI_ERROR (
        Dhcp6SeekInnerOptionSafe (
          Instance->Config->IaDescriptor.Type,
          *Option,
          OptionLen,
          &IaInnerOpt,
          &IaInnerLen
          )
        ))
  {
    return EFI_DEVICE_ERROR;
  }

  //
  // The format of the Status Code option is:
  //
  //   0                   1                   2                   3
  //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |       OPTION_STATUS_CODE      |         option-len            |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |          status-code          |                               |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
  //  .                                                               .
  //  .                        status-message                         .
  //  .                                                               .
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //

  //
  // sizeof (option-code + option-len) = 4
  //
  *Option = Dhcp6SeekOption (IaInnerOpt, IaInnerLen, Dhcp6OptStatusCode);
  if (*Option != NULL) {
    StsCode = NTOHS (ReadUnaligned16 ((UINT16 *)((DHCP6_OFFSET_OF_STATUS_CODE (*Option)))));
    if (StsCode != Dhcp6StsSuccess) {
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}


/**


**/
EFI_STATUS
EFIAPI
InitializeDhcp6SeekStsOption (
  IN        EFI_DHCP6_IA_DESCRIPTOR      *IaDescriptor,
  IN        EFI_DHCP6_HEADER  *Dhcp6Header,
  IN        UINTN PackedOptionDataLen,
  IN        VOID  *TestBuffer,
  IN        UINTN BufferSize
)
{
  if (BufferSize < sizeof (EFI_DHCP6_IA_DESCRIPTOR) + sizeof (EFI_DHCP6_HEADER) + PackedOptionDataLen) {
    printf("Buffer too small!\n");
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem ((void *)IaDescriptor, (void *) TestBuffer, sizeof (EFI_DHCP6_IA_DESCRIPTOR));

  CopyMem ((void *)Dhcp6Header, (void *) ((UINT8 *)TestBuffer + sizeof (EFI_DHCP6_IA_DESCRIPTOR)), sizeof (EFI_DHCP6_HEADER) + PackedOptionDataLen);

  return EFI_SUCCESS;
}