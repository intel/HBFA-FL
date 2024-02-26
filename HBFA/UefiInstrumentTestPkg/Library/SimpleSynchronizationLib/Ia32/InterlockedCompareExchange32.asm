;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   InterlockedCompareExchange32.Asm
;
; Abstract:
;
;   InterlockedCompareExchange32 function
;
; Notes:
;
;------------------------------------------------------------------------------

    .486
    .model  flat,C
    .code

;------------------------------------------------------------------------------
; UINT32
; EFIAPI
; InternalSyncCompareExchange32 (
;   IN      volatile UINT32           *Value,
;   IN      UINT32                    CompareValue,
;   IN      UINT32                    ExchangeValue
;   );
;------------------------------------------------------------------------------
InternalSyncCompareExchange32   PROC
    mov     ecx, [esp + 4]
    mov     eax, [esp + 8]
    mov     edx, [esp + 12]
    lock    cmpxchg [ecx], edx
    ret
InternalSyncCompareExchange32   ENDP

    END
