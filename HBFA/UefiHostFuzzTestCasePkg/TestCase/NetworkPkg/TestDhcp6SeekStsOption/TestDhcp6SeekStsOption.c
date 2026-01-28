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
  + sizeof(EFI_DHCP6_HEADER) // DHCP6 Header
  + DHCPV6_PACKED_OPTION_DATA // Max Length of DHCPv6 packed option data
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
  if (!Instance) {
    printf("Allocation failed!\n");
    return;
  }
  Packet = (EFI_DHCP6_PACKET *)AllocateZeroPool (sizeof (EFI_DHCP6_PACKET) + DHCPV6_PACKED_OPTION_DATA);
  if (!Packet) {
    printf("Allocation failed!\n");
    FreePool(Instance);
    return;
  }
  Option = (UINT8 **)AllocateZeroPool (sizeof (UINT8 *));
  if (!Option) {
    printf("Allocation failed!\n");
    FreePool(Packet);
    FreePool(Instance);
    return;
  }
  Instance->Config = (EFI_DHCP6_CONFIG_DATA *)AllocateZeroPool (sizeof (EFI_DHCP6_CONFIG_DATA));
  if (!Instance->Config) {
    printf("Allocation failed!\n");
    FreePool(Option);
    FreePool(Packet);
    FreePool(Instance);
    return;
  }

  // Packet and IaDescriptor are fed from TestBuffer
  // IaDescriptor is 8 bytes (sizeof(EFI_DHCP6_IA_DESCRIPTOR))
  // Dhcp6 Header + Options is sizeof(EFI_DHCP6_HEADER) + DHCPV6_PACKED_OPTION_DATA bytes
  // Total minimum TestBufferSize = 8 + sizeof(EFI_DHCP6_HEADER) + 24 bytes

  if (TestBufferSize < (sizeof(EFI_DHCP6_IA_DESCRIPTOR) + sizeof(EFI_DHCP6_HEADER) + DHCPV6_PACKED_OPTION_DATA)) {
    printf("TestBufferSize is too small!\n");
    FreePool(Instance->Config);
    FreePool(Option);
    FreePool(Packet);
    FreePool(Instance);
    return;
  }

  // 1. Initialize Instance->Config->IaDescriptor, Packet options buffer from TestBuffer
  EFI_STATUS Status = InitializeDhcp6SeekStsOption(&Instance->Config->IaDescriptor, (EFI_DHCP6_HEADER *)&Packet->Dhcp6, DHCPV6_PACKED_OPTION_DATA, TestBuffer, TestBufferSize);

  // Initialize packed option data buffer length

  // Size of the EFI_DHCP6_PACKET buffer.
  Packet->Size = sizeof (EFI_DHCP6_PACKET) + DHCPV6_PACKED_OPTION_DATA;

  // Length of the EFI_DHCP6_PACKET from the first byte of the Header field 
  // to the last byte of the Option[] field.
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

  // 3. Clean up allocated resources
  FreePool(Instance->Config);
  FreePool(Option);
  FreePool(Packet);
  FreePool(Instance);

  return;
}
