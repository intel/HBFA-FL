#!/usr/bin/env python3
#
# Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
import os
import sys
import subprocess
import re


class GenLLVMReport(object):
    """A tool help streamline generation and collection of LLVM coverage \
        reports for the HBFA tool-suite."""
    def __init__(self, module_binary, report_path):
        self.log_file = None
        if self.validate_path_pattern(module_binary):
            self.module_binary = module_binary
        else:
            print("[!] 'module_binary' path not valid: "
                  "{}".format(module_binary))
            sys.exit(1)
        if self.validate_path_pattern(report_path):
            self.report_base_path = report_path
        else:
            print("[!] Report path provided to GenLLVMReport not valid:"
                  " {}".format(report_path))
            sys.exit(1)
        self.module_name = os.path.split(self.module_binary)[-1]
        self.profraw_file = os.getenv('LLVM_PROFILE_FILE')
        if not os.path.exists(self.profraw_file):
            print("[!] Profraw file not found [self.profraw]")
            sys.exit(1)

    def validate_path_pattern(self, path_uri):
        """
        Apply a simple regex to sanity-check if a directory path URI is
        valid
        """
        regex_dir_pattern = re.compile('^(/[^/ ]*)+/?$')
        return regex_dir_pattern.match(path_uri)

    def check_create_dir(self, dir_name):
        """Check if a directory exists, if not then create it."""
        if not os.path.isdir(dir_name):
            try:
                os.makedirs(dir_name)
            except OSError as error_msg:
                print("[!] Error when attempting to create directory:"
                      " {}".format(error_msg))
                sys.exit(1)

    def run_command(self, command):
        """Run a Linux OS command and return output."""
        try:
            proc = subprocess.Popen(command.split(' '),
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT,
                                    shell=False)
            msg = list(proc.communicate())
        except OSError as error:
            print("Exception encountered: {}".format(error))
            sys.exit(1)
        return msg

    def gen_reports(self):
        """Leverage llvm-profdata and llvm-cov tools to create reports."""
        self.check_create_dir(self.report_base_path)
        working_dir = self.report_base_path + '/llvm_coverage_report'
        self.check_create_dir(working_dir)
        commands = [
            'llvm-profdata merge -sparse ' + self.profraw_file + ' -o '
            + working_dir + '/default.profdata',
            'llvm-cov show --output-dir=' + working_dir +
            ' ' + self.module_binary + ' -instr-profile='
            + working_dir + '/default.profdata -format=html',
            'llvm-cov show --output-dir=' + working_dir +
            ' ' + self.module_binary + ' -instr-profile='
            + working_dir + '/default.profdata'
            ]

        for command in commands:
            msg = self.run_command(command)
            print(command)
            print(msg)
