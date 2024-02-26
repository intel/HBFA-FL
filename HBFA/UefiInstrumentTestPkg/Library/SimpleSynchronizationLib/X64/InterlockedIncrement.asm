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

    .code

;------------------------------------------------------------------------------
; UINT32
; EFIAPI
; InternalSyncIncrement (
;   IN      volatile UINT32           *Value
;   );
;------------------------------------------------------------------------------
InternalSyncIncrement   PROC
    lock    inc     dword ptr [rcx]
    mov     eax, [rcx]
    ret
InternalSyncIncrement   ENDP

    END
