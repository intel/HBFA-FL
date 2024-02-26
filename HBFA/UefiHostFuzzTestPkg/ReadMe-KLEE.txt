How to run KLEE with UefiHostTestPkg in Linux?
==============
KLEE:
Prepare Env
1)	"cd /home/<user name>"
	"mkdir Env"
2)	"sudo apt-get install build-essential curl libcap-dev cmake libncurses5-dev unzip libtcmalloc-minimal4 libgoogle-perftools-dev zlib1g-dev sqlite3 libsqlite3-dev uuid-dev texinfo bison flex libgmp3-dev libmpfr-dev libc6-dev-i386 doxygen python-pip"

Prepare KLEE
1)	install LLVM 3.8
	"sudo apt-get install clang-3.8 llvm-3.8 llvm-3.8-dev"
2)	install constraint solver STP
	a)	"sudo apt-get install libboost-all-dev perl minisat"
	b)	"cd /home/<user name>/Env"
	c)	"git clone https://github.com/stp/stp"
	d)	"cd stp"
	e)	"mkdir build"
	f)	"cd build"
	g)	"cmake .."
	h)	"make"
	i)	"sudo make install"
3)	build uclibc and the POSIX environment model:
	a)	"cd /home/<user name>/Env"
	b)	"git clone https://github.com/klee/klee-uclibc.git"
	c)	"cd klee-uclibc"
	d)	"./configure --make-llvm-lib"
	e)	"make -j2"
4)	get Google test sources
	a)	"cd /home/<user name>/Env"
	b)	"curl -OL https://github.com/google/googletest/archive/release-1.7.0.zip"
	c)	"unzip release-1.7.0.zip"
5)	install lit
	a)	"cd /home/<user name>/Env"
	b)	get lit source from https://pypi.org/project/lit/#files
	c)	"sudo pip install lit-0.6.0.tar.gz"
6)	get and configure KLEE
	a)	"cd /home/<user name>/Env"
	b)	"git clone https://github.com/klee/klee.git"
	c)	"mkdir klee_build_dir"
	NOTE: this directory must out of KLEE source code
	d)	"cd klee_build_dir"
	e)	"cmake \
		-DENABLE_SOLVER_STP=ON \
		-DENABLE_POSIX_RUNTIME=ON \
		-DENABLE_KLEE_UCLIBC=ON \
		-DKLEE_UCLIBC_PATH=<KLEE_UCLIBC_SOURCE_DIR> \
		-DGTEST_SRC_DIR=<GTEST_SOURCE_DIR> \
		-DENABLE_SYSTEM_TESTS=ON \
		-DENABLE_UNIT_TESTS=ON \
		-DLLVM_CONFIG_BINARY=<PATH_TO_LLVM38_llvm-config> \
		-DLLVMCC=<PATH_TO_clang-3.8> \
		-DLLVMCXX=<PATH_TO_clang++-3.8>
		<KLEE_SRC_DIRECTORY>"
	NOTE: where <KLEE_UCLIBC_SOURCE_DIR> is the absolute path the klee-uclibc source tree, <GTEST_SOURCE_DIR> is the absolute path to the Google Test source tree, <KLEE_SRC_DIRECTORY> is the absolute path the klee source tree.
		e.g.
		"cmake \
		-DENABLE_SOLVER_STP=ON \
		-DENABLE_POSIX_RUNTIME=ON \
		-DENABLE_KLEE_UCLIBC=ON \
		-DKLEE_UCLIBC_PATH=/home/<user name>/Env/klee-uclibc \
		-DGTEST_SRC_DIR=/home/<user name>/Env/googletest-release-1.7.0 \
		-DENABLE_SYSTEM_TESTS=ON \
		-DENABLE_UNIT_TESTS=ON \
		-DLLVM_CONFIG_BINARY=/usr/bin/llvm-config \
		-DLLVMCC=/usr/bin/clang-3.8 \
		-DLLVMCXX=/usr/bin/clang++-3.8 \
		/home/<user name>/Env/klee"
	f)	"make"
7)	you can varify whether your environment is setup correctly by running the first tutorial http://klee.github.io/tutorials/testing-function/
8)	Add below content at the end of ~/.bashrc:
	  export KLEE_BIN_PATH=<KLEE_BUILD_DIR>/bin
	  export KLEE_SRC_PATH=<KLEE_SRC_DIRECTORY>
	  export PATH=$PATH:$KLEE_BIN_PATH
	For example:
	  export KLEE_BIN_PATH=/home/<user name>/Env/klee_build_dir/bin
	  export KLEE_SRC_PATH=/home/<user name>/Env/klee
	  export PATH=$PATH:$KLEE_BIN_PATH
	NOTE: <KLEE_SRC_DIRECTORY> is the directory you clone klee from github in step 6 b), <KLEE_BUILD_DIR> is the directory you build klee in step 6 c)

Prepare wllvm
1)	get whole-program-llvm
	"cd /home/xxx/Env"
	"git clone https://github.com/travitch/whole-program-llvm.git"
2)	"cd whole-program-llvm"
3)	"sudo pip install -e ."

Build EDKII test case
1)	"python edk2-staging/HBFA/UefiHostTestTools/HBFAEnvSetup.py"
2)	"cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/build_rule.txt edk2/Conf/build_rule.txt"
3)	"cp edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/tools_def.txt edk2/Conf/tools_def.txt"
4)	"export SCRIPT_PATH=<WORKSPACE_PATH>/edk2-staging/HBFA/UefiHostFuzzTestPkg/Conf/LLVMLink.py"
5)	"export LLVM_COMPILER=clang"
	NOTE: if you can't find clang in /usr/bin, you can use clang-3.8 (clang-3.x)
6)	"build -p UefiHostFuzzTestCasePkg/UefiHostFuzzTestCasePkg.dsc -a IA32 -t KLEE -DTEST_WITH_KLEE --disable-include-path-check"

Run KLEE
1)	klee --only-output-states-covering-new Build/UefiHostFuzzTestCasePkg/DEBUG_KLEE/IA32/TestPartition"
2)	You will see something like below.

KLEE: output directory is "Build/UefiHostFuzzTestCasePkg/DEBUG_KLEE/IA32/klee-out-0"
KLEE: Using STP solver backend
KLEE: WARNING ONCE: Alignment of memory from call "malloc" is not modelled. Using alignment of 8.
KLEE: WARNING: killing 400 states (over memory cap)
KLEE: WARNING ONCE: skipping fork (memory cap exceeded)
KLEE: WARNING: killing 321 states (over memory cap)
KLEE: WARNING: killing 1291 states (over memory cap)
KLEE: WARNING: killing 860 states (over memory cap)
KLEE: WARNING: killing 996 states (over memory cap)
KLEE: WARNING: killing 946 states (over memory cap)
KLEE: WARNING: killing 839 states (over memory cap)
KLEE: WARNING: killing 500 states (over memory cap)
KLEE: WARNING: killing 802 states (over memory cap)
KLEE: WARNING: killing 629 states (over memory cap)
KLEE: WARNING: killing 789 states (over memory cap)
KLEE: WARNING: killing 634 states (over memory cap)
KLEE: WARNING: killing 753 states (over memory cap)
KLEE: WARNING: killing 552 states (over memory cap)

Transfer generated .ktest to seed file
1)	"python edk2-staging/HBFA/UefiHostTestTools/Script/TransferKtestToSeed.py <KTEST-FILE-FOLDER>"
2)	generate .seed file can be used as seeds for AFL-Fuzzer.

