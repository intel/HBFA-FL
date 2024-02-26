;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   InterlockedCompareExchange64.Asm
;
; Abstract:
;
;   InterlockedCompareExchange64 function
;
; Notes:
;
;------------------------------------------------------------------------------

    .586P
    .model  flat,C
    .code

;------------------------------------------------------------------------------
; UINT64
; EFIAPI
; InternalSyncCompareExchange64 (
;   IN      volatile UINT64           *Value,
;   IN      UINT64                    CompareValue,
;   IN      UINT64                    ExchangeValue
;   );
;------------------------------------------------------------------------------
InternalSyncCompareExchange64   PROC    USES    esi ebx
    mov     esi, [esp + 12]
    mov     eax, [esp + 16]
    mov     edx, [esp + 20]
    mov     ebx, [esp + 24]
    mov     ecx, [esp + 28]
    lock    cmpxchg8b   qword ptr [esi]
    ret
InternalSyncCompareExchange64   ENDP

    END
