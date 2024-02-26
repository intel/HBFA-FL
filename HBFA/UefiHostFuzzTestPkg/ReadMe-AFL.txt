How to run AFL with UefiHostTestPkg in OS?
==============
AFL:
Prepare AFL in Linux
1)	Goto http://lcamtuf.coredump.cx/afl/, download http://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz
2)	Extract afl-latest.tgz:
	mv afl-latest.tgz /home/<user name>/Env
	cd /home/<user name>/Env
	tar xzvf afl-latest.tgz
3)	Follow docs\QuickStartGuide.txt in AFL package to quickly make AFL:
	cd afl-2.5.2b
	make
4)	Add below content at the end of ~/.bashrc:
	  export AFL_PATH=<AFL_PATH>
	  export PATH=$PATH:$AFL_PATH
	For example:
	  export AFL_PATH=/home/<user name>/Env/afl-2.52b
	  export PATH=$PATH:$AFL_PATH
5)	Run these commands as root (these command need to be ran every time you reboot your OS):
	sudo bash -c 'echo core >/proc/sys/kernel/core_pattern'
	cd /sys/devices/system/cpu/
	sudo bash -c 'echo performance | tee cpu*/cpufreq/scaling_governor'

Prepare AFL in Windows
NOTE: as known issue https://github.com/googleprojectzero/winafl/issues/145 that cause compatibility issues in recent Windows versions, the author has disabled Drsyms in recent WinAFL builds, if you want you use the newest version, please according to Method.2 to rebuild winafl yourself.
Method.1:
1)	goto https://github.com/googleprojectzero/winafl, clone the project and checkout version d501350f02147860604b5d755960fe3fc201653a
	git clone https://github.com/googleprojectzero/winafl.git
	cd winafl
	git checkout -b Branch_d501350f d501350f02147860604b5d755960fe3fc201653a
2)	download DynamoRIO release 6.2.0-2 from https://github.com/DynamoRIO/dynamorio/releases/tag/release_6_2_0
3)	setup AFL_PATH=<...>\winafl, DRIO_PATH=<...>\DynamoRIO-Windows-6.2.0-2
Method.2:
1)	get newest winafl
	git clone https://github.com/googleprojectzero/winafl.git
2)	download newest DynamoRIO release from https://github.com/DynamoRIO/dynamorio/wiki/Downloads
3)	build winafl
	cd winafl
	mkdir build32
	cd build32
	cmake -G"Visual Studio 14 2015" .. -DDynamoRIO_DIR=%DRIO_PATH%\cmake -DUSE_DRSYMS=1
	cmake --build . --config Release
	cd ..
	mkdir build64
	cmake -G"Visual Studio 14 2015 Win64" .. -DDynamoRIO_DIR=%DRIO_PATH%\cmake -DUSE_DRSYMS=1
	cmake --build . --config Release
	NOTE: If you get errors where the linker couldn't find certain .lib files. please refer to https://github.com/googleprojectzero/winafl/issues/145 and delete the nonexistent files from "Additional Dependencies".
4)	copy all binary under build32/bin/Release to bin32, copy all binary under build64/bin/Release to bin64
	robocopy /E /is /it build32/bin/Release bin32
	robocopy /E /is /it build64/bin/Release bin64
5)	setup AFL_PATH=<...>\winafl, DRIO_PATH=<...>\DynamoRIO-Windows-x.x.x-x



Build EDKII test case in Linux
1)	python edk2-staging/HBFA/UefiHostTestTools/HBFAEnvSetup.py
2)	cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt edk2/Conf/build_rule.txt
3)	cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt edk2/Conf/tools_def.txt
4)	build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a IA32 -t AFL

Build EDKII test case in Windows
1)	build -p UefiHostFuzzTestCasePkg\UefiHostFuzzTestCasePkg.dsc -a IA32 -t VS2015x86



Run AFL in Linux
1)	mkdir testcase_dir
2)	mkdir /dev/shm/findings_dir
	Note:'/dev/shm' is tmpfs, so 'findings_dir' will disappear after reboot system, please backup test result before reboot system.
3)	cp edk2-staging/HBFA/UefiHostFuzzTestCasePkg/Seed/<SPECIFIC_SEED_FOLDER>/xxx.bin testcase_dir
	NOTE: <SPECIFIC_SEED_FOLDER> mapping list please refer to edk2-staging/HBFA/UefiHostFuzzTestCasePkg/Seed/readme.txt
4)	afl-fuzz -i testcase_dir -o /dev/shm/findings_dir Build/UefiHostFuzzTestCasePkg/DEBUG_AFL/IA32/TestPartition @@
5)	You will see something like below. Have fun!

Run AFL in Windows
1)	mkdir %AFL_PATH%\bin32\in
2)	mkdir %AFL_PATH%\bin32\out
3)	cp edk2-staging/HBFA/UefiHostFuzzTestCasePkg\Seed\<SPECIFIC_SEED_FOLDER>\xxx.bin %AFL_PATH%\bin32\in
	NOTE: <SPECIFIC_SEED_FOLDER> mapping list please refer to edk2-staging/HBFA\UefiHostFuzzTestCasePkg\Seed\readme.txt
4)	Copy xxx.exe and xxx.pdb to the same dir as %AFL_PATH%\bin32 or %AFL_PATH%\bin64, for example: TestPartition.exe and TestPartition.pdb. (NOTE: xxx.pdb must be copied)
5)	cd %AFL_PATH%\bin32
	afl-fuzz.exe -i in -o out -D %DRIO_PATH%\bin32 -t 20000 -- -coverage_module xxx.exe -fuzz_iterations 1000 -target_module xxx.exe -target_method main -nargs 2 -- xxx.exe @@ 
	or
	cd %AFL_PATH%\bin64
	afl-fuzz.exe -i in -o out -D %DRIO_PATH%\bin64 -t 20000 -- -coverage_module xxx.exe -fuzz_iterations 1000 -target_module xxx.exe -target_method main -nargs 2 -- xxx.exe @@ 
6)	You will see similar output, although it is slower than Linux.


                    american fuzzy lop 2.52b (TestPartition)

lq process timing qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqwq overall results qqqqqk
x        run time : 0 days, 0 hrs, 0 min, 5 sec        x  cycles done : 40     x
x   last new path : none yet (odd, check syntax!)      x  total paths : 1      x
x last uniq crash : none seen yet                      x uniq crashes : 0      x
x  last uniq hang : none seen yet                      x   uniq hangs : 0      x
tq cycle progress qqqqqqqqqqqqqqqqqqqqwq map coverage qvqqqqqqqqqqqqqqqqqqqqqqqu
x  now processing : 0 (0.00%)         x    map density : 0.00% / 0.00%         x
x paths timed out : 0 (0.00%)         x count coverage : 1.00 bits/tuple       x
tq stage progress qqqqqqqqqqqqqqqqqqqqnq findings in depth qqqqqqqqqqqqqqqqqqqqu
x  now trying : havoc                 x favored paths : 1 (100.00%)            x
x stage execs : 164/256 (64.06%)      x  new edges on : 1 (100.00%)            x
x total execs : 11.8k                 x total crashes : 0 (0 unique)           x
x  exec speed : 2092/sec              x  total tmouts : 0 (0 unique)           x
tq fuzzing strategy yields qqqqqqqqqqqvqqqqqqqqqqqqqqqwq path geometry qqqqqqqqu
x   bit flips : 0/32, 0/31, 0/29                      x    levels : 1          x
x  byte flips : 0/4, 0/3, 0/1                         x   pending : 0          x
x arithmetics : 0/224, 0/204, 0/68                    x  pend fav : 0          x
x  known ints : 0/8, 0/18, 0/10                       x own finds : 0          x
x  dictionary : 0/0, 0/0, 0/0                         x  imported : n/a        x
x       havoc : 0/11.0k, 0/0                          x stability : 100.00%    x
x        trim : 100.00%/29, 0.00%                     tqqqqqqqqqqqqqqqqqqqqqqqqj
^Cqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj          [cpu000: 70%]



