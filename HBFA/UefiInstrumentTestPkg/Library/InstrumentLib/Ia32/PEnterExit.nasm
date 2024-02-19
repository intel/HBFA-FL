;------------------------------------------------------------------------------ ;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

extern ASM_PFX(Cpenter)
extern ASM_PFX(Cpexit)

%define PARAM_COUNT  14

    SECTION .text

; void __declspec(naked) _cdecl _penter( void )
global ASM_PFX(_penter)
ASM_PFX(_penter):
;---------------
; P1: Context
;---------------
; P2: Ret Addr
;---------------
; EDI          |
;---------------
; ESI          |
;---------------
; EBP          |
;---------------
; ESP          |
;---------------
; EBX          |
;---------------
; EDX          |
;---------------
; ECX          |
;---------------
; EAX          |
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    ; prolog - begin
    push ebp
    mov ebp, esp

    push eax
    push ecx
    push edx
    push ebx
    mov  eax, ebp
    add  eax, 4         ; the caller to _penter
    push eax            ; original esp
    mov  eax, [ebp]
    push eax            ; original ebp
    push esi
    push edi
    ; prolog - end

    mov  ecx, esp       ; ecx = context
    mov  edx, [ebp + 4] ; edx = return address
    push edx
    push ecx
    call ASM_PFX(Cpenter)
    add  esp, 8

    ; epilog - begin
    pop  edi
    pop  esi
    add  esp, 4   ; skip ebp
    add  esp, 4   ; skip esp
    pop  ebx
    pop  edx
    pop  ecx

    cmp  eax, 0
    jz   Exit

; skip function

;---------------
; EAX          |
;---------------
; EBP <- EBP    | EAX
;---------------
; Addr in FuncB | EBP <- EBP
;---------------
; Addr in FuncA | <- ESP
;---------------
; Param
;---------------
    mov  eax, [ebp - 4 * 5]
    mov  esp, eax
    mov  eax, [ebp]
    mov  [esp - 4], eax
    mov  eax, [ebp - 4]
    mov  [esp - 8], eax
    sub  esp, 8
    mov  ebp, esp
    add  ebp, 4

    pop  eax
    mov esp, ebp
    pop ebp
    ; epilog - end
    ret

Exit:
    push  ecx
    push  edx
    push  esi
    push  edi

; copy the param

;---------------
; EDI
;---------------
; ESI
;---------------
; EDX
;---------------
; ECX
;---------------
; EAX
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param (4 * PARAM_COUNT)
;---------------
; EDI          | <- ESP
;---------------
; ESI
;---------------
; EDX
;---------------
; ECX
;---------------
; EAX
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    sub  esp, (4 * PARAM_COUNT + 4 * 8)
    mov  esi, esp
    mov  edi, esi
    add  esi, (4 * PARAM_COUNT + 4 * 8)
    mov  ecx, (PARAM_COUNT + 8)
    rep  movsd
    sub  ebp, (4 * PARAM_COUNT + 4 * 8)

    pop  edi
    pop  esi
    pop  edx
    pop  ecx

    pop  eax
    mov esp, ebp
    pop ebp
    ; epilog - end
    ret

; void __declspec(naked) _cdecl _pexit( void )
global ASM_PFX(_pexit)
ASM_PFX(_pexit):
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param (4 * PARAM_COUNT)
;---------------
; EDI
;---------------
; ESI
;---------------
; EDX
;---------------
; ECX
;---------------
; EAX
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    ; prolog - begin
    push ebp
    mov ebp, esp

    push eax
    push ecx
    push edx
    push ebx
    mov  eax, ebp
    add  eax, 4         ; the caller to _pexit
    push eax            ; original esp
    mov  eax, [ebp]
    push eax            ; original ebp
    push esi
    push edi
    ; prolog - end

    mov  ecx, esp       ; ecx = context
    mov  edx, [ebp + 4 * (2 + PARAM_COUNT + 7)] ; edx = return address
    push edx
    push ecx
    call ASM_PFX(Cpexit)
    add  esp, 8

    ; epilog - begin
    pop  edi
    pop  esi
    add  esp, 4   ; skip ebp
    add  esp, 4   ; skip esp
    pop  ebx
    pop  edx
    pop  ecx

;---------------
; EDI
;---------------
; ESI
;---------------
; EDX
;---------------
; ECX
;---------------
; EAX
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param (4 * PARAM_COUNT)
;---------------
; EDI
;---------------
; ESI
;---------------
; EDX
;---------------
; ECX
;---------------
; EAX
;---------------
; EBP <- EBP
;---------------
; Addr in FuncB
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    push ecx
    push edx
    push esi
    push edi
    mov  esi, esp
    mov  edi, esi
    add  edi, (4 * PARAM_COUNT + 4 * 8)
    mov  ecx, (7)
    rep  movsd
    add  ebp, (4 * PARAM_COUNT + 4 * 8)
    add  esp, (4 * PARAM_COUNT + 4 * 8)

    pop  edi
    pop  esi
    pop  edx
    pop  ecx

    pop  eax
    mov esp, ebp
    pop ebp
    ; epilog - end
    ret
