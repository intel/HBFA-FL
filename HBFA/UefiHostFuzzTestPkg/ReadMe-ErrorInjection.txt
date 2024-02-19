How to do error injection?
==============
Build
1) Make sure UefiInstrumentTestPkg is in PACKAGES_PATH.
2) build -p UefiHostFuzzTestCasePkg\UefiHostFuzzTestCasePkg.dsc -a IA32 -t XXX -D TEST_WITH_INSTRUMENT

Write Error Injection Profile
1) Create .Ini file like below: (test.ini)
#######################
# CallErrorCount=N means which call returns error. N means the Nth call returns error.
#   N start from 1. CallErrorCount = 0 means disable.
# ReturnValue=X means when error happens, which value is returned.
#######################

[AllocateZeroPool]
  CallErrorCount = 1
  ReturnValue = 0

[ReadBlocks]
  CallErrorCount = 1
  ReturnValue = EFI_DEVICE_ERROR

[ReadDisk]
  CallErrorCount = 1
  ReturnValue = EFI_DEVICE_ERROR
#######################

Run
1) <TestApp> <Seed> <TestIni>
for example: Build\UefiHostFuzzTestCasePkg\DEBUG_VS2015x86\IA32\TestPartition.exe UefiHostFuzzTestCasePkg\Seed\UDF\Raw\Udf_linux.bin test.ini

Recommendation:
1) We had better provide a set of seeds, which already have high coverage. Only error handling path is not covered.
2) We had better have a script to increase CallErrorCount from 1 to N. And run the TestApp again and again.
   As such, all the error can be returned.

