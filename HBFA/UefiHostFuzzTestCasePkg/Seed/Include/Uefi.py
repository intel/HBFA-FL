# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import ctypes


class EFI_GUID(ctypes.Structure):
    _fields_ = [
        ('Guid1',            ctypes.c_uint32),
        ('Guid2',            ctypes.c_uint16),
        ('Guid3',            ctypes.c_uint16),
        ('Guid4',            ctypes.ARRAY(ctypes.c_uint8, 8)),
        ]


class EFI_TIME(ctypes.Structure):
    _fields_ = [
        ('Year',             ctypes.c_uint16),
        ('Month',            ctypes.c_uint8),
        ('Day',              ctypes.c_uint8),
        ('Hour',             ctypes.c_uint8),
        ('Minute',           ctypes.c_uint8),
        ('Second',           ctypes.c_uint8),
        ('Pad1',             ctypes.c_uint8),
        ('Nanosecond',       ctypes.c_uint32),
        ('TimeZone',         ctypes.c_int16),
        ('Daylight',         ctypes.c_uint8),
        ('Pad2',             ctypes.c_uint8),
        ]


EFI_VARIABLE_NON_VOLATILE = 0x00000001
EFI_VARIABLE_BOOTSERVICE_ACCESS = 0x00000002
EFI_VARIABLE_RUNTIME_ACCESS = 0x00000004
EFI_VARIABLE_HARDWARE_ERROR_RECORD = 0x00000008
EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS = 0x00000020
EFI_VARIABLE_APPEND_WRITE = 0x00000040
EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS = 0x00000010
