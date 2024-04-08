# Fuzzing test harnesses included with HBFA-FL

A number of fuzzing test harness cases are included in HBFA-FL. These, test-harnesses were included in the original [HBFA](https://github.com/tianocore/edk2-staging/tree/HBFA); however, the original HBFA includes a few additional test-cases not included here, which are described in the following [section](#test-cases-presently-not-included-in-hbfa-fl). Carefully examining all of these tests harnesses is very helpful for learning how to implement fuzzing tests harnesses in HBFA-FL. A listing of these files is presented in the following Table.

| Fuzzing Test Case Name | File Location (based from repository root) |
| ------------------------------------- | ----------------------------- |
| TestTpm2CommandLib | HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/Tpm2CommandLib/TestTpm2CommandLib.{c,inf} |
| TestBmpSupportLib | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.{c,inf} |
| TestPartition | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Disk/PartitionDxe/TestPartition.{c,inf} |
| TestUdf | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Disk/UdfDxe/TestUdf.{c,inf} |
| TestPeiUsb | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Bus/Usb/UsbBusPei/TestPeiUsb.{c,inf} |
| TestVariableSmm | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Variable/RuntimeDxe/TestVariableSmm.{c,inf} |
| TestFmpAuthenticationLibPkcs7 | HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/FmpAuthenticationLibPkcs7/TestFmpAuthenticationLibPkcs7.{c,inf} |
| TestFmpAuthenticationLibRsa2048Sha256 | HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/FmpAuthenticationLibRsa2048Sha256/TestFmpAuthenticationLibRsa2048Sha256.{c,inf} |
| TestCapsulePei | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/CapsulePei/Common/TestCapsulePei.{c,inf} |
| TestFileName | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Disk/UdfDxe/TestFileName.{c,inf} |
| TestValidateTdxCfv | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/EmuVariableFvbRuntimeDxe/TestValidateTdxCfv.{c,inf} |
| TestTcg2MeasureGptTable | HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/DxeTpm2MeasureBootLib/TestTcg2MeasureGptTable.{c,inf} |
| TestTcg2MeasurePeImage | HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/DxeTpm2MeasureBootLib/TestTcg2MeasurePeImage.{c,inf} |
| TestVirtioPciDevice | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/VirtioPciDeviceDxe/TestVirtioPciDevice.{c,inf} |
| TestVirtio10Blk | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/Virtio10BlkDxe/TestVirtio10Blk.{c,inf} |
| TestVirtioBlk | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/VirtioBlkDxe/TestVirtioBlk.{c,inf} |
| TestVirtioBlkReadWrite | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/VirtioBlkReadWrite/TestVirtioBlkReadWrite.{c,inf} |

Additionally, many of the test-cases make use of stub-libraries to simulate responses from function call that would interact with hardware. These libraries are included in HBFA in the relative directory:

```
├── HBFA
    ├── UefiHostFuzzTestCasePkb
        ├── TestStub
            ├── DiskStubLib
            ├── Tcg2StubLib
            ├── Tpm2DeviceLibStub
            ├── ...
```

## Seed files included

HBFA-FL includes some seed corpus for the included test-cases. The relative locations are shown in the following table.

| Case Name | Seed Location (based from repository root) |
| -------------- | ------------- |
| TestTpm2CommandLib | HBFA/UefiHostFuzzTestCasePkg/Seed/TPM/Raw
| TestBmpSupportLib | HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw
| TestPartition | HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/Partition
| TestUdf | HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/FileSystem
| TestPeiUsb | HBFA/UefiHostFuzzTestCasePkg/Seed/USB/Raw
| TestDxeCapsuleLibFmp | HBFA/UefiHostFuzzTestCasePkg/Seed/Capsule
| TestVariableSmm | HBFA/UefiHostFuzzTestCasePkg/Seed/VariableSmm/Raw
| TestFmpAuthenticationLibPkcs7 | HBFA/UefiHostFuzzTestCasePkg/Seed/Capsule
| TestFmpAuthenticationLibRsa2048Sha256 | HBFA/UefiHostFuzzTestCasePkg/Seed/Capsule
| TestCapsulePei | HBFA/UefiHostFuzzTestCasePkg/Seed/Capsule
| TestUpdateLockBoxFuzzLength | HBFA/UefiHostFuzzTestCasePkg/Seed/LockBox/Raw
| TestUpdateLockBoxFuzzOffset | HBFA/UefiHostFuzzTestCasePkg/Seed/LockBox/Raw
| TestFileName | HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/FileName

## Test-cases presently not included in HBFA-FL

The following fuzzing test-cases are not included in HBFA-FL; however, they are in the original HBFA. These test cases are no longer build and may be re-integrated at some future point.

| Fuzzing Test Case Name | File Location (based from repository root of HBFA) |
| ------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| TestDxeCapsuleLibFmp | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/DxeCapsuleLibFmp/TestDxeCapsuleLibFmp.{c,inf} |
| TestUpdateLockBoxFuzzLength | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/SmmLockBoxLib/UpdateLockBoxTestCase/TestUpdateLockBoxFuzzLength.{c,inf} |
| TestUpdateLockBoxFuzzOffset | HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/SmmLockBoxLib/UpdateLockBoxTestCase/TestUpdateLockBoxFuzzOffset.{c,inf} |
| TestHobList | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/Library/TdxStartupLib/TestHobList.{c,inf} |
| TestParseMmioExitInstructions | HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/Library/CcExitLib/TestParseMmioExitInstructions.{c,inf} |

[&lt;&lt;](../fuzzing/building.md) Back | Return to [Summary](../SUMMARY.md) | Next [&gt;&gt;](../fuzzing/fuzzingwithAFL.md)