;------------------------------------------------------------------------------ ;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
;------------------------------------------------------------------------------

extern ASM_PFX(CygProfileEnter)
extern ASM_PFX(CygProfileExit)

    SECTION .text

; void __cyg_profile_func_enter (void *this_fn, void *call_site)
global ASM_PFX(__cyg_profile_func_enter)
ASM_PFX(__cyg_profile_func_enter):
    ; prolog - begin
    push rbp
    mov rbp, rsp

    push rax
    push rcx
    push rdx
    push rbx
    mov  rax, rbp
    add  rax, 8         ; the caller to __cyg_profile_func_enter
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

    mov  r8, rdi        ; r8 = func address
    mov  rcx, rsp       ; rcx = context
    mov  rdx, rsi       ; rdx = caller address
    sub  rsp, 0x20
    call ASM_PFX(CygProfileEnter)
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
    pop  rax
    mov rsp, rbp
    pop rbp
    ; epilog - end
    ret

; void __cyg_profile_func_exit  (void *this_fn, void *call_site)
global ASM_PFX(__cyg_profile_func_exit)
ASM_PFX(__cyg_profile_func_exit):
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

    mov  r8, rdi        ; r8 = func address
    mov  rcx, rsp       ; rcx = context
    mov  rdx, rsi       ; rdx = caller address
    sub  rsp, 0x20
    call ASM_PFX(CygProfileExit)
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
    pop  rax
    mov rsp, rbp
    pop rbp
    ; epilog - end
    ret
