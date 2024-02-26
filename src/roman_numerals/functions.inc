roman_to_number proto :dword,:dword
get_number proto :dword
compare_roman_numbers proto :dword,:dword

.code

roman_to_number proc uses ebx esi edi lpBuffer:dword, nsize:dword
    LOCAL current:dword
    LOCAL prev:dword
    LOCAL result:dword
    LOCAL number_of_chars:dword
    
    and ebx, 0
    mov esi, lpBuffer
    and result, 0
    and number_of_chars, 0

  
@@:
    cmp byte ptr[esi+ebx], 0
    jz @F
    movzx eax, byte ptr[esi+ebx]
    push eax
    
    inc ebx
    cmp ebx, nsize
    je @F
    jne @B 
    

@@: 
    mov number_of_chars, ebx
    dec number_of_chars
    
    
    pop prev
    add result, rv(get_number,prev)
    
    cmp number_of_chars, 0
    jz @@ret
    
@@loop: 
    dec number_of_chars
    pop current
    mov eax, rv(compare_roman_numbers,current,prev)
    cmp eax, -1
    je @@sub
    jmp @@add

@@sub:
    sub result, rv(get_number,current)
    jmp @@loop_end
    
@@add:
    add result, rv(get_number,current)

@@loop_end:
    push current
    pop prev
    cmp number_of_chars, 0
    je @F   
    jmp @@loop
    
@@:
@@ret:
    mov eax, result
	ret 8
roman_to_number endp


compare_roman_numbers proc a:dword, b:dword
    local a_result:dword
    local b_result:dword
    
    mov a_result, rv(get_number,a)
    mov b_result, rv(get_number,b)
    mov eax, a_result
    cmp eax, b_result
    jg @@more
    jl @@less
    jmp @@eq

@@less:
    mov eax, -1
    jmp @@ret
    
@@more:
    mov eax, 1
    jmp @@ret
    
@@eq:
    mov eax, 0
    jmp @@ret
    
@@ret:    
	ret 8
compare_roman_numbers endp


get_number proc roman:dword
    xor eax, eax
    
@@: cmp roman, 'M'
    jne @F
    mov eax, 1000
    jmp @@ret
    
@@: cmp roman, 'D'
    jne @F
    mov eax, 500
    jmp @@ret
    
@@: cmp roman, 'C'
    jne @F
    mov eax, 100
    jmp @@ret
    
@@: cmp roman, 'L'
    jne @F
    mov eax, 50
    jmp @@ret
    
@@: cmp roman, 'X'
    jne @F
    mov eax, 10
    jmp @@ret
    
@@: cmp roman, 'V'
    jne @F
    mov eax, 5
    jmp @@ret
    
@@: cmp roman, 'I'
    jne @F
    mov eax, 1
    jmp @@ret
    
@@:
@@ret:
	ret 4
get_number endp

number_to_roman proc uses ebx ecx edx esi
    local buffer[16]:byte
    local buffer_len:dword
    local pos:dword
    
    mov buffer_len, 16
    mov ebx, 0
    
@@number: 
    inc ebx
    lea esi, buffer
    push buffer_len
    push esi
    ;------
    call clear_buffer
    mov pos, 0
    ;------
    
    mov eax, ebx
    xor edx, edx
    mov ecx, 1000
    div ecx
    
    cmp eax, 0
    jz @F
    
    push eax
    push pos
    push buffer_len
    push esi
    call set_thousands
    mov pos, eax
    
    
    
@@: mov eax, edx
    xor edx, edx
    mov ecx, 100
    div ecx
    
    cmp eax, 0
    jz @F
    
    push eax
    push pos
    push buffer_len
    push esi
    call set_hundreds
    mov pos, eax
    
    
@@: mov eax, edx
    xor edx, edx
    mov ecx, 10
    div ecx
    
    cmp eax, 0
    jz @F
    
    push eax
    push pos
    push buffer_len
    push esi
    call set_tens
    mov pos, eax
    
@@: mov eax, edx
    
    push eax
    push pos
    push buffer_len
    push esi
    call set_digit
    
    
    ;printf("%s\n", esi)
    mov eax, rv(roman_to_number,esi,16)
    cmp eax, ebx
    jz @@equal
    
@@not_equal:
    printf("expected '%d', but got '%d' [%s]\n", ebx, eax, esi)
    jmp @@number_end

@@equal:
    printf("%d = %s\n", eax, esi)

@@number_end:
    cmp ebx, 3999
    jne @@number

@@ret:
	ret
number_to_roman endp


set_thousands proc; lpBuffer, lSize, lPos, lNumber -> new pos
    lpBuffer equ dword ptr[ebp+08h]
    lSize    equ dword ptr[ebp+0Ch]
    lPos     equ dword ptr[ebp+10h]
    lNumber  equ dword ptr[ebp+14h]
    
    ;-----
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi
    push edi
    ;-----
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret
    
    
@@: cmp ebx, 1
    jnz @F
    
    and eax, append$(lpBuffer, "M", lPos)
    jmp @@ret


@@: cmp ebx, 2
    jnz @F
    
    and eax, append$(lpBuffer, "MM", lPos)
    jmp @@ret


@@: cmp ebx, 3
    jnz @F
    
    and eax, append$(lpBuffer, "MMM", lPos)
    jmp @@ret
    

@@:  
@@ret:
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp    

	ret 16
set_thousands endp


set_hundreds proc; lpBuffer, lSize, lPos, lNumber -> new pos
    lpBuffer equ dword ptr[ebp+08h]
    lSize    equ dword ptr[ebp+0Ch]
    lPos     equ dword ptr[ebp+10h]
    lNumber  equ dword ptr[ebp+14h]
    
    ;-----
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi
    push edi
    ;-----
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret


@@: cmp ebx, 1
    jnz @F
    
    and eax, append$(lpBuffer, "C", lPos)
    jmp @@ret


@@: cmp ebx, 2
    jnz @F
    
    and eax, append$(lpBuffer, "CC", lPos)
    jmp @@ret


@@: cmp ebx, 3
    jnz @F
    
    and eax, append$(lpBuffer, "CCC", lPos)
    jmp @@ret
    

@@: cmp ebx, 4
    jnz @F
    
    and eax, append$(lpBuffer, "CD", lPos)
    jmp @@ret

   
@@: cmp ebx, 5
    jnz @F
    
    and eax, append$(lpBuffer, "D", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 6
    jnz @F
    
    and eax, append$(lpBuffer, "DC", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 7
    jnz @F
    
    and eax, append$(lpBuffer, "DCC", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 8
    jnz @F
    
    and eax, append$(lpBuffer, "DCCC", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 9
    jnz @F
    
    and eax, append$(lpBuffer, "CM", lPos)
    jmp @@ret


@@:  
@@ret:
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp    

	ret 16
set_hundreds endp


set_tens proc; lpBuffer, lSize, lPos, lNumber -> new pos
    lpBuffer equ dword ptr[ebp+08h]
    lSize    equ dword ptr[ebp+0Ch]
    lPos     equ dword ptr[ebp+10h]
    lNumber  equ dword ptr[ebp+14h]
    
    ;-----
    push ebp
    mov ebp, esp
    push ebx
    push edx
    push esi
    push edi
    ;-----
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret
    
    
@@: cmp ebx, 1
    jnz @F
    
    and eax, append$(lpBuffer, "X", lPos)
    jmp @@ret


@@: cmp ebx, 2
    jnz @F
    
    and eax, append$(lpBuffer, "XX", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 3
    jnz @F
    
    and eax, append$(lpBuffer, "XXX", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 4
    jnz @F
    
    and eax, append$(lpBuffer, "XL", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 5
    jnz @F
    
    and eax, append$(lpBuffer, "L", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 6
    jnz @F
    
    and eax, append$(lpBuffer, "LX", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 7
    jnz @F
    
    and eax, append$(lpBuffer, "LXX", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 8
    jnz @F
    
    and eax, append$(lpBuffer, "LXXX", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 9
    jnz @F
    
    and eax, append$(lpBuffer, "XC", lPos)
    jmp @@ret
    
@@:  
@@ret:
    pop edi
    pop esi
    pop edx
    pop ebx
    mov esp, ebp
    pop ebp    

	ret 16
set_tens endp


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
    ;-----
    cmp ebx, 0
    jz @@ret
    
    
@@: cmp ebx, 1
    jnz @F
    
    and eax, append$(lpBuffer, "I", lPos)
    jmp @@ret


@@: cmp ebx, 2
    jnz @F
    
    and eax, append$(lpBuffer, "II", lPos)
    jmp @@ret
    

@@: cmp ebx, 3
    jnz @F
    
    and eax, append$(lpBuffer, "III", lPos)
    jmp @@ret
    

@@: cmp ebx, 4
    jnz @F
    
    and eax, append$(lpBuffer, "IV", lPos)
    jmp @@ret
    
@@: cmp ebx, 5
    jnz @F
    
    and eax, append$(lpBuffer, "V", lPos)
    jmp @@ret
    

@@: cmp ebx, 6
    jnz @F
    
    and eax, append$(lpBuffer, "VI", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 7
    jnz @F
    
    and eax, append$(lpBuffer, "VII", lPos)
    jmp @@ret


@@: cmp ebx, 8
    jnz @F
    
    and eax, append$(lpBuffer, "VIII", lPos)
    jmp @@ret
    
    
@@: cmp ebx, 9
    jnz @F
    
    and eax, append$(lpBuffer, "IX", lPos)
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

