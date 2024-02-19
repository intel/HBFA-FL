;------------------------------------------------------------------------------ ;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

extern ASM_PFX(Cpenter)
extern ASM_PFX(Cpexit)

%define PARAM_COUNT  14
%define RSVD_COUNT   16

    SECTION .text

; void _cdecl _penter( void )
global ASM_PFX(_penter)
ASM_PFX(_penter):
;---------------
; P1: Context
;---------------
; P2: Ret Addr
;---------------
; R8~R15       |
;---------------
; RDI          |
;---------------
; RSI          |
;---------------
; RBP          |
;---------------
; RSP          |
;---------------
; RBX          |
;---------------
; RDX          |
;---------------
; RCX          |
;---------------
; RAX          |
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    ; prolog - begin
    push rbp
    mov rbp, rsp

    push rax
    push rcx
    push rdx
    push rbx
    mov  rax, rbp
    add  rax, 8         ; the caller to _penter
    push rax            ; original rsp
    mov  rax, [rbp]
    push rax            ; original rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; prolog - end

    mov  rcx, rsp       ; rcx = context
    mov  rdx, [rbp + 8] ; rdx = return address
    sub  rsp, 0x20
    call ASM_PFX(Cpenter)
    add  rsp, 0x20

    ; epilog - begin
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rdi
    pop  rsi
    add  rsp, 8   ; skip rbp
    add  rsp, 8   ; skip rsp
    pop  rbx
    pop  rdx
    pop  rcx

    cmp  rax, 0
    jz   Exit

; skip function

;---------------
; RAX          |
;---------------
; RBP <- RBP    |
;---------------
; Addr in FuncB |
;---------------
;                 RAX
; RSVD            RBP <- RBP
;---------------
; Addr in FuncA | <- RSP
;---------------
; Param
;---------------
    mov  rax, [rbp - 8 * 5]
    mov  rsp, rax
    mov  rax, [rbp]
    mov  [rsp - 8], rax
    mov  rax, [rbp - 8]
    mov  [rsp - 0x10], rax
    sub  rsp, 0x10
    mov  rbp, rsp
    add  rbp, 8

    pop  rax
    mov rsp, rbp
    pop rbp
    ; epilog - end
    ret

Exit:
    push  rcx
    push  rdx
    push  rsi
    push  rdi

; copy the param

;---------------
; RDI
;---------------
; RSI
;---------------
; RDX
;---------------
; RCX
;---------------
; RAX
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD (8 * RSVD_COUNT)
;---------------
; Addr in FuncA
;---------------
; Param (8 * PARAM_COUNT)
;---------------
; RDI          | <- RSP
;---------------
; RSI
;---------------
; RDX
;---------------
; RCX
;---------------
; RAX
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD (8 * RSVD_COUNT)
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    sub  rsp, (8 * PARAM_COUNT + 8 * (8 + RSVD_COUNT))
    mov  rsi, rsp
    mov  rdi, rsi
    add  rsi, (8 * PARAM_COUNT + 8 * (8 + RSVD_COUNT))
    mov  rcx, (PARAM_COUNT + 8 + RSVD_COUNT)
    rep  movsq
    sub  rbp, (8 * PARAM_COUNT + 8 * (8 + RSVD_COUNT))

    pop  rdi
    pop  rsi
    pop  rdx
    pop  rcx

    pop  rax
    mov rsp, rbp
    pop rbp
    ; epilog - end
    ret

; void _cdecl _pexit( void )
global ASM_PFX(_pexit)
ASM_PFX(_pexit):
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD
;---------------
; Addr in FuncA
;---------------
; Param (8 * PARAM_COUNT)
;---------------
; RDI
;---------------
; RSI
;---------------
; RDX
;---------------
; RCX
;---------------
; RAX
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    ; prolog - begin
    push rbp
    mov rbp, rsp

    push rax
    push rcx
    push rdx
    push rbx
    mov  rax, rbp
    add  rax, 8         ; the caller to _pexit
    push rax            ; original rsp
    mov  rax, [rbp]
    push rax            ; original rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    ; prolog - end

    mov  rcx, rsp       ; rcx = context
    mov  rdx, [rbp + 8 * (2 + RSVD_COUNT + PARAM_COUNT + 7)] ; rdx = return address
    sub  rsp, 0x20
    call ASM_PFX(Cpexit)
    add  rsp, 0x20

    ; epilog - begin
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rdi
    pop  rsi
    add  rsp, 8   ; skip rbp
    add  rsp, 8   ; skip rsp
    pop  rbx
    pop  rdx
    pop  rcx

;---------------
; RDI
;---------------
; RSI
;---------------
; RDX
;---------------
; RCX
;---------------
; RAX
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD (8 * RSVD_COUNT)
;---------------
; Addr in FuncA
;---------------
; Param (8 * PARAM_COUNT)
;---------------
; RDI
;---------------
; RSI
;---------------
; RDX
;---------------
; RCX
;---------------
; RAX
;---------------
; RBP <- RBP
;---------------
; Addr in FuncB
;---------------
; RSVD (8 * RSVD_COUNT)
;---------------
; Addr in FuncA
;---------------
; Param
;---------------
    push rcx
    push rdx
    push rsi
    push rdi
    mov  rsi, rsp
    mov  rdi, rsi
    add  rdi, (8 * PARAM_COUNT + 8 * (8 + RSVD_COUNT))
    mov  rcx, (7)
    rep  movsq
    add  rbp, (8 * PARAM_COUNT + 8 * (8 + RSVD_COUNT))
    add  rsp, (8 * PARAM_COUNT + 8 * (8 + RSVD_COUNT))

    pop  rdi
    pop  rsi
    pop  rdx
    pop  rcx

    pop  rax
    mov rsp, rbp
    pop rbp
    ; epilog - end
    ret
