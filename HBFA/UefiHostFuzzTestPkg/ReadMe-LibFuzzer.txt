How to run LibFuzzer in OS?
==============
Install LLVM in Linux
1)	Download LLVM 8.0.0 Linux pre build binary from http://releases.llvm.org/download.html 
2)	Extract clang+llvm-x.x.x-<system_info>.tar.xz
	mv clang+llvm-x.x.x-<system_info>.tar.xz /home/<user name>/Env
	cd /home/<user name>/Env
	xz -d clang+llvm-x.x.x-<system_info>.tar.xz
3)	Extract clang+llvm-x.x.x-<system_info>.tar
	tar xvf clang+llvm-x.x.x-<system_info>.tar
4)	Add below content at the end of ~/.bashrc:
	  export CLANG_PATH=<LLVM_CLANG_BUILD_DIR>/bin
	  export ASAN_SYMBOLIZER_PATH=$CLANG_PATH/llvm-symbolizer
	For example:
	  export CLANG_PATH=/home/tiano/Env/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin
	  export ASAN_SYMBOLIZER_PATH=$CLANG_PATH/llvm-symbolizer

===============
Install LLVM in Windows (currently CLAGN8)
***NOTE: current CLAGN8 support Sanitizer (IA32/X64) and LIBFUZZER (X64) in Windows.
1)	Download LLVM 8.0.0 windows pre build binary from http://releases.llvm.org/download.html and setup LLVM environment.
Note: Please install 64bit exe for X64 build and 32bit exe for IA32 build.
2)	After LLVM setup done, please add LLVM installed location to system environment variable:
	  set LLVM_PATH=<64_LLVM_PATH>
	  set LLVMx86_PATH =<32_LLVM_PATH>
	For example:
	  set LLVM_PATH=C:\Program Files\LLVM
	  set LLVMx86_PATH =C:\Program Files (x86)\LLVM
3)	Add %LLVM_PATH%\bin, %LLVM_PATH%\lib\clang\8.0.0\lib\windows and %LLVMx86_PATH%\lib\clang\8.0.0\lib\windows to system environment variable PATH.


===========================
Run Clang in Linux
1)	python edk2-staging/HBFA/UefiHostTestTools/HBFAEnvSetup.py
2)	cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt edk2/Conf/build_rule.txt
3)	cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt edk2/Conf/tools_def.txt
4)	export PATH=$CLANG_PATH:$PATH
5)	build with "CLANG8" as toolchain.
	build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a X64 -t CLANG8
   
Run Clang in Windows
1)	python edk2-staging\HBFA\UefiHostTestTools\HBFAEnvSetup.py
2)	copy edk2-staging\HBFA\UefiHostFuzzTestPkg\Conf\build_rule.txt edk2\Conf\build_rule.txt
3)	copy edk2-staging\HBFA\UefiHostFuzzTestPkg\Conf\tools_def.txt edk2\Conf\tools_def.txt
4)	build with "CLANGWIN" as toolchain.
	build -p UefiHostFuzzTestCasePkg\UefiHostFuzzTestCasePkg.dsc -a X64 -t CLANGWIN

===========================
Run LibFuzzer in Linux:
1)	python edk2-staging/HBFA/UefiHostTestTools/HBFAEnvSetup.py
2)	cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt edk2/Conf/build_rule.txt
3)	cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt edk2/Conf/tools_def.txt
4)	export PATH=$CLANG_PATH:$PATH
5)	build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a X64 -t LIBFUZZER
6)	mkdir NEW_CORPUS_DIR
7)	cp HBFA/UefiHostFuzzTestCasePkg/Seed/XXX/Raw/Xxx.bin NEW_CORPUS_DIR
8)	./Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestXxx  NEW_CORPUS_DIR -rss_limit_mb=0 -artifact_prefix=<OUTPUT_PATH>/

Run Clang in Windows
1)	python edk2-staging\HBFA\UefiHostTestTools\HBFAEnvSetup.py
2)	copy edk2-staging\HBFA\UefiHostFuzzTestPkg\Conf\build_rule.txt edk2\Conf\build_rule.txt
3)	copy edk2-staging\HBFA\UefiHostFuzzTestPkg\Conf\tools_def.txt edk2\Conf\tools_def.txt
4)	build -p UefiHostFuzzTestCasePkg\UefiHostFuzzTestCasePkg.dsc -a X64 -t LIBFUZZERWIN
5)	mkdir NEW_CORPUS_DIR
6)	copy HBFA\UefiHostFuzzTestCasePkg\Seed\XXX\Raw\Xxx.bin NEW_CORPUS_DIR
7)	.\Build\UefiHostFuzzTestCasePkg\DEBUG_LIBFUZZERWIN\X64\TestXxx.exe  NEW_CORPUS_DIR -rss_limit_mb=0 -artifact_prefix=<OUTPUT_PATH>\

===========================
Sanitizer Coverage:
1)	goto http://clang.llvm.org/docs/SanitizerCoverage.html, read the content.
2)	ASAN_OPTIONS=coverage=1 ./Build/UefiHostFuzzTestCasePkg/DEBUG_LIBFUZZER/X64/TestXxx
3)	sancov -symbolize TestXxx.123.sancov my_program > TestXxx.123.symcov
4)	python3 tools/sancov/coverage-report-server.py --symcov TestXxx.123.symcov --srcpath <source code root>
