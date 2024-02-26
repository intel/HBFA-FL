/** @file
  Library functions that abstract driver model protocols
  installation and uninstallation.

  Copyright (c) 2019, NVIDIA Corporation. All rights reserved.
  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "UefiLibInternal.h"


/**
  Installs Driver Binding Protocol with optional Component Name and Component Name 2 Protocols.

  Initializes a driver by installing the Driver Binding Protocol together with the
  optional Component Name and optional Component Name 2 protocols onto the driver's
  DriverBindingHandle.  If DriverBindingHandle is NULL, then the protocols are installed
  onto a newly created handle.  DriverBindingHandle is typically the same as the driver's
  ImageHandle, but it can be different if the driver produces multiple Driver Binding Protocols.
  If DriverBinding is NULL, then ASSERT().
  If the installation fails, then ASSERT().

  @param  ImageHandle          The image handle of the driver.
  @param  SystemTable          The EFI System Table that was passed to the driver's entry point.
  @param  DriverBinding        A Driver Binding Protocol instance that this driver is producing.
  @param  DriverBindingHandle  The handle that DriverBinding is to be installed onto.  If this
                               parameter is NULL, then a new handle is created.
  @param  ComponentName        A Component Name Protocol instance that this driver is producing.
  @param  ComponentName2       A Component Name 2 Protocol instance that this driver is producing.

  @retval EFI_SUCCESS           The protocol installation successfully completed.
  @retval EFI_OUT_OF_RESOURCES  There was not enough memory in pool to install all the protocols.

**/
EFI_STATUS
EFIAPI
EfiLibInstallDriverBindingComponentName2 (
  IN CONST EFI_HANDLE                         ImageHandle,
  IN CONST EFI_SYSTEM_TABLE                   *SystemTable,
  IN EFI_DRIVER_BINDING_PROTOCOL              *DriverBinding,
  IN EFI_HANDLE                               DriverBindingHandle,
  IN CONST EFI_COMPONENT_NAME_PROTOCOL        *ComponentName,       OPTIONAL
  IN CONST EFI_COMPONENT_NAME2_PROTOCOL       *ComponentName2       OPTIONAL
  )
{
  EFI_STATUS  Status;

  ASSERT (DriverBinding != NULL);

  //
  // Update the ImageHandle and DriverBindingHandle fields of the Driver Binding Protocol
  //
  DriverBinding->ImageHandle         = ImageHandle;
  DriverBinding->DriverBindingHandle = DriverBindingHandle;

  if (ComponentName == NULL || FeaturePcdGet(PcdComponentNameDisable)) {
    if (ComponentName2 == NULL || FeaturePcdGet(PcdComponentName2Disable)) {
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &DriverBinding->DriverBindingHandle,
                      &gEfiDriverBindingProtocolGuid, DriverBinding,
                      NULL
                      );
      } else {
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &DriverBinding->DriverBindingHandle,
                      &gEfiDriverBindingProtocolGuid, DriverBinding,
                      &gEfiComponentName2ProtocolGuid, ComponentName2,
                      NULL
                      );
     }
  } else {
     if (ComponentName2 == NULL || FeaturePcdGet(PcdComponentName2Disable)) {
       Status = gBS->InstallMultipleProtocolInterfaces (
                       &DriverBinding->DriverBindingHandle,
                       &gEfiDriverBindingProtocolGuid, DriverBinding,
                       &gEfiComponentNameProtocolGuid, ComponentName,
                       NULL
                       );
     } else {
       Status = gBS->InstallMultipleProtocolInterfaces (
                       &DriverBinding->DriverBindingHandle,
                       &gEfiDriverBindingProtocolGuid, DriverBinding,
                       &gEfiComponentNameProtocolGuid, ComponentName,
                       &gEfiComponentName2ProtocolGuid, ComponentName2,
                       NULL
                       );
    }
  }

  //
  // ASSERT if the call to InstallMultipleProtocolInterfaces() failed
  //
  ASSERT_EFI_ERROR (Status);

  return Status;
}


