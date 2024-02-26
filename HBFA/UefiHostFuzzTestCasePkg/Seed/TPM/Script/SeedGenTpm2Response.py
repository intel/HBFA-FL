# @file
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

'''
SeedGenTpm2Response
'''

import os
import sys
import argparse
import socket
import ctypes

IncludePath = os.path.join(os.path.dirname(
   os.path.dirname(os.path.dirname(os.path.realpath(__file__)))), 'Include')
sys.path.append(IncludePath)

#
# Globals for help information
#
__prog__ = 'SeedGenTpm2Response'
__version__ = '%s Version %s' % (__prog__, '0.11 ')
__copyright__ = 'Copyright (c) 2018, Intel Corporation. All rights reserved.'
__usage__ = '%s [options] -o <output_file>' % (__prog__)

TPM_ALG_SHA1 = 0x0004
TPM_ALG_SHA256 = 0x000B
TPM_ALG_SHA384 = 0x000C
TPM_ALG_SHA512 = 0x000D
TPM_ALG_SM3_256 = 0x0012

TPM_ST_NO_SESSIONS = 0x8001
TPM_ST_SESSIONS = 0x8002

TPM_RC_SUCCESS = 0x000


class TPM2_RESPONSE_HEADER(ctypes.Structure):
    _fields_ = [
        ('Dummy', ctypes.c_uint16),
        ('tag', ctypes.c_uint16),
        ('paramSize', ctypes.c_uint32),
        ('responseCode', ctypes.c_uint32),
        ]


class TPMT_HA(ctypes.Structure):
    _fields_ = [
        ('hashAlg', ctypes.c_uint16),
        ('digest', ctypes.ARRAY(ctypes.c_uint8, 32)),
        ]


class TPML_DIGEST_VALUES(ctypes.Structure):
    _fields_ = [
        ('count', ctypes.c_uint32),
        ('digests', ctypes.ARRAY(TPMT_HA, 100)),
        ]


class TPMS_AUTH_RESPONSE(ctypes.Structure):
    _fields_ = [
        ('dummy', ctypes.ARRAY(ctypes.c_uint8, 10)),
        ]


class TPM2_PCR_EVENT_RESPONSE(ctypes.Structure):
    _fields_ = [
        ('Header', TPM2_RESPONSE_HEADER),
        ('ParameterSize', ctypes.c_uint32),
        ('Digests', TPML_DIGEST_VALUES),
        ('AuthSessionPcr', TPMS_AUTH_RESPONSE),
        ]


if __name__ == '__main__':
    #
    # Create command line argument parser object
    #
    parser = argparse.ArgumentParser(prog=__prog__,
                                     usage=__usage__,
                                     description=__copyright__,
                                     conflict_handler='resolve')
    parser.add_argument('--version', action='version', version=__version__)
    parser.add_argument("-v", "--verbose", dest='Verbose',
                        action="store_true", help="increase output messages")
    parser.add_argument("-q", "--quiet", dest='Quiet', action="store_true",
                        help="reduce output messages")
    parser.add_argument("-o", "--output", dest='OutputFileName', type=str,
                        metavar='filename',
                        help="specify the output filename", required=True)

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    #
    # Write output file
    #
    args.OutputFile = open(args.OutputFileName, 'wb')

    DummyBuf = ctypes.create_string_buffer(
        ctypes.sizeof(TPM2_PCR_EVENT_RESPONSE))

    Tpm2PcrEventResponse = TPM2_PCR_EVENT_RESPONSE.from_buffer(DummyBuf, 0)
    Tpm2PcrEventResponse.Header.tag = socket.ntohs(TPM_ST_SESSIONS)
    Tpm2PcrEventResponse.Header.paramSize = socket.ntohl(0x56) \
        # socket.ntohl(sizeof(TPM2_PCR_EVENT_RESPONSE) - 2)
    Tpm2PcrEventResponse.Header.responseCode = socket.ntohl(TPM_RC_SUCCESS)
    Tpm2PcrEventResponse.ParameterSize = socket.ntohl(0x56) \
        # socket.ntohl(sizeof(TPM2_PCR_EVENT_RESPONSE) - 2)
    Tpm2PcrEventResponse.Digests.count = socket.ntohl(0x100)
    for i in range(0, 100):
        Tpm2PcrEventResponse.Digests.digests[i].hashAlg = \
            socket.ntohs(TPM_ALG_SHA256)

    args.OutputFile.write(DummyBuf[2:])

    args.OutputFile.close()
