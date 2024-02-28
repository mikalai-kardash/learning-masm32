roman_to_number       proto :dword,:dword
get_number            proto :dword
compare_roman_numbers proto :dword,:dword
clear_buffer          proto :dword,:dword
set_thousands         proto :dword,:dword,:dword,:dword
set_hundreds          proto :dword,:dword,:dword,:dword
set_tens              proto :dword,:dword,:dword,:dword
set_digit             proto :dword,:dword,:dword,:dword
division              proto :dword,:dword,:dword

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


verify macro variable, romanNumber, number

@@: cmp variable, romanNumber
    jne @F
    mov eax, number
    jmp @@ret
    
endm


get_number proc roman:dword
    xor eax, eax
    
    verify roman, 'M', 1000
    verify roman, 'D', 500
    verify roman, 'C', 100
    verify roman, 'L', 50
    verify roman, 'X', 10
    verify roman, 'V', 5
    verify roman, 'I', 1
    
@@:
@@ret:
	ret 4
get_number endp


;-------------------------------------------------
;   Iterating over range of digits 1..3999.
;
;   1. Sequentially converts number to roman 
;      numeral (string buffer).
;   2. Converts string numeral back to digit form.
;
;-------------------------------------------------
number_to_roman proc uses ebx
    ;--------------
    local buffer[16]:byte
    local buffer_len:dword
    local pos:dword
    local remainder:dword
    local number:dword
    ;--------------

    mov buffer_len, 16
    mov ebx, 0

@@number:
    inc ebx
    ;--------------
    mov pos, 0
    mov number, ebx
    ;--------------
    fn clear_buffer, addr buffer, buffer_len
    
    ; divide by 1000
    mov number, rv(division, number, 1000, addr remainder)
    cmp number, 0
    jz @f
   
    ; decode number to roman representation if > 0
    mov pos, rv(set_thousands, addr buffer, buffer_len, pos, number)
    
@@: 
    push remainder
    pop number
    
    ; divide by 100
    mov number, rv(division, number, 100, addr remainder)
    cmp number, 0
    jz @f
    
    ; decode number to roman representation if > 0
    mov pos, rv(set_hundreds, addr buffer, buffer_len, pos, number)
    
    
@@: 
    push remainder
    pop number
    
    ;divide by 10
    mov number, rv(division, number, 10, addr remainder)
    cmp number, 0
    jz @f
    
    ; decode number to roman representation if > 0
    mov pos, rv(set_tens, addr buffer, buffer_len, pos, number)
    
    
@@: 
    ; remainder is the number
    ; decode it to roman representation if > 0
    mov pos, rv(set_digit, addr buffer, buffer_len, pos, remainder)
    
    ; buffer is containing roman number now
    ; reverse roman representation to number
    mov number, rv(roman_to_number, addr buffer, buffer_len)
    
    ; compare ebx (original number) with 
    ; decoded-encoded value (number > roman numeral > number)
    cmp ebx, number
    jz @@equal


@@not_equal:
    ; display text suggesting something went wrong
    printf("expected '%d', but got '%d' [%s]\n", ebx, number, addr buffer)
    jmp @@number_end

@@equal:
    ; display number and its roman numeral representation
    printf("%d = %s\n", number, addr buffer)

@@number_end:
    cmp ebx, 3999
    jne @@number

@@ret:
	ret
number_to_roman endp


;------------------------------------------
;
;    Divides the number. Result is returned
;    via eax. Remainder is returned via 
;    pointer to remainder variable (pRemainder).
;
;    - number     : number to divide
;    - divider    : divisor, will devide number by it
;    - pRemainder : address of variable that 
;                   is going to receive remainder value
;
;------------------------------------------
division proc uses ebx esi number:dword, divider:dword, pRemainder:dword
    mov eax, number
    xor edx, edx
    mov ebx, divider
    div ebx
    
    ; transfer remainer to address
    mov esi, pRemainder
    mov [esi], edx

@@ret:
	ret 12
division endp


;------------------------------------------
;
;    Appends roman literal to a buffer
;    if number matches.
;
;    - number   : arabic digit
;    - roman    : roman literal (ex.: "I", "V", etc.)
;    - pBuffer  : address of string buffer
;    - position : position at which to append 
;                 roman literal
;
;------------------------------------------
append_matching_digit macro number, roman, pBuffer, position

@@: cmp ebx, number
    jnz @F
    test eax, append$(pBuffer, roman, position)
    jmp @@ret

endm


;------------------------------------------
;
;   Appends roman numeral representing a
;   thousand if number matches.
;
;   - lpBuffer: address of a string buffer
;   - lSize   : number of bytes in buffer
;   - lPos    : position in a buffer where new
;               symbols can be appended
;   - lNumber : number of thousands            
;
;------------------------------------------
set_thousands proc uses ebx lpBuffer:dword, lSize:dword, lPos:dword, lNumber:dword
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret
    
    ; TODO: use lSize to avoid 
    ;       overflowing the buffer (lpBuffer).
    append_matching_digit 1, "M",   lpBuffer, lPos
    append_matching_digit 2, "MM",  lpBuffer, lPos
    append_matching_digit 3, "MMM", lpBuffer, lPos
   
@@:  
@@ret:
	ret 16
set_thousands endp


set_hundreds proc uses ebx lpBuffer:dword, lSize:dword, lPos:dword, lNumber:dword
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret

    ; TODO: use lSize to avoid 
    ;       overflowing the buffer (lpBuffer).
    append_matching_digit 1, "C", lpBuffer, lPos
    append_matching_digit 2, "CC", lpBuffer, lPos
    append_matching_digit 3, "CCC", lpBuffer, lPos
    append_matching_digit 4, "CD", lpBuffer, lPos
    append_matching_digit 5, "D", lpBuffer, lPos
    append_matching_digit 6, "DC", lpBuffer, lPos
    append_matching_digit 7, "DCC", lpBuffer, lPos
    append_matching_digit 8, "DCCC", lpBuffer, lPos
    append_matching_digit 9, "CM", lpBuffer, lPos


@@:  
@@ret:
	ret 16
set_hundreds endp


set_tens proc uses ebx lpBuffer:dword, lSize:dword, lPos:dword, lNumber:dword
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret
    
    ; TODO: use lSize to avoid 
    ;       overflowing the buffer (lpBuffer).
    append_matching_digit 1, "X", lpBuffer, lPos
    append_matching_digit 2, "XX", lpBuffer, lPos
    append_matching_digit 3, "XXX", lpBuffer, lPos
    append_matching_digit 4, "XL", lpBuffer, lPos
    append_matching_digit 5, "L", lpBuffer, lPos
    append_matching_digit 6, "LX", lpBuffer, lPos
    append_matching_digit 7, "LXX", lpBuffer, lPos
    append_matching_digit 8, "LXXX", lpBuffer, lPos
    append_matching_digit 9, "XC", lpBuffer, lPos
    
@@:  
@@ret:
    ret 16
set_tens endp


set_digit proc uses ebx lpBuffer:dword, lSize:dword, lPos:dword, lNumber:dword
    mov ebx, lNumber
    ;-----
    cmp ebx, 0
    jz @@ret

    ; TODO: use lSize to avoid 
    ;       overflowing the buffer (lpBuffer).
    append_matching_digit 1, "I", lpBuffer, lPos
    append_matching_digit 2, "II", lpBuffer, lPos
    append_matching_digit 3, "III", lpBuffer, lPos
    append_matching_digit 4, "IV", lpBuffer, lPos
    append_matching_digit 5, "V", lpBuffer, lPos
    append_matching_digit 6, "VI", lpBuffer, lPos
    append_matching_digit 7, "VII", lpBuffer, lPos
    append_matching_digit 8, "VIII", lpBuffer, lPos
    append_matching_digit 9, "IX", lpBuffer, lPos
    
@@:  
@@ret:
	ret 16
set_digit endp


;------------------------------------------
;
;   Set all symbols in buffer to zero. It
;   is working on 16 bytes at once.
;
;   - lpBuffer: address of a string buffer
;   - lSize   : number of bytes in buffer
;               should be power of 16
;
;------------------------------------------
clear_buffer proc uses ebx esi lpBuffer:dword, lSize:dword
    mov ebx, lSize
    shr ebx, 4
    mov esi, lpBuffer
    mov eax, 0
    
@@:
    mov [esi], eax
    mov [esi+4], eax
    mov [esi+8], eax
    mov [esi+12], eax

    add esi, 16
    dec ebx
    jnz @B

@@ret:
	ret 8
clear_buffer endp
