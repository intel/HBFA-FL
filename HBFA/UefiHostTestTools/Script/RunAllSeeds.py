# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import subprocess

workdir = os.getcwd()

tcsbin_path = sys.argv[1]
findings_dir_path = sys.argv[2]


def run_all_seeds():
    for root, dirs, files in os.walk(findings_dir_path):
        for dir in dirs:
            if dir in ['crashed', 'hangs', 'queue']:
                dir_path = os.path.join(root, dir)
                for file in os.listdir(dir_path):
                    file_path = os.path.join(dir_path, file)
                    if os.path.isfile(file_path):
                        cmd = './' + tcsbin_path + ' ' + file_path
                        print(cmd)
                        try:
                            subprocess.run(cmd.split(' '), shell=False,
                                           check=True)
                        except subprocess.CalledProcessError as e:
                            print(e)


if __name__ == '__main__':
    run_all_seeds()
