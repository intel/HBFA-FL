/** @file
  Dhcp6 support functions declaration.

**/

#include "Dhcp6Impl.h"

EFI_STATUS
EFIAPI
InitializeDhcp6SeekStsOption (
  IN        EFI_DHCP6_IA_DESCRIPTOR      *IaDescriptor,
  IN        EFI_DHCP6_HEADER  *Dhcp6Header,
  IN        UINTN PackedOptionDataLen,
  IN        VOID  *TestBuffer,
  IN        UINTN BufferSize
);

EFI_STATUS
Dhcp6SeekStsOption (
  IN     DHCP6_INSTANCE         *Instance,
  IN     EFI_DHCP6_PACKET       *Packet,
  OUT    UINT8                  **Option
);
