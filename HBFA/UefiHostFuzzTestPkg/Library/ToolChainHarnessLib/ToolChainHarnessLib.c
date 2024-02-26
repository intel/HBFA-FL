/** @file

Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ToolChainHarnessLib.h>

#ifdef TEST_WITH_INSTRUMENT
#include <Library/InstrumentHookLib.h>
#endif

#ifdef TEST_WITH_LIBFUZZER
#include <stdint.h>
#include <stddef.h>
#endif

#ifdef TEST_WITH_KLEE
#include <klee/klee.h>
#endif

// TODO: xxx Improve coding style: naming, data type, etc

#ifdef TEST_WITH_INSTRUMENT
VOID
ReadInstrumentProfile (
  IN CHAR8 *FileName
  )
{
  VOID  *Buffer;

  FILE *f = fopen(FileName, "rb");
  if (f==NULL) {
    fputs ("File error",stderr);
    exit (1);
  }
  fseek(f, 0, SEEK_END);

  UINTN fsize = ftell(f);
  rewind(f);

  Buffer = malloc (fsize);
  size_t bytes_read = fread((void *)Buffer, 1, fsize, f);
  if ((UINTN)bytes_read!=fsize) {
    fputs ("File error",stderr);
    exit (1);
  }
  fclose(f);

  InstrumentHookLibInit (Buffer, fsize);
}
#endif

VOID
InitTestBuffer (
  int argc,
  char **argv,
  IN UINTN MaxBufferSize,
  IN VOID  **TestBuffer,
  OUT UINTN *BufferSize
  )
{
  // 1. Allocate buffer
  VOID  *Buffer = AllocatePool (MaxBufferSize);

  // 2. Assign to TestBuffer and BufferSize
    *TestBuffer = Buffer;
  if (BufferSize != NULL) {
    *BufferSize = MaxBufferSize;
  } else {
    printf ("error - Null pointer for Buffersize");
    exit(1);
  }

  // 3. Initialize TestBuffer
#ifdef TEST_WITH_KLEE
  // 3.1 For test with KLEE: write symbolic values to TestBuffer
  klee_make_symbolic((UINT8 *)Buffer, MaxBufferSize, "Buffer");
  return;
#endif

  // 3.2 For other tests: read values from file to TestBuffer
  //     (may also update the value of BufferSize)
  if (argc == 1) {
    printf ("error - missing input file\n");
    exit(1);
  }
  CHAR8 *FileName = argv[1];

  FILE *f = fopen(FileName, "rb");
  if (f==NULL) {
    fputs ("File error",stderr);
    exit (1);
  }
  fseek(f, 0, SEEK_END);

  UINTN fsize = ftell(f);
  rewind(f);

  fsize = fsize > MaxBufferSize ? MaxBufferSize : fsize;
  size_t bytes_read = fread((void *)Buffer, 1, fsize, f);
  if ((UINTN)bytes_read!=fsize) {
    fputs ("File error",stderr);
    exit (1);
  }
  fclose(f);
  if (BufferSize != NULL) {
    *BufferSize = fsize;
  } else {
    printf ("error - Null pointer for Buffersize");
    exit(1);
  }
  
  // 3.3 For ErrorInjection: read instrument profile for initialization
#ifdef TEST_WITH_INSTRUMENT
  if (argc >= 3) {
    ReadInstrumentProfile (argv[2]);
  }
#endif
}

#ifdef TEST_WITH_LIBFUZZER
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  VOID                   *TestBuffer;
  UINTN                  MaxBufferSize;

  // 1. Initialize TestBuffer
  MaxBufferSize = GetMaxBufferSize();
  TestBuffer = AllocateZeroPool (MaxBufferSize);
  if (Size > MaxBufferSize) {
    Size = MaxBufferSize;
  }
  CopyMem (TestBuffer, Data, Size);
  // 2. Run test
  RunTestHarness(TestBuffer, Size);
  // 3. Clean up
  FreePool (TestBuffer);
  return 0;
}
#else
#ifdef  TEST_WITH_LIBFUZZERWIN
int LLVMFuzzerTestOneInput(const wint_t *Data, size_t Size) {
  VOID                   *TestBuffer;
  UINTN                  MaxBufferSize;

  // 1. Initialize TestBuffer
  MaxBufferSize = GetMaxBufferSize();
  TestBuffer = AllocateZeroPool (MaxBufferSize);
  if (Size > MaxBufferSize) {
    Size = MaxBufferSize;
  }
  CopyMem (TestBuffer, Data, Size);
  // 2. Run test
  RunTestHarness(TestBuffer, Size);
  // 3. Clean up
  FreePool (TestBuffer);
  return 0;
}
#else
int main(int argc, char **argv)
{
  VOID                   *TestBuffer;
  UINTN                  TestBufferSize;

  // 1. Initialize TestBuffer
  InitTestBuffer (argc, argv, GetMaxBufferSize(), &TestBuffer, &TestBufferSize);
  // 2. Run test
  RunTestHarness(TestBuffer, TestBufferSize);
  // 3. Clean up
  FreePool (TestBuffer);
  return 0;
}
#endif
#endif
