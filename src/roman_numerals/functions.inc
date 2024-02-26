.code

number_to_roman proc
    
    buffer     equ dword ptr[ebp-14h]
    buffer_len equ dword ptr[ebp-4]
    
    ;------
    push ebp
    mov ebp,esp
    add esp, -16
    ;------
    push ebx
    push esi
    push edi
    ;------
    mov buffer_len, 16
    
    ;------
    lea esi, buffer
    push buffer_len
    push esi
    call clear_buffer
    
    push 1
    push 0
    push buffer_len
    push esi
    call set_digit
    
    printf("number: %s\n", esi)

@@ret:
    ;-----
    pop edi
    pop esi
    pop ebx
    ;-----
    mov esp, ebp
    pop ebp
    
	ret
number_to_roman endp


set_digit proc; lpBuffer, lSize, lPos, lNumber -> new pos
    lpBuffer equ dword ptr[ebp+08h]
    lSize    equ dword ptr[ebp+0Ch]
    lPos     equ dword ptr[ebp+10h]
    lNumber  equ dword ptr[ebp+14h]
    
    ;-----
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    ;-----
    
    mov ebx, lNumber
    cmp ebx, 1
    jnz @F
    
    mov lPos, append$(lpBuffer, "I", lPos)
    mov eax, lPos
    jmp @@ret
    
    
@@:
    
    
@@ret:
    pop edi
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp    

	ret 16
set_digit endp


clear_buffer proc; lpBuffer, usize
    lpBuffer    equ dword ptr[ebp+8]
    lSize       equ dword ptr[ebp+12]
    
    ;-------
    push ebp
    mov ebp, esp
    ;-------
    push ebx
    push esi
    push edi
    ;-------
    
    mov ebx, lSize
    mov esi, lpBuffer
    
@@:
    dec ebx
    mov byte ptr[esi+ebx], 0
    and ebx, ebx
    jnz @B

    ;-------
@@ret:
    ;-------
    pop edi
    pop esi
    pop ebx
    ;-------
    mov esp, ebp
    pop ebp
    ;-------
	ret 8
clear_buffer endp

