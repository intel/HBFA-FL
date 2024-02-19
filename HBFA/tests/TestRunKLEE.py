#!/usr/bin/env python3
import unittest
import os
import sys
import io
import contextlib
try:
    from unittest import mock  # python 3.3+
except ImportError:
    import mock  # python 2.6-3.2


class TestRunKLEE(unittest.TestCase):

    # Test bad value for arch is handled (-a 'X64')
    @mock.patch('UefiHostTestTools.RunKLEE.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.RunKLEE', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.CheckTestEnv', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunKLEE.py', '-m', '/root/hbfa_workspace/edk2-'
                 'staging/HBFA/UefiHostFuzzTestCasePkg/TestCase'
                 '/MdeModulePkg/Library/BaseBmpSupportLib/'
                 'TestBmpSupportLib.inf', '-o',
                 '/tmp/test', '-a', 'X65', '-c', 'rawcommand'])
    def test_args_badarch(self, *args, **kwargs):
        print("\n[+] Testing bad value for arch: -a X65")
        with io.StringIO() as buffer:
            with contextlib.redirect_stderr(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunKLEE.main()
            check_string = "argument -a/--arch: invalid choice: 'X65' " + \
                           "(choose from 'IA32', 'X64', 'ARM', 'AARCH64')"
            self.assertIn(check_string, buffer.getvalue())

    # Test bad value for module is handled (not in HBFA_PATH)
    @mock.patch('UefiHostTestTools.RunKLEE.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.RunKLEE', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.CheckTestEnv', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunKLEE.py', '-m', '/tmp/TestBmpSupportLib.inf',
                 '-o', '/tmp/test', '-a', 'X64',
                 '-c', 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_module_hbfa_path(self, *args, **kwargs):
        print("\n[+] Test bad value for module is handled (not in HBFA_PATH)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunKLEE.main()
                message = buffer.getvalue()
                # Check against expected print string:
                # ModuleFile path: /tmp/TestBmpSupportLib.inf should start
                #     with /root/hbfa_workspace/edk2-staging/HBFA.
            check_string = b'\x4d\x6f\x64\x75\x6c\x65\x46\x69\x6c\x65\x20' \
                + b'\x70\x61\x74\x68\x3a\x20\x2f\x74\x6d\x70\x2f\x54\x65' \
                + b'\x73\x74\x42\x6d\x70\x53\x75\x70\x70\x6f\x72\x74\x4c' \
                + b'\x69\x62\x2e\x69\x6e\x66\x20\x73\x68\x6f\x75\x6c\x64' \
                + b'\x20\x62\x65\x20\x73\x74\x61\x72\x74\x20\x77\x69\x74' \
                + b'\x68\x20\x2f\x72\x6f\x6f\x74\x2f\x68\x62\x66\x61\x5f' \
                + b'\x77\x6f\x72\x6b\x73\x70\x61\x63\x65\x2f\x65\x64\x6b' \
                + b'\x32\x2d\x73\x74\x61\x67\x69\x6e\x67\x2f\x48\x42\x46' \
                + b'\x41\x2e\x0a'
            check_string = "/tmp/TestBmpSupportLib.inf should start with " \
                           + "/root/hbfa_workspace/edk2-staging/HBFA."
            self.assertIn(check_string, message)

    # Test bad value for module is handled (no value set)
    @mock.patch('UefiHostTestTools.RunKLEE.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.RunKLEE', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.CheckTestEnv', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv', ['RunKLEE.py', '-m', '', '-o',
                             '/tmp/test', '-a', 'X64', '-c', 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_module_no_value(self, *args, **kwargs):
        print("\n[+] Test bad value for module is handled (no value set)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunKLEE.main()
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
    @mock.patch('UefiHostTestTools.RunKLEE.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.RunKLEE', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.CheckTestEnv', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunKLEE.py', '-m',
                 'root/hbfa_workspace/edk2-staging/HBFA/test/test',
                 '-o', '/tmp/test', '-a', 'X64',
                 '-c', 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_module_bad_value(self, *args, **kwargs):
        print("\n[+] Test bad value for module is handled (bad value set)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunKLEE.main()
            message = buffer.getvalue()
            check_string = "root/hbfa_workspace/edk2-staging/HBFA/test/test" \
                           + " does not exist or is not in the relative" \
                           + " path for HBFA"
            self.assertIn(check_string, message)

    # Test bad value for output path (no argument)
    @mock.patch('UefiHostTestTools.RunKLEE.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.RunKLEE', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.CheckTestEnv', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunKLEE.py', '-m',
                 '/root/hbfa_workspace/edk2-staging/HBFA/'
                 'UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/'
                 'Library/BaseBmpSupportLib/TestBmpSupportLib.inf',
                 '-o', '', '-a', 'X64', '-c',
                 'rawcommand'])
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_output_seed_none(self, *args, **kwargs):
        print("\n[+]Test bad value for output seed path is handled "
              "(no argument)")
        with io.StringIO() as buffer:
            with contextlib.redirect_stdout(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunKLEE.main()
            message = buffer.getvalue()
            # Check against expected print string:
            # OutputSeed path should be set once by command -o OUTPUT,
            #  --output=OUTPUT.
            check_string = b'\x4f\x75\x74\x70\x75\x74\x53\x65\x65\x64\x20' \
                + b'\x70\x61\x74\x68\x20\x73\x68\x6f\x75\x6c\x64\x20\x62' \
                + b'\x65\x20\x73\x65\x74\x20\x6f\x6e\x63\x65\x20\x62\x79' \
                + b'\x20\x63\x6f\x6d\x6d\x61\x6e\x64\x20\x2d\x6f\x20\x4f' \
                + b'\x55\x54\x50\x55\x54\x2c\x20\x2d\x2d\x6f\x75\x74\x70' \
                + b'\x75\x74\x3d\x4f\x55\x54\x50\x55\x54\x2e\x0a'
            check_string = check_string.decode("ascii")
            self.assertEqual(message, check_string)

    # Test bad input value for command line mode (assuming we are on Linux)
    @mock.patch('UefiHostTestTools.RunKLEE.Build', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.RunKLEE', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.CheckTestEnv', return_value='')
    @mock.patch('UefiHostTestTools.RunKLEE.HBFA_PATH',
                '/root/hbfa_workspace/edk2-staging/HBFA')
    @mock.patch('sys.argv',
                ['RunKLEE.py', '-m',
                 '/root/hbfa_workspace/edk2-staging/HBFA/'
                 'UefiHostFuzzTestCasePkg/TestCase/MdeModulePkg/'
                 'Library/BaseBmpSupportLib/TestBmpSupportLib.inf',
                 '-o', '/tmp/test/test/test/test', '-a', 'X64',
                 '-c', 'gnome'])
    @mock.patch('UefiHostTestTools.RunKLEE.SysType', 'Linux')
    @mock.patch('os._exit', sys.exit)
    def test_args_bad_command_mode(self, *args, **kwargs):
        print("\n[+]Test bad value for command line mode")
        with io.StringIO() as buffer:
            with contextlib.redirect_stderr(buffer):
                with self.assertRaises(SystemExit):
                    UefiHostTestTools.RunKLEE.main()
            check_string = "invalid choice: 'gnome' (choose from " + \
                           "'rawcommand', 'manual')"
            self.assertIn(check_string, buffer.getvalue())


if __name__ == "__main__":
    sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
    import UefiHostTestTools.RunKLEE
    unittest.main()
