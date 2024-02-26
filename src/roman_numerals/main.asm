include main.inc

.data
    szRomanNumber db 16 dup(0)
    dwRomanNumberSize dd 16

.code
start:
    call main
    ;-------------
    push eax
    call ExitProcess

;======================

main proc

    ;-------------
    fn number_to_roman
    ;fn roman_to_number,chr$("IX"),3
    ;-------------
    inkey
    ;-------------
    
    xor eax, eax
	ret
main endp
    
end start