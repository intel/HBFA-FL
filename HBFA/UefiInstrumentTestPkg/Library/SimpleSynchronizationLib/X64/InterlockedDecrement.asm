;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   InterlockedDecrement.Asm
;
; Abstract:
;
;   InterlockedDecrement function
;
; Notes:
;
;------------------------------------------------------------------------------

    .code

;------------------------------------------------------------------------------
; UINT32
; EFIAPI
; InternalSyncDecrement (
;   IN      volatile UINT32           *Value
;   );
;------------------------------------------------------------------------------
InternalSyncDecrement   PROC
    lock    dec     dword ptr [rcx]
    mov     eax, [rcx]
    ret
InternalSyncDecrement   ENDP

    END
