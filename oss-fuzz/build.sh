#!/bin/bash -eu
#
# Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
export CLANG_PATH=/usr/local/bin
export ASAN_SYMBOLIZER_PATH=/usr/local/bin/llvm-symbolizer
export PACKAGES_PATH="$SRC/hbfa-fl/HBFA:$SRC/edk2"
export EDK_TOOLS_PATH="$SRC/edk2/BaseTools"
export CONF_PATH="$SRC/edk2/Conf"
export WORKSPACE=$WORK

build_fuzzer() {
    fuzzer=$1
    inf=$2
    seed=${3:-""}

    if [ $SANITIZER == "coverage" -a -n "$COVERAGE_FLAGS" ]; then
        coverage="true"
        sanitizer="address"
    else
        coverage="false"
        sanitizer=$SANITIZER
    fi

    if [ ! -z $seed ]; then
        zip -j $OUT/${fuzzer}_seed_corpus.zip ${seed}/*
    fi

    python $SRC/hbfa-fl/HBFA/UefiHostTestTools/RunLibFuzzer.py -c manual -a X64 \
        -p $coverage \
        -s $sanitizer \
        -m $inf \
        -o $WORK

    cp $WORK/Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/$fuzzer $OUT
}

cd $SRC/edk2
for p in $(ls $SRC/hbfa-fl/edk2-bugfixes/*.patch); do patch -p1 < $p || :; done
make -C BaseTools
source edksetup.sh
cd ..
python $SRC/hbfa-fl/HBFA/UefiHostTestTools/HBFAEnvSetup.py

cp $SRC/hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt $SRC/edk2/Conf/build_rule.txt
cp $SRC/hbfa-fl/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt $SRC/edk2/Conf/tools_def.txt
cp $SRC/hbfa-fl/oss-fuzz/*.options $OUT

build_fuzzer "TestBmpSupportLib" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Library/BaseBmpSupportLib/TestBmpSupportLib.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/BMP/Raw"

build_fuzzer "TestTpm2CommandLib" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/Tpm2CommandLib/TestTpm2CommandLib.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/TPM/Raw"

build_fuzzer "TestPartition" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Disk/PartitionDxe/TestPartition.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/Partition"

build_fuzzer "TestUdf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Disk/UdfDxe/TestUdf.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/FileSystem"

build_fuzzer "TestPeiUsb" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Bus/Usb/UsbBusPei/TestPeiUsb.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/FileSystem"

build_fuzzer "TestFmpAuthenticationLibPkcs7" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/FmpAuthenticationLibPkcs7/TestFmpAuthenticationLibPkcs7.inf"

build_fuzzer "TestFmpAuthenticationLibRsa2048Sha256" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/SecurityPkg/Library/FmpAuthenticationLibRsa2048Sha256/TestFmpAuthenticationLibRsa2048Sha256.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/Capsule"

build_fuzzer "TestCapsulePei" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/CapsulePei/Common/TestCapsulePei.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/Capsule"

build_fuzzer "TestFileName" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/Universal/Disk/UdfDxe/TestFileName.inf" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/Seed/UDF/Raw/FileName"

build_fuzzer "TestVirtio10Blk" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/Virtio10BlkDxe/TestVirtio10Blk.inf"

build_fuzzer "TestVirtioBlk" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/VirtioBlkDxe/TestVirtioBlk.inf"

build_fuzzer "TestVirtioPciDevice" \
    "$SRC/hbfa-fl/HBFA/UefiHostFuzzTestCasePkg/TestCase/OvmfPkg/VirtioPciDeviceDxe/TestVirtioPciDevice.inf"
