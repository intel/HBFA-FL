/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Protocol/Usb2HostController.h>
#include <IndustryStandard/Usb.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

VOID                  *mDeviceDescBuffer;
UINTN                 mDeviceDescBufferSize;
VOID                  *mConfigDescBuffer;
UINTN                 mConfigDescBufferSize;
VOID                  *mStringDescBuffer;
UINTN                 mStringDescBufferSize;

//
// USB Keyboard example
//
USB_DEVICE_DESCRIPTOR mDeviceDesc = {
  0x12,           // Length;
  0x01,           // DescriptorType; - USB_DESC_TYPE_DEVICE
  0x0110,         // BcdUSB;
  0x00,           // DeviceClass;
  0x00,           // DeviceSubClass;
  0x00,           // DeviceProtocol;
  0x08,           // MaxPacketSize0;
  0x03F0,         // IdVendor;
  0x0325,         // IdProduct;
  0x0103,         // BcdDevice;
  0x01,           // StrManufacturer;
  0x02,           // StrProduct;
  0x00,           // StrSerialNumber;
  0x01,           // NumConfigurations;
};

typedef struct {
  USB_CONFIG_DESCRIPTOR     ConfigDesc;
  USB_INTERFACE_DESCRIPTOR  InterfaceDesc;
  EFI_USB_HID_DESCRIPTOR    HidDesc;
  USB_ENDPOINT_DESCRIPTOR   EndpointDesc;
} FULL_USB_CONFIG_DESCRIPTOR;

FULL_USB_CONFIG_DESCRIPTOR  mConfigDesc = {
  {
    0x09,           // Length;
    0x02,           // DescriptorType; - USB_DESC_TYPE_CONFIG
    0x0022,         // TotalLength;
    0x01,           // NumInterfaces;
    0x01,           // ConfigurationValue;
    0x00,           // Configuration;
    0xA0,           // Attributes;
    0x32,           // MaxPower;
  },
  {
    0x09,           // Length;
    0x04,           // DescriptorType; - USB_DESC_TYPE_INTERFACE
    0x00,           // InterfaceNumber;
    0x00,           // AlternateSetting;
    0x01,           // NumEndpoints;
    0x03,           // InterfaceClass;
    0x01,           // InterfaceSubClass;
    0x01,           // InterfaceProtocol;
    0x00,           // Interface;
  },
  {
    0x09,                         // Length;
    0x21,                         // DescriptorType; - USB_DESC_TYPE_HID
    0x0110,                       // BcdHID;
    0x00,                         // CountryCode;
    0x01,                         // NumDescriptors;
    {{0x22, 0x0041}},               // HidClassDesc[0];
  },
  {
    0x07,           // Length;
    0x05,           // DescriptorType; - USB_DESC_TYPE_ENDPOINT
    0x81,           // EndpointAddress;
    0x03,           // Attributes;
    0x0008,         // MaxPacketSize;
    0x0A,           // Interval;
  },
};

EFI_USB_STRING_DESCRIPTOR mStringDesc = {
  0x04,           // Length;
  0x03,           // DescriptorType; - USB_DESC_TYPE_STRING
  {0x0409},       // String[0];
};

typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  CHAR16          String[4];
} FULL_EFI_USB_STRING_DESCRIPTOR;

FULL_EFI_USB_STRING_DESCRIPTOR mStringDesc_2 = {
  0x0A,           // Length;
  0x03,           // DescriptorType; - USB_DESC_TYPE_STRING
  {0x2000, 0x2000, 0x2000, 0x2000}, // String
};

//
// USB Key example
//
USB_DEVICE_DESCRIPTOR mDeviceDesc2 = {
  0x12,           // Length;
  0x01,           // DescriptorType; - USB_DESC_TYPE_DEVICE
  0x0200,         // BcdUSB;
  0x00,           // DeviceClass;
  0x00,           // DeviceSubClass;
  0x00,           // DeviceProtocol;
  0x40,           // MaxPacketSize0;
  0x0781,         // IdVendor;
  0x5151,         // IdProduct;
  0x0010,         // BcdDevice;
  0x01,           // StrManufacturer;
  0x02,           // StrProduct;
  0x03,           // StrSerialNumber;
  0x01,           // NumConfigurations;
};

typedef struct {
  USB_CONFIG_DESCRIPTOR     ConfigDesc;
  USB_INTERFACE_DESCRIPTOR  InterfaceDesc;
  USB_ENDPOINT_DESCRIPTOR   EndpointDesc1;
  USB_ENDPOINT_DESCRIPTOR   EndpointDesc2;
} FULL_USB_CONFIG_DESCRIPTOR2;

FULL_USB_CONFIG_DESCRIPTOR2  mConfigDesc2 = {
  {
    0x09,           // Length;
    0x02,           // DescriptorType; - USB_DESC_TYPE_CONFIG
    0x0020,         // TotalLength;
    0x01,           // NumInterfaces;
    0x01,           // ConfigurationValue;
    0x00,           // Configuration;
    0x80,           // Attributes;
    0x64,           // MaxPower;
  },
  {
    0x09,           // Length;
    0x04,           // DescriptorType; - USB_DESC_TYPE_INTERFACE
    0x00,           // InterfaceNumber;
    0x00,           // AlternateSetting;
    0x02,           // NumEndpoints;
    0x08,           // InterfaceClass;
    0x06,           // InterfaceSubClass;
    0x50,           // InterfaceProtocol;
    0x00,           // Interface;
  },
  {
    0x07,           // Length;
    0x05,           // DescriptorType; - USB_DESC_TYPE_ENDPOINT
    0x81,           // EndpointAddress;
    0x02,           // Attributes;
    0x0200,         // MaxPacketSize;
    0x00,           // Interval;
  },
  {
    0x07,           // Length;
    0x05,           // DescriptorType; - USB_DESC_TYPE_ENDPOINT
    0x01,           // EndpointAddress;
    0x02,           // Attributes;
    0x0200,         // MaxPacketSize;
    0x01,           // Interval;
  },
};

EFI_USB_STRING_DESCRIPTOR mStringDesc2 = {
  0x04,           // Length;
  0x03,           // DescriptorType; - USB_DESC_TYPE_STRING
  {0x0409},       // String[0];
};

typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  CHAR16          String[4];
} FULL_EFI_USB_STRING_DESCRIPTOR2;

FULL_EFI_USB_STRING_DESCRIPTOR2 mStringDesc2_2 = {
  0x0A,           // Length;
  0x03,           // DescriptorType; - USB_DESC_TYPE_STRING
  {0x2000, 0x2000, 0x2000, 0x2000}, // String
};

EFI_STATUS
EFIAPI
Usb2HcStubGetCapability (
  IN  EFI_USB2_HC_PROTOCOL  *This,
  OUT UINT8                 *MaxSpeed,
  OUT UINT8                 *PortNumber,
  OUT UINT8                 *Is64BitCapable
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: GetCapability\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubReset (
  IN EFI_USB2_HC_PROTOCOL   *This,
  IN UINT16                 Attributes
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: Reset\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubGetState (
  IN        EFI_USB2_HC_PROTOCOL    *This,
  OUT       EFI_USB_HC_STATE        *State
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: GetState\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubSetState (
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN EFI_USB_HC_STATE        State
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: SetState\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubControlTransfer (
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST             *Request,
  IN     EFI_USB_DATA_DIRECTION             TransferDirection,
  IN OUT VOID                               *Data       OPTIONAL,
  IN OUT UINTN                              *DataLength OPTIONAL,
  IN     UINTN                              TimeOut,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  OUT    UINT32                             *TransferResult
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: ControlTransfer\n"));

  if ((TransferDirection == EfiUsbDataIn) && 
      (Request->RequestType == 0x80) &&
      (Request->Request == USB_REQ_GET_DESCRIPTOR)) {
    switch (Request->Value >> 8) {
    case USB_DESC_TYPE_DEVICE:
      DEBUG ((DEBUG_INFO, "DEVICE - 0x%x\n", *DataLength));
      if (mDeviceDescBuffer != NULL) {
        if (*DataLength > mDeviceDescBufferSize) {
          *DataLength = mDeviceDescBufferSize;
        }
        CopyMem (Data, mDeviceDescBuffer, *DataLength);
      } else {
        if (*DataLength > sizeof(mDeviceDesc)) {
          *DataLength = sizeof(mDeviceDesc);
        }
        CopyMem (Data, &mDeviceDesc, *DataLength);
      }
      break;
    case USB_DESC_TYPE_CONFIG:
      DEBUG ((DEBUG_INFO, "CONFIG - 0x%x\n", *DataLength));
      if (mConfigDescBuffer != NULL) {
        if (*DataLength > mConfigDescBufferSize) {
          *DataLength = mConfigDescBufferSize;
        }
        CopyMem (Data, mConfigDescBuffer, *DataLength);
      } else {
        if (*DataLength > sizeof(mConfigDesc)) {
          *DataLength = sizeof(mConfigDesc);
        }
        CopyMem (Data, &mConfigDesc, *DataLength);
      }
      break;
    case USB_DESC_TYPE_STRING:
      DEBUG ((DEBUG_INFO, "STRING - 0x%x\n", *DataLength));
      if (mStringDescBuffer != NULL) {
        if (*DataLength > mStringDescBufferSize) {
          *DataLength = mStringDescBufferSize;
        }
        CopyMem (Data, mStringDescBuffer, *DataLength);
      } else {
        if (*DataLength > sizeof(mStringDesc)) {
          *DataLength = sizeof(mStringDesc);
        }
        CopyMem (Data, &mStringDesc, *DataLength);
      }
      break;
    }
  }

  *TransferResult = EFI_USB_NOERROR;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Usb2HcStubBulkTransfer (
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                              *DataLength,
  IN OUT UINT8                              *DataToggle,
  IN     UINTN                              TimeOut,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  OUT    UINT32                             *TransferResult
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: BulkTransfer\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubAsyncInterruptTransfer (
  IN     EFI_USB2_HC_PROTOCOL                                *This,
  IN     UINT8                                               DeviceAddress,
  IN     UINT8                                               EndPointAddress,
  IN     UINT8                                               DeviceSpeed,
  IN     UINTN                                               MaxiumPacketLength,
  IN     BOOLEAN                                             IsNewTransfer,
  IN OUT UINT8                                               *DataToggle,
  IN     UINTN                                               PollingInterval  OPTIONAL,
  IN     UINTN                                               DataLength       OPTIONAL,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR                  *Translator      OPTIONAL,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK                     CallBackFunction OPTIONAL,
  IN     VOID                                                *Context         OPTIONAL
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: AsyncInterruptTransfer\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubSyncInterruptTransfer (
  IN     EFI_USB2_HC_PROTOCOL                        *This,
  IN     UINT8                                       DeviceAddress,
  IN     UINT8                                       EndPointAddress,
  IN     UINT8                                       DeviceSpeed,
  IN     UINTN                                       MaximumPacketLength,
  IN OUT VOID                                        *Data,
  IN OUT UINTN                                       *DataLength,
  IN OUT UINT8                                       *DataToggle,
  IN     UINTN                                       TimeOut,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR          *Translator,
  OUT    UINT32                                      *TransferResult
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: SyncInterruptTransfer\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubIsochronousTransfer (
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
  IN     UINTN                              DataLength,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  OUT    UINT32                             *TransferResult
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubAsyncIsochronousTransfer (
  IN     EFI_USB2_HC_PROTOCOL               *This,
  IN     UINT8                              DeviceAddress,
  IN     UINT8                              EndPointAddress,
  IN     UINT8                              DeviceSpeed,
  IN     UINTN                              MaximumPacketLength,
  IN     UINT8                              DataBuffersNumber,
  IN OUT VOID                               *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
  IN     UINTN                              DataLength,
  IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK    IsochronousCallBack,
  IN     VOID                               *Context OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubGetRootHubPortStatus (
  IN        EFI_USB2_HC_PROTOCOL    *This,
  IN        UINT8                   PortNumber,
  OUT       EFI_USB_PORT_STATUS     *PortStatus
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: GetRootHubPortStatus\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubSetRootHubPortFeature (
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN UINT8                   PortNumber,
  IN EFI_USB_PORT_FEATURE    PortFeature
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: SetRootHubPortFeature\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Usb2HcStubClearRootHubPortFeature (
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN UINT8                   PortNumber,
  IN EFI_USB_PORT_FEATURE    PortFeature
  )
{
  DEBUG ((DEBUG_INFO, "USB_STUB: ClearRootHubPortFeature\n"));
  return EFI_UNSUPPORTED;
}

EFI_USB2_HC_PROTOCOL       mUsb2HcStub = {
  Usb2HcStubGetCapability,
  Usb2HcStubReset,
  Usb2HcStubGetState,
  Usb2HcStubSetState,
  Usb2HcStubControlTransfer,
  Usb2HcStubBulkTransfer,
  Usb2HcStubAsyncInterruptTransfer,
  Usb2HcStubSyncInterruptTransfer,
  Usb2HcStubIsochronousTransfer,
  Usb2HcStubAsyncIsochronousTransfer,
  Usb2HcStubGetRootHubPortStatus,
  Usb2HcStubSetRootHubPortFeature,
  Usb2HcStubClearRootHubPortFeature,
  0x2,
  0x0
};

EFI_STATUS
EFIAPI
Usb2HcStubInitialize (
  IN  VOID                  *DeviceDescBuffer,
  IN  UINTN                 DeviceDescBufferSize,
  IN  VOID                  *ConfigDescBuffer,
  IN  UINTN                 ConfigDescBufferSize,
  IN  VOID                  *StringDescBuffer,
  IN  UINTN                 StringDescBufferSize,
  OUT EFI_USB2_HC_PROTOCOL  **Usb2Hc
  )
{
  mDeviceDescBuffer = DeviceDescBuffer;
  mDeviceDescBufferSize = DeviceDescBufferSize;
  mConfigDescBuffer = ConfigDescBuffer;
  mConfigDescBufferSize = ConfigDescBufferSize;
  mStringDescBuffer = StringDescBuffer;
  mStringDescBufferSize = StringDescBufferSize;
  *Usb2Hc = &mUsb2HcStub;
  return EFI_SUCCESS;
}
