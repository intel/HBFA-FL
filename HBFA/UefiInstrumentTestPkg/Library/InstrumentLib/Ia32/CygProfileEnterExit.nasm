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
    push ebp
    mov ebp, esp

    push eax
    push ecx
    push edx
    push ebx
    mov  eax, ebp
    add  eax, 4         ; the caller to __cyg_profile_func_enter
    push eax            ; original esp
    mov  eax, [ebp]
    push eax            ; original ebp
    push esi
    push edi
    ; prolog - end

    mov  eax, [ebp + 8]  ; eax = func address
    mov  ecx, esp        ; ecx = context
    mov  edx, [ebp + 12] ; edx = caller address
    push eax
    push edx
    push ecx
    call ASM_PFX(CygProfileEnter)
    add  esp, 12

    ; epilog - begin
    pop  edi
    pop  esi
    add  esp, 4   ; skip ebp
    add  esp, 4   ; skip esp
    pop  ebx
    pop  edx
    pop  ecx
    pop  eax
    mov esp, ebp
    pop ebp
    ; epilog - end
    ret

; void __cyg_profile_func_exit  (void *this_fn, void *call_site)
global ASM_PFX(__cyg_profile_func_exit)
ASM_PFX(__cyg_profile_func_exit):
    ; prolog - begin
    push ebp
    mov ebp, esp

    push eax
    push ecx
    push edx
    push ebx
    mov  eax, ebp
    add  eax, 4         ; the caller to __cyg_profile_func_exit
    push eax            ; original esp
    mov  eax, [ebp]
    push eax            ; original ebp
    push esi
    push edi
    ; prolog - end

    mov  eax, [ebp + 8]  ; eax = func address
    mov  ecx, esp        ; ecx = context
    mov  edx, [ebp + 12] ; edx = caller address
    push eax
    push edx
    push ecx
    call ASM_PFX(CygProfileExit)
    add  esp, 12

    ; epilog - begin
    pop  edi
    pop  esi
    add  esp, 4   ; skip ebp
    add  esp, 4   ; skip esp
    pop  ebx
    pop  edx
    pop  ecx
    pop  eax
    mov esp, ebp
    pop ebp
    ; epilog - end
    ret
