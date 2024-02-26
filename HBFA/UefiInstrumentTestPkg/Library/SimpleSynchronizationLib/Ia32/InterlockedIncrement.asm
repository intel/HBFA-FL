;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   InterlockedIncrement.Asm
;
; Abstract:
;
;   InterlockedIncrement function
;
; Notes:
;
;------------------------------------------------------------------------------

    .386
    .model  flat,C
    .code

;------------------------------------------------------------------------------
; UINT32
; EFIAPI
; InternalSyncIncrement (
;   IN      volatile UINT32           *Value
;   );
;------------------------------------------------------------------------------
InternalSyncIncrement   PROC
    mov     eax, [esp + 4]
    lock    inc     dword ptr [eax]
    mov     eax, [eax]
    ret
InternalSyncIncrement   ENDP

    END
