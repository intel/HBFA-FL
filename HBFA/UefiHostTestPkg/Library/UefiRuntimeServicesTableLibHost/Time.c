/** @file

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

EFI_STATUS
EFIAPI
CoreGetTime (
  OUT  EFI_TIME                    *Time,
  OUT  EFI_TIME_CAPABILITIES       *Capabilities OPTIONAL
  )
{
  time_t timer;
  struct tm *info;

  if (Time != NULL) {
    ZeroMem (Time, sizeof(*Time));
    time (&timer);
    info = localtime (&timer);
    Time->Year = (UINT16)(info->tm_year + 1900);
    Time->Month = (UINT8)(info->tm_mon + 1);
    Time->Day = (UINT8)info->tm_mday;
    Time->Hour = (UINT8)info->tm_hour;
    Time->Minute = (UINT8)info->tm_min;
    Time->Second = (UINT8)info->tm_sec;
  }
  if (Capabilities != NULL) {
    ZeroMem (Capabilities, sizeof(*Capabilities));
  }
  return EFI_SUCCESS;
}
