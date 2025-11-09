/** @file


**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/Dhcp6SeekStsOptionStubLib.h>

#define DHCPV6_PACKED_OPTION_DATA 24


VOID
FixBuffer (
  UINT8                   *TestBuffer
  )
{
}

UINTN
EFIAPI
GetMaxBufferSize (
  VOID
  )
{
  return (
    sizeof(EFI_DHCP6_IA_DESCRIPTOR) // IaDescriptor
  + sizeof(EFI_DHCP6_PACKET) // DHCP6 Packet
  + DHCPV6_PACKED_OPTION_DATA // Max Length of DHCPv6 packed option data is sizeof(UINT32)
  // We assume the max length of option data is 24 bytes here.
  );
}

VOID
EFIAPI
RunTestHarness(
  IN VOID  *TestBuffer,
  IN UINTN TestBufferSize
  )
{
  DHCP6_INSTANCE    *Instance;
  EFI_DHCP6_PACKET  *Packet;
  UINT8             **Option;

  Instance = (DHCP6_INSTANCE *)AllocateZeroPool (sizeof (DHCP6_INSTANCE));
  Packet = (EFI_DHCP6_PACKET *)AllocateZeroPool (sizeof (EFI_DHCP6_PACKET) + DHCPV6_PACKED_OPTION_DATA);
  Option = (UINT8 **)AllocateZeroPool (sizeof (UINT8 *));

  Instance->Config = (EFI_DHCP6_CONFIG_DATA *)AllocateZeroPool (sizeof (EFI_DHCP6_CONFIG_DATA));

  // 1. Initialize Instance->Config->IaDescriptor, Packet options buffer from TestBuffer
  EFI_STATUS Status = InitializeDhcp6SeekStsOption(&Instance->Config->IaDescriptor, Packet, DHCPV6_PACKED_OPTION_DATA, TestBuffer, TestBufferSize);

  // Intialize packed option data buffer length
  Packet->Size = sizeof (EFI_DHCP6_PACKET) + DHCPV6_PACKED_OPTION_DATA;
  Packet->Length = sizeof(EFI_DHCP6_HEADER) + DHCPV6_PACKED_OPTION_DATA;


  if (EFI_ERROR (Status)) {
    FreePool(Instance->Config);
    FreePool(Option);
    FreePool(Packet);
    FreePool(Instance);
    return;
  }

  // 2. Call the function under test
  Dhcp6SeekStsOption( Instance, Packet, Option);

  return;
}
