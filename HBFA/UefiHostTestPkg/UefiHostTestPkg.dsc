## @file UefiHostTestPkg.dsc
# 
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = UefiHostTestPkg
  PLATFORM_GUID                  = C2A9A6F1-57DC-4397-ACB6-BCF5DF454BC0
  PLATFORM_VERSION               = 0.11
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/UefiHostTestPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[LibraryClasses]
  BaseLib|UefiHostTestPkg/Library/BaseLibHost/BaseLibHost.inf
  CacheMaintenanceLib|UefiHostTestPkg/Library/BaseCacheMaintenanceLibHost/BaseCacheMaintenanceLibHost.inf
  BaseMemoryLib|UefiHostTestPkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  MemoryAllocationLib|UefiHostTestPkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  DebugLib|UefiHostTestPkg/Library/DebugLibHost/DebugLibHost.inf
  UefiBootServicesTableLib|UefiHostTestPkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  HobLib|UefiHostTestPkg/Library/HobLibHost/HobLibHost.inf
  SmmMemLib|UefiHostTestPkg/Library/SmmMemLibHost/SmmMemLibHost.inf
  DevicePathLib|UefiHostTestPkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf

  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf

[LibraryClasses.common.USER_DEFINED]

[Components]
  UefiHostTestPkg/Library/BaseLibHost/BaseLibHost.inf
  UefiHostTestPkg/Library/BaseLibNullCpuid/BaseLibNullCpuid.inf
  UefiHostTestPkg/Library/BaseLibNullMsr/BaseLibNullMsr.inf
  UefiHostTestPkg/Library/BaseCacheMaintenanceLibHost/BaseCacheMaintenanceLibHost.inf
  UefiHostTestPkg/Library/BaseCpuLibHost/BaseCpuLibHost.inf
  UefiHostTestPkg/Library/BaseMemoryLibHost/BaseMemoryLibHost.inf
  UefiHostTestPkg/Library/BaseTimerLibHost/BaseTimerLibHost.inf
  UefiHostTestPkg/Library/MemoryAllocationLibHost/MemoryAllocationLibHost.inf
  UefiHostTestPkg/Library/DebugLibHost/DebugLibHost.inf
  UefiHostTestPkg/Library/UefiBootServicesTableLibHost/UefiBootServicesTableLibHost.inf
  UefiHostTestPkg/Library/UefiRuntimeServicesTableLibHost/UefiRuntimeServicesTableLibHost.inf
  UefiHostTestPkg/Library/DxeServicesTableLibHost/DxeServicesTableLibHost.inf
  UefiHostTestPkg/Library/PeiServicesTablePointerLibHost/PeiServicesTablePointerLibHost.inf
  UefiHostTestPkg/Library/HobLibHost/HobLibHost.inf
  UefiHostTestPkg/Library/UefiDevicePathLibHost/UefiDevicePathLibHost.inf
  UefiHostTestPkg/Library/SmmMemLibHost/SmmMemLibHost.inf
  UefiHostTestPkg/Library/UefiLibHost/UefiLibHost.inf
  UefiHostTestPkg/Library/PeimEntryPointHost/PeimEntryPointHost.inf
  UefiHostTestPkg/Library/UefiDriverEntryPointHost/UefiDriverEntryPointHost.inf
  UefiHostTestPkg/Library/OsServiceLibHost/OsServiceLibHost.inf

!include UefiHostTestPkg/UefiHostTestBuildOption.dsc
