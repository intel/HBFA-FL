/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Usb2HostController.h>

EFI_EVENT                   mUsbHookEvent;
VOID                        *mUsbHookRegistration;

#define USB2_HC_HOOK_PRIVATE_SIGNATURE  SIGNATURE_32 ('U', 'H', 'H', 'K')

typedef struct {
  UINTN                      Signature;
  EFI_USB2_HC_PROTOCOL       OrgUsb2Hc;
  EFI_USB2_HC_PROTOCOL       *Usb2Hc;
  LIST_ENTRY                 Link;
} USB2_HC_HOOK_PRIVATE;

LIST_ENTRY  mUsb2HcHookQueue = INITIALIZE_LIST_HEAD_VARIABLE (mUsb2HcHookQueue);

VOID
InternalDumpData (
  IN UINT8  *Data,
  IN UINTN  Size
  )
{
  UINTN  Index;
  for (Index = 0; Index < Size; Index++) {
    DEBUG ((EFI_D_INFO, "%02x", (UINTN)Data[Index]));
  }
}

#define USB_REQUEST_TYPE_TYPE_STANDARD       0

typedef union {
  struct {
    UINT8 Recipient:5;
    UINT8 Type:2;
    UINT8 Direction:1;
  } Bits;
  UINT8   Data;
} USB_REQUEST_TYPE;

CHAR8 *mUsbRequestTypeRecipientStr[] = {
  "Device",
  "Interface",
  "Endpoint",
  "Other",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Vendor",
};

CHAR8 *mUsbRequestTypeTypeStr[] = {
  "Standard",
  "Class",
  "Vendor",
  "Reserved",
};

CHAR8 *mUsbRequestTypeDirectionStr[] = {
  "H-D",
  "D-H",
};

CHAR8 mRequestTypeStringTemplate[] = "H-D Standard Interface";

CHAR8 *
UsbRequestTypeToString (
  IN EFI_USB_DEVICE_REQUEST *Request
  )
{
  USB_REQUEST_TYPE  ReqType;

  ReqType.Data = Request->RequestType;
  AsciiStrCpyS (mRequestTypeStringTemplate, sizeof(mRequestTypeStringTemplate), mUsbRequestTypeDirectionStr[ReqType.Bits.Direction]);
  AsciiStrCatS (mRequestTypeStringTemplate, sizeof(mRequestTypeStringTemplate), " ");
  AsciiStrCatS (mRequestTypeStringTemplate, sizeof(mRequestTypeStringTemplate), mUsbRequestTypeTypeStr[ReqType.Bits.Type]);
  AsciiStrCatS (mRequestTypeStringTemplate, sizeof(mRequestTypeStringTemplate), " ");
  AsciiStrCatS (mRequestTypeStringTemplate, sizeof(mRequestTypeStringTemplate), mUsbRequestTypeRecipientStr[ReqType.Bits.Recipient]);
  return mRequestTypeStringTemplate;
}

CHAR8 *mUsbRequestRequestStr[] = {
  "GET_STATUS",
  "CLEAR_FEATURE",
  "reserved",
  "SET_FEATURE",
  "reserved",
  "SET_ADDRESS",
  "GET_DESCRIPTOR",
  "SET_DESCRIPTOR",
  "GET_CONFIG",
  "SET_CONFIG",
  "GET_INTERFACE",
  "SET_INTERFACE",
  "SYNCH_FRAME",
};

CHAR8 *
UsbRequestRequestToString (
  IN EFI_USB_DEVICE_REQUEST *Request
  )
{
  USB_REQUEST_TYPE  ReqType;

  ReqType.Data = Request->RequestType;
  if (ReqType.Bits.Type == USB_REQUEST_TYPE_TYPE_STANDARD) {
    if (Request->Request < ARRAY_SIZE(mUsbRequestRequestStr)) {
      return mUsbRequestRequestStr[Request->Request];
    }
  }
  return "";
}

typedef union {
  struct {
    UINT8 DescIndex;
    UINT8 DescType;
  } Bits;
  UINT16   Data;
} USB_REQUEST_DESCRIPTOR;

CHAR8 *mUsbRequestValueDescriptorTypeStr[] = {
  "",
  "DEVICE",
  "CONFIG",
  "STRING",
  "INTERFACE",
  "ENDPOINT",
};

CHAR8 *
UsbRequestValueToString (
  IN EFI_USB_DEVICE_REQUEST *Request
  )
{
  USB_REQUEST_TYPE  ReqType;
  USB_REQUEST_DESCRIPTOR  Descriptor;

  ReqType.Data = Request->RequestType;
  if (ReqType.Bits.Type == USB_REQUEST_TYPE_TYPE_STANDARD) {
    if (Request->Request == USB_REQ_GET_DESCRIPTOR) {
      Descriptor.Data = Request->Value; 
      if (Descriptor.Bits.DescType < ARRAY_SIZE(mUsbRequestValueDescriptorTypeStr)) {
        return mUsbRequestValueDescriptorTypeStr[Descriptor.Bits.DescType];
      } else if (Descriptor.Bits.DescType == USB_DESC_TYPE_HID) {
        return "HID";
      } else if (Descriptor.Bits.DescType == USB_DESC_TYPE_REPORT) {
        return "REPORT";
      }
    }
  }
  return "";
}

USB2_HC_HOOK_PRIVATE *
GetUsb2HcHookPrivateFromThis (
  IN  EFI_USB2_HC_PROTOCOL  *This
  )
{
  LIST_ENTRY                        *Usb2HcHookLink;
  LIST_ENTRY                        *Usb2HcHookList;
  USB2_HC_HOOK_PRIVATE              *Usb2HcHookPrivate;

  Usb2HcHookList = &mUsb2HcHookQueue;
  for (Usb2HcHookLink = Usb2HcHookList->ForwardLink;
       Usb2HcHookLink != Usb2HcHookList;
       Usb2HcHookLink = Usb2HcHookLink->ForwardLink) {
    Usb2HcHookPrivate = CR (
                          Usb2HcHookLink,
                          USB2_HC_HOOK_PRIVATE,
                          Link,
                          USB2_HC_HOOK_PRIVATE_SIGNATURE
                          );
    if (Usb2HcHookPrivate->Usb2Hc == This) {
      return Usb2HcHookPrivate;
    }
  }

  ASSERT(FALSE);
  return NULL;
}

EFI_STATUS
EFIAPI
Usb2HcHookGetCapability (
  IN  EFI_USB2_HC_PROTOCOL  *This,
  OUT UINT8                 *MaxSpeed,
  OUT UINT8                 *PortNumber,
  OUT UINT8                 *Is64BitCapable
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  Status = Usb2HcHookPrivate->OrgUsb2Hc.GetCapability (
             Usb2HcHookPrivate->Usb2Hc,
             MaxSpeed,
             PortNumber,
             Is64BitCapable
             );
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): GetCapability - MaxSpeed - 0x%02x, PortNumber - 0x%02x, Is64BitCapable - 0x%02x\n", Usb2HcHookPrivate->Usb2Hc, *MaxSpeed, *PortNumber, *Is64BitCapable));
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookReset (
  IN EFI_USB2_HC_PROTOCOL   *This,
  IN UINT16                 Attributes
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): Reset - Attributes - 0x%04x\n", Usb2HcHookPrivate->Usb2Hc, Attributes));
  Status = Usb2HcHookPrivate->OrgUsb2Hc.Reset (
             Usb2HcHookPrivate->Usb2Hc,
             Attributes
             );
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookGetState (
  IN        EFI_USB2_HC_PROTOCOL    *This,
  OUT       EFI_USB_HC_STATE        *State
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  Status = Usb2HcHookPrivate->OrgUsb2Hc.GetState (
             Usb2HcHookPrivate->Usb2Hc,
             State
             );
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): GetState - State - 0x%08x\n", Usb2HcHookPrivate->Usb2Hc, State));
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookSetState (
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN EFI_USB_HC_STATE        State
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): SetState - State - 0x%08x\n", Usb2HcHookPrivate->Usb2Hc, State));
  Status = Usb2HcHookPrivate->OrgUsb2Hc.SetState (
             Usb2HcHookPrivate->Usb2Hc,
             State
             );
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookControlTransfer (
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
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  Status = Usb2HcHookPrivate->OrgUsb2Hc.ControlTransfer (
             Usb2HcHookPrivate->Usb2Hc,
             DeviceAddress,
             DeviceSpeed,
             MaximumPacketLength,
             Request,
             TransferDirection,
             Data,
             DataLength,
             TimeOut,
             Translator,
             TransferResult
             );
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): ControlTransfer: ", Usb2HcHookPrivate->Usb2Hc));
  DEBUG ((DEBUG_INFO, "Addr - 0x%02x, ", DeviceAddress));
  DEBUG ((DEBUG_INFO, "Speed - 0x%02x, ", DeviceSpeed));
  DEBUG ((DEBUG_INFO, "MaxPkgLen - 0x%x, ", MaximumPacketLength));
  DEBUG ((DEBUG_INFO, "Direction - 0x%x, ", TransferDirection));
  DEBUG ((DEBUG_INFO, "Request (Type - 0x%02x(%a), ", Request->RequestType, UsbRequestTypeToString (Request)));
  DEBUG ((DEBUG_INFO, "Request - 0x%02x(%a), ", Request->Request, UsbRequestRequestToString (Request)));
  DEBUG ((DEBUG_INFO, "Value - 0x%04x(%a), ", Request->Value, UsbRequestValueToString (Request)));
  DEBUG ((DEBUG_INFO, "Index - 0x%04x, Length - 0x%04x), ", Request->Index, Request->Length));
  DEBUG ((DEBUG_INFO, "Translator - (Hub - 0x%02x, Port - 0x%02x), ", Translator->TranslatorHubAddress, Translator->TranslatorPortNumber));
  if (TransferDirection == EfiUsbDataOut) {
    DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", *DataLength));
    DEBUG ((DEBUG_INFO, "Data - "));
    InternalDumpData (Data, *DataLength);
    DEBUG ((DEBUG_INFO, ", "));
  }
  if ((Status == EFI_SUCCESS) && (*TransferResult == EFI_USB_NOERROR)) {
    if (TransferDirection == EfiUsbDataIn) {
      DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", *DataLength));
      DEBUG ((DEBUG_INFO, "Data - "));
      InternalDumpData (Data, *DataLength);
      DEBUG ((DEBUG_INFO, ", "));
    }
  }
  DEBUG ((DEBUG_INFO, "Result - %x, ", *TransferResult));
  DEBUG ((DEBUG_INFO, "Status - %x", Status));
  DEBUG ((DEBUG_INFO, "\n"));
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookBulkTransfer (
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
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;
  EFI_USB_DATA_DIRECTION             TransferDirection;

  if ((EndPointAddress & 0x80) != 0) {
    TransferDirection = EfiUsbDataIn;
  } else {
    TransferDirection = EfiUsbDataOut;
  }

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  Status = Usb2HcHookPrivate->OrgUsb2Hc.BulkTransfer (
             Usb2HcHookPrivate->Usb2Hc,
             DeviceAddress,
             EndPointAddress,
             DeviceSpeed,
             MaximumPacketLength,
             DataBuffersNumber,
             Data,
             DataLength,
             DataToggle,
             TimeOut,
             Translator,
             TransferResult
             );
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): BulkTransfer: ", Usb2HcHookPrivate->Usb2Hc));
  DEBUG ((DEBUG_INFO, "Addr - 0x%02x, ", DeviceAddress));
  DEBUG ((DEBUG_INFO, "EndPoint - 0x%02x, ", EndPointAddress));
  DEBUG ((DEBUG_INFO, "Speed - 0x%02x, ", DeviceSpeed));
  DEBUG ((DEBUG_INFO, "MaxPkgLen - 0x%x, ", MaximumPacketLength));
  DEBUG ((DEBUG_INFO, "DataBufNumber - 0x%x, ", DataBuffersNumber));
  DEBUG ((DEBUG_INFO, "Translator - (Hub - 0x%02x, Port - 0x%02x), ", Translator->TranslatorHubAddress, Translator->TranslatorPortNumber));
  if (TransferDirection == EfiUsbDataOut) {
    DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", *DataLength));
    DEBUG ((DEBUG_INFO, "Data - "));
    InternalDumpData (*Data, *DataLength);
    DEBUG ((DEBUG_INFO, ", "));
  }
  if ((Status == EFI_SUCCESS) && (*TransferResult == EFI_USB_NOERROR)) {
    if (TransferDirection == EfiUsbDataIn) {
      DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", *DataLength));
      DEBUG ((DEBUG_INFO, "Data - "));
      InternalDumpData (*Data, *DataLength);
      DEBUG ((DEBUG_INFO, ", "));
    }
  }
  DEBUG ((DEBUG_INFO, "Result - %x, ", *TransferResult));
  DEBUG ((DEBUG_INFO, "Status - %x", Status));
  DEBUG ((DEBUG_INFO, "\n"));
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookAsyncInterruptTransfer (
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
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  // TBD
  Status = Usb2HcHookPrivate->OrgUsb2Hc.AsyncInterruptTransfer (
             Usb2HcHookPrivate->Usb2Hc,
             DeviceAddress,
             EndPointAddress,
             DeviceSpeed,
             MaxiumPacketLength,
             IsNewTransfer,
             DataToggle,
             PollingInterval,
             DataLength,
             Translator,
             CallBackFunction,
             Context
             );
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): AsyncInterruptTransfer: ", Usb2HcHookPrivate->Usb2Hc));
  DEBUG ((DEBUG_INFO, "Addr - 0x%02x, ", DeviceAddress));
  DEBUG ((DEBUG_INFO, "EndPoint - 0x%02x, ", EndPointAddress));
  DEBUG ((DEBUG_INFO, "Speed - 0x%02x, ", DeviceSpeed));
  DEBUG ((DEBUG_INFO, "MaxPkgLen - 0x%x, ", MaxiumPacketLength));
  DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", DataLength));
  DEBUG ((DEBUG_INFO, "Translator - (Hub - 0x%02x, Port - 0x%02x), ", Translator->TranslatorHubAddress, Translator->TranslatorPortNumber));
  DEBUG ((DEBUG_INFO, "Status - %x", Status));
  DEBUG ((DEBUG_INFO, "\n"));
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookSyncInterruptTransfer (
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
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;
  EFI_USB_DATA_DIRECTION             TransferDirection;

  if ((EndPointAddress & 0x80) != 0) {
    TransferDirection = EfiUsbDataIn;
  } else {
    TransferDirection = EfiUsbDataOut;
  }

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  Status = Usb2HcHookPrivate->OrgUsb2Hc.SyncInterruptTransfer (
             Usb2HcHookPrivate->Usb2Hc,
             DeviceAddress,
             EndPointAddress,
             DeviceSpeed,
             MaximumPacketLength,
             Data,
             DataLength,
             DataToggle,
             TimeOut,
             Translator,
             TransferResult
             );
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): SyncInterruptTransfer: ", Usb2HcHookPrivate->Usb2Hc));
  DEBUG ((DEBUG_INFO, "Addr - 0x%02x, ", DeviceAddress));
  DEBUG ((DEBUG_INFO, "EndPoint - 0x%02x, ", EndPointAddress));
  DEBUG ((DEBUG_INFO, "Speed - 0x%02x, ", DeviceSpeed));
  DEBUG ((DEBUG_INFO, "MaxPkgLen - 0x%x, ", MaximumPacketLength));
  DEBUG ((DEBUG_INFO, "Translator - (Hub - 0x%02x, Port - 0x%02x), ", Translator->TranslatorHubAddress, Translator->TranslatorPortNumber));
  if (TransferDirection == EfiUsbDataOut) {
    DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", *DataLength));
    DEBUG ((DEBUG_INFO, "Data - "));
    InternalDumpData (Data, *DataLength);
    DEBUG ((DEBUG_INFO, ", "));
  }
  if ((Status == EFI_SUCCESS) && (*TransferResult == EFI_USB_NOERROR)) {
    if (TransferDirection == EfiUsbDataIn) {
      DEBUG ((DEBUG_INFO, "DataLength - 0x%x, ", *DataLength));
      DEBUG ((DEBUG_INFO, "Data - "));
      InternalDumpData (Data, *DataLength);
      DEBUG ((DEBUG_INFO, ", "));
    }
  }
  DEBUG ((DEBUG_INFO, "Result - %x, ", *TransferResult));
  DEBUG ((DEBUG_INFO, "Status - %x", Status));
  DEBUG ((DEBUG_INFO, "\n"));
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookIsochronousTransfer (
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
Usb2HcHookAsyncIsochronousTransfer (
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
Usb2HcHookGetRootHubPortStatus (
  IN        EFI_USB2_HC_PROTOCOL    *This,
  IN        UINT8                   PortNumber,
  OUT       EFI_USB_PORT_STATUS     *PortStatus
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  Status = Usb2HcHookPrivate->OrgUsb2Hc.GetRootHubPortStatus (
             Usb2HcHookPrivate->Usb2Hc,
             PortNumber,
             PortStatus
             );
  if (PortStatus->PortChangeStatus != 0) {
    DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): GetRootHubPortStatus - PortNumber - 0x%02x, PortStatus - (0x%04x, Change - 0x%04x)\n", Usb2HcHookPrivate->Usb2Hc, PortNumber, PortStatus->PortStatus, PortStatus->PortChangeStatus));
  }
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookSetRootHubPortFeature (
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN UINT8                   PortNumber,
  IN EFI_USB_PORT_FEATURE    PortFeature
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): SetRootHubPortFeature - PortNumber - 0x%02x, PortFeature - 0x%08x\n", Usb2HcHookPrivate->Usb2Hc, PortNumber, PortFeature));
  Status = Usb2HcHookPrivate->OrgUsb2Hc.SetRootHubPortFeature (
             Usb2HcHookPrivate->Usb2Hc,
             PortNumber,
             PortFeature
             );
  return Status;
}

EFI_STATUS
EFIAPI
Usb2HcHookClearRootHubPortFeature (
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN UINT8                   PortNumber,
  IN EFI_USB_PORT_FEATURE    PortFeature
  )
{
  USB2_HC_HOOK_PRIVATE *Usb2HcHookPrivate;
  EFI_STATUS           Status;

  Usb2HcHookPrivate = GetUsb2HcHookPrivateFromThis (This);
  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): ClearRootHubPortFeature - PortNumber - 0x%02x, PortFeature - 0x%08x\n", Usb2HcHookPrivate->Usb2Hc, PortNumber, PortFeature));
  Status = Usb2HcHookPrivate->OrgUsb2Hc.ClearRootHubPortFeature (
             Usb2HcHookPrivate->Usb2Hc,
             PortNumber,
             PortFeature
             );
  return Status;
}

EFI_USB2_HC_PROTOCOL       mUsb2HcHookTemplate = {
  Usb2HcHookGetCapability,
  Usb2HcHookReset,
  Usb2HcHookGetState,
  Usb2HcHookSetState,
  Usb2HcHookControlTransfer,
  Usb2HcHookBulkTransfer,
  Usb2HcHookAsyncInterruptTransfer,
  Usb2HcHookSyncInterruptTransfer,
  Usb2HcHookIsochronousTransfer,
  Usb2HcHookAsyncIsochronousTransfer,
  Usb2HcHookGetRootHubPortStatus,
  Usb2HcHookSetRootHubPortFeature,
  Usb2HcHookClearRootHubPortFeature,
  0x0,
  0x0
};

VOID
EFIAPI
Usb2HcProtocolCallback (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  EFI_USB2_HC_PROTOCOL   *Usb2Hc;
  USB2_HC_HOOK_PRIVATE   *Usb2HcHookPrivate;
  EFI_STATUS             Status;

  Status = gBS->LocateProtocol (
                  &gEfiUsb2HcProtocolGuid,
                  mUsbHookRegistration,
                  (VOID **)&Usb2Hc
                  );
  if (EFI_ERROR(Status)) {
    return ;
  }

  DEBUG ((DEBUG_INFO, "USB_HOOK(0x%x): start hook\n", Usb2Hc));

  Usb2HcHookPrivate = AllocatePool (sizeof(USB2_HC_HOOK_PRIVATE));
  ASSERT (Usb2HcHookPrivate != NULL);

  Usb2HcHookPrivate->Signature = USB2_HC_HOOK_PRIVATE_SIGNATURE;
  CopyMem (&Usb2HcHookPrivate->OrgUsb2Hc, Usb2Hc, sizeof(EFI_USB2_HC_PROTOCOL));
  Usb2HcHookPrivate->Usb2Hc = Usb2Hc;
  mUsb2HcHookTemplate.MajorRevision = Usb2Hc->MajorRevision;
  mUsb2HcHookTemplate.MinorRevision = Usb2Hc->MinorRevision;
  CopyMem (Usb2Hc, &mUsb2HcHookTemplate, sizeof(EFI_USB2_HC_PROTOCOL));
  InsertTailList (&mUsb2HcHookQueue, &Usb2HcHookPrivate->Link);
}

EFI_STATUS
EFIAPI
InitializeUsb2HcHook (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  mUsbHookEvent = EfiCreateProtocolNotifyEvent (
                    &gEfiUsb2HcProtocolGuid,
                    TPL_CALLBACK,
                    Usb2HcProtocolCallback,
                    NULL,
                    &mUsbHookRegistration
                    );
  ASSERT(mUsbHookEvent != NULL);

  return EFI_SUCCESS;
}