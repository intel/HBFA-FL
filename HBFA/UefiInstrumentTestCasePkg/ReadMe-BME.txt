How to collect PCI BME access in BIOS?
==============
1) Add below in Platform.dsc

[LibraryClasses.common]
  NULL|UefiInstrumentTestPkg/Library/InstrumentLib/InstrumentLib.inf
  InstrumentHookLib|UefiInstrumentTestPkg/TestCase/InstrumentHookLibTracingPci/InstrumentHookLibTracingPci.inf

  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  SerialPortLib|PcAtChipsetPkg/Library/SerialIoLib/SerialIoLib.inf

NOTE: The UefiInstrumentTestPkg provides the hook capability.
NOTE: Overriding debug serial port lib is needed, because we want to use simple serial port output to dump the resource access.

2) Add below in Platform.dsc

[Components]

  MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf {
    <BuildOptions>
      MSFT:  *_*_*_CC_FLAGS = /Gh /GH /Od /GL-
  }

3) Build and run the new BIOS image.
You may see some resource access below:

RSC_TRACE: MmioRead8(0xE00F9004->0x06) - FFF0ACA0 from FFF0AB41 (*** TOUCH PCIE BME ***)
RSC_TRACE: MmioWrite8(0xE00F9004<-0x06) - FFF0ADF0 from FFF0AB57 (*** TOUCH PCIE BME ***)

RSC_TRACE: IoWrite32(0xCF8<-0x80000004) - 89FEF558 from 89FEE52A (*** TOUCH PCI BME ***)
RSC_TRACE: IoRead32(0xCFC->0x20900006) - 89FEF46C from 89FEE532
