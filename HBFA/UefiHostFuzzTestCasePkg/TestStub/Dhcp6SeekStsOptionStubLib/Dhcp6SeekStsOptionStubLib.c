/** @file


**/
#include <Library/Dhcp6SeekStsOptionStubLib.h>


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

  //
  // Seek StatusCode option directly in DHCP message body. That is, search in
  // non-encapsulated option fields.
  //
  *Option = Dhcp6SeekOption (
              Packet->Dhcp6.Option,
              Packet->Length - 4,
              Dhcp6OptStatusCode
              );

  if (*Option != NULL) {
    StsCode = NTOHS (ReadUnaligned16 ((UINT16 *)(*Option + 4)));
    if (StsCode != Dhcp6StsSuccess) {
      return EFI_DEVICE_ERROR;
    }
  }

  //
  // Seek in encapsulated options, IA_NA and IA_TA.
  //
  *Option = Dhcp6SeekIaOption (
              Packet->Dhcp6.Option,
              Packet->Length - sizeof (EFI_DHCP6_HEADER),
              &Instance->Config->IaDescriptor
              );
  if (*Option == NULL) {
    return EFI_SUCCESS;
  }

  //
  // The format of the IA_NA option is:
  //
  //     0                   1                   2                   3
  //     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |          OPTION_IA_NA         |          option-len           |
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |                        IAID (4 octets)                        |
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |                              T1                               |
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |                              T2                               |
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |                                                               |
  //    .                         IA_NA-options                         .
  //    .                                                               .
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //
  // The format of the IA_TA option is:
  //
  //     0                   1                   2                   3
  //     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |         OPTION_IA_TA          |          option-len           |
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |                        IAID (4 octets)                        |
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //    |                                                               |
  //    .                         IA_TA-options                         .
  //    .                                                               .
  //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //

  //
  // sizeof (option-code + option-len + IaId)           = 8
  // sizeof (option-code + option-len + IaId + T1)      = 12
  // sizeof (option-code + option-len + IaId + T1 + T2) = 16
  //
  // The inner options still start with 2 bytes option-code and 2 bytes option-len.
  //
  if (Instance->Config->IaDescriptor.Type == Dhcp6OptIana) {
    IaInnerOpt = *Option + 16;
    IaInnerLen = (UINT16)(NTOHS (ReadUnaligned16 ((UINT16 *)(*Option + 2))) - 12);
  } else {
    IaInnerOpt = *Option + 8;
    IaInnerLen = (UINT16)(NTOHS (ReadUnaligned16 ((UINT16 *)(*Option + 2))) - 4);
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
    StsCode = NTOHS (ReadUnaligned16 ((UINT16 *)(*Option + 4)));
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
  IN        EFI_DHCP6_PACKET  *Packet,
  IN        UINTN PackedOptionDataLen,
  IN        VOID  *TestBuffer,
  IN        UINTN BufferSize
)
{
  if (BufferSize < sizeof (EFI_DHCP6_IA_DESCRIPTOR) + sizeof (EFI_DHCP6_PACKET) + PackedOptionDataLen) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem ((void *)IaDescriptor, (void *) TestBuffer, sizeof (EFI_DHCP6_IA_DESCRIPTOR));

  CopyMem ((void *)Packet, (void *) ((UINT8 *)TestBuffer + sizeof (EFI_DHCP6_IA_DESCRIPTOR)), sizeof (EFI_DHCP6_PACKET) + PackedOptionDataLen);

  return EFI_SUCCESS;
}