/** @file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TOOLCHAIN_HARNESS_LIB_
#define _TOOLCHAIN_HARNESS_LIB_

VOID
EFIAPI
RunTestHarness (
  IN VOID  *TestBuffer,
  IN UINTN TestBufferSize
  );

UINTN
EFIAPI
GetMaxBufferSize (
  VOID
  );

#endif