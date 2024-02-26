include main.inc

.code
start:
    call main
    ;-------------
    push eax
    call ExitProcess

;======================

main proc

    printf("\n")
    ;-------------
    call number_to_roman
    ;-------------
    inkey
    ;-------------
    
    xor eax, eax
	ret
main endp
    
end start