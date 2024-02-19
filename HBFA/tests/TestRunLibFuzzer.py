#!/usr/bin/env python3
#
# Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
import unittest
import os
import sys
import io
import contextlib
try:
    from unittest import mock  # python 3.3+
except ImportError:
    import mock  # python 2.6-3.2


class TestRunLibFuzzer(unittest.TestCase):

    # Test bad value for arch is handled (-a 'X64')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv', ['RunLibFuzzer.py', '-m',
                             '/root/hbfa_workspace/edk2-'
                             'staging/HBFA/UefiHostFuzzTestCasePkg/TestCase'
                             '/MdeModulePkg/Library/BaseBmpSupportLib/'
                             'TestBmpSupportLib.inf', '-i', '/tmp/', '-o',
                             '/tmp/test', '-a', 'X65', '-c', 'rawcommand'])
    def test_args_badarch(self, *args, **kwargs):
        print("\n[+]Testing bad value for arch: -a X65")
        with io.StringIO() as buffer:
            with contextlib.redirect_stderr(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
            check_string = "argument -a/--arch: invalid choice: 'X65' " + \
                           "(choose from 'IA32', 'X64', 'ARM', 'AARCH64')"
            self.assertIn(check_string, buffer.getvalue())

    # Test bad value for module is handled (not in HBFA_PATH)
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv', ['RunLibFuzzer.py', '-m',
                             '/tmp/TestBmpSupportLib.inf', '-i', '/tmp/',
                             '-o', '/tmp/test', '-a', 'X64',
                             '-c', 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_module_hbfa_path(self, *args, **kwargs):
        print("\n[+]Test bad value for module is handled (not in HBFA_PATH)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
                message = buffer.getvalue()
                # Check against expected print string:
                # ModuleFile path: /tmp/TestBmpSupportLib.inf should start
                #     with /root/hbfa_workspace/edk2-staging/HBFA.
            check_string = b'\x4d\x6f\x64\x75\x6c\x65\x46\x69\x6c\x65\x20' \
                + b'\x70\x61\x74\x68\x3a\x20\x2f\x74\x6d\x70\x2f\x54\x65' \
                + b'\x73\x74\x42\x6d\x70\x53\x75\x70\x70\x6f\x72\x74\x4c' \
                + b'\x69\x62\x2e\x69\x6e\x66\x20\x73\x68\x6f\x75\x6c\x64' \
                + b'\x20\x73\x74\x61\x72\x74\x20\x77\x69\x74\x68\x20\x2f' \
                + b'\x72\x6f\x6f\x74\x2f\x68\x62\x66\x61\x5f\x77\x6f\x72' \
                + b'\x6b\x73\x70\x61\x63\x65\x2f\x65\x64\x6b\x32\x2d\x73' \
                + b'\x74\x61\x67\x69\x6e\x67\x2f\x48\x42\x46\x41\x2e\x0a'
            check_string = check_string.decode("ascii")
            self.assertEqual(message, check_string)

    # Test bad value for module is handled (no value set)
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv', ['RunLibFuzzer.py', '-m', '', '-i', '/tmp/', '-o',
                             '/tmp/test', '-a', 'X64', '-c', 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_module_no_value(self, *args, **kwargs):
        print("\n[+]Test bad value for module is handled (no value set)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
                # Check against expected print string:
                # ModuleFile path: /tmp/TestBmpSupportLib.inf should be
                # start with /root/hbfa_workspace/edk2-staging/HBFA
                message = buffer.getvalue()
            check_string = b'\x4d\x6f\x64\x75\x6c\x65\x46\x69\x6c\x65\x20' \
                + b'\x73\x68\x6f\x75\x6c\x64\x20\x62\x65\x20\x73\x65\x74' \
                + b'\x20\x6f\x6e\x63\x65\x20\x62\x79\x20\x63\x6f\x6d\x6d' \
                + b'\x61\x6e\x64\x20\x2d\x6d\x20\x4d\x4f\x44\x55\x4c\x45' \
                + b'\x46\x49\x4c\x45\x2c\x20\x2d\x2d\x6d\x6f\x64\x75\x6c' \
                + b'\x65\x3d\x4d\x4f\x44\x55\x4c\x45\x46\x49\x4c\x45' \
                + b'\x2e\x0a'
            check_string = check_string.decode("ascii")
            self.assertEqual(message, check_string)

    # Test bad value for module is handled (file does not exist or is not
    #  in relative path)
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunLibFuzzer.py', '-m',
                 'root/hbfa_workspace/edk2-staging/HBFA/test/test',
                 '-i', '/tmp/', '-o', '/tmp/test', '-a', 'X64',
                 '-c', 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_module_bad_value(self, *args, **kwargs):
        print("\n[+]Test bad value for module is handled (bad value set)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
                # Check against expected print string:
                # ModuleFile path:
                #  ModuleFile path: /tmp does not exist or is not in the
                #  relative path for HBFA
                message = buffer.getvalue()
            check_string = b'\x4d\x6f\x64\x75\x6c\x65\x46\x69\x6c\x65\x20' \
                + b'\x70\x61\x74\x68\x3a\x20\x2f\x74\x6d\x70\x20\x64\x6f' \
                + b'\x65\x73\x20\x6e\x6f\x74\x20\x65\x78\x69\x73\x74\x20' \
                + b'\x6f\x72\x20\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e\x20' \
                + b'\x74\x68\x65\x20\x72\x65\x6c\x61\x74\x69\x76\x65\x20' \
                + b'\x70\x61\x74\x68\x20\x66\x6f\x72\x20\x48\x42\x46\x41' \
                + b'\x0a'
            check_string = check_string.decode("ascii")
            self.assertEqual(message, check_string)

    # Test bad input value for command line mode (assuming we are on Linux)
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunLibFuzzer.py', '-m',
                 '/root/hbfa_workspace/edk2-staging/HBFA/'
                 'UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/'
                 'Library/BaseBmpSupportLib/TestBmpSupportLib.inf',
                 '-i', '/tmp/', '-o', '/tmp/test', '-a', 'X64',
                 '-c', 'gnome'])
    @mock.patch('UefiHostTestTools.RunLibFuzzer.SysType', 'Linux')
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_command_mode(self, *args, **kwargs):
        print("\n[+]Test bad value for command line mode")
        with io.StringIO() as buffer:
            with contextlib.redirect_stderr(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
            check_string = "argument -c/--commandline: invalid choice:" + \
                           " 'gnome' (choose from 'rawcommand', 'manual')"
            self.assertIn(check_string, buffer.getvalue())

    # Test bad value for -p profraw (not True, true, T, or t)
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('os._exit', sys.exit)
    @mock.patch('sys.argv', ['RunLibFuzzer.py', '-m',
                             '/root/hbfa_workspace/edk2-'
                             'staging/HBFA/UefiHostFuzzTestCasePkg/TestCase'
                             '/MdeModulePkg/Library/BaseBmpSupportLib/'
                             'TestBmpSupportLib.inf', '-i', '/tmp/', '-o',
                             '/tmp/test', '-a', 'X64', '-c', 'rawcommand',
                             '-p', 'ABCD'])
    def test_args_bad_profraw(self, *args, **kwargs):
        print("\n[+]Testing bad value for -p profraw (not True, true, "
              "T, or t)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stderr(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
            check_string = "argument -p/--gen-profraw: invalid choice: " + \
                           "'ABCD' (choose from 't', 'T', 'true', " + \
                           "'True', 'F', 'false', 'False')"
            self.assertIn(check_string, buffer.getvalue())

    # Test bad value for -s sanitizer (give a wrong value)
    @mock.patch('UefiHostTestTools.RunLibFuzzer.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.RunLibFuzzer',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.CheckTestEnv',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.updateBuildFlags',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.restoreBuildOptionFile',
                return_value='')
    @mock.patch('UefiHostTestTools.RunLibFuzzer.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('os._exit', sys.exit)
    @mock.patch('sys.argv', ['RunLibFuzzer.py', '-m',
                             '/root/hbfa_workspace/edk2-'
                             'staging/HBFA/UefiHostFuzzTestCasePkg/TestCase'
                             '/MdeModulePkg/Library/BaseBmpSupportLib/'
                             'TestBmpSupportLib.inf', '-i', '/tmp/', '-o',
                             '/tmp/test', '-a', 'X64', '-c', 'rawcommand',
                             '-p', 'T', '-s', 'abcd'])
    def test_args_bad_sanitizer(self, *args, **kwargs):
        print("\n[+]Testing bad value for -s sanitizer ('abcd')")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunLibFuzzer.main()
            # Check against expected error string:
            # [!] Unsupported sanitizer provided in option -s: [ abcd ]
            check_string = b'\x5b\x21\x5d\x20\x55\x6e\x73\x75\x70\x70\x6f' \
                + b'\x72\x74\x65\x64\x20\x73\x61\x6e\x69\x74\x69\x7a\x65' \
                + b'\x72\x20\x70\x72\x6f\x76\x69\x64\x65\x64\x20\x69\x6e' \
                + b'\x20\x6f\x70\x74\x69\x6f\x6e\x20\x2d\x73\x3a\x20\x5b' \
                + b'\x20\x61\x62\x63\x64\x20\x5d\x0a'
            check_string = check_string.decode("ascii")
            self.assertIn(check_string, buffer.getvalue())


if __name__ == "__main__":
    sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
    import UefiHostTestTools.RunLibFuzzer
    unittest.main()
