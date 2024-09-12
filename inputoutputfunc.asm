section .data
msg_overflow db "Overflow detectado", 0dh, 0ah
tam_msg equ $-msg_overflow

section .bss
result resb 16
outputbuffer resb 20

section .text

input:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    push esi

    mov esi, eax        ; Preservar o endereço do buffer de destino

    ; Ler o número do usuário
    mov eax, 3          ; syscall número 3 (sys_read)
    mov ebx, 0          ; file descriptor 0 (stdin)
    mov ecx, esi        ; endereço do buffer para armazenar a entrada
    mov edx, 12         ; número máximo de bytes a ler
    int 0x80            ; chamada de sistema

    xor ebx, ebx        ; Zerando ebx (resultado)
    mov ecx, esi        ; Resetar ecx para o início do buffer
    xor edx, edx        ; Zerando edx (flag para número negativo)

    ; Verificar se o primeiro caractere é um sinal de menos
    cmp byte [ecx], '-'
    jne next_digit
    inc ecx             ; Avançar para o próximo caractere após o sinal
    mov edx, 1          ; Definir flag de número negativo

next_digit:
    movzx eax, byte [ecx]  ; Carregar o próximo byte (caractere)
    cmp eax, 10            ; Verificar se é o caractere de nova linha
    je done                ; Se for nova linha, terminar
    sub eax, '0'           ; Converter caractere ASCII para dígito
    imul ebx, ebx, 10      ; Multiplicar o resultado atual por 10
    add ebx, eax           ; Adicionar o dígito ao resultado
    inc ecx                ; Avançar para o próximo caractere
    jmp next_digit         ; Repetir para o próximo dígito

done:
    test edx, edx          ; Verificar se o número é negativo
    jz store_result        ; Se não for negativo, armazenar o resultado
    neg ebx                ; Se for negativo, negar o resultado

store_result:
    mov [esi], ebx         ; Armazenar o resultado no endereço apontado por esi

    pop esi
    pop edx
    pop ecx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

output:
    
    push ebp                ; Salvar o valor atual do base pointer
    mov ebp, esp            ; Estabelecer o novo base pointe
    push edx                ; Salvar o valor do registrador edx
    push ecx 
    push ebx
    push esi
    push edi

    test eax, eax
    jns int_to_string
    jmp negativo
    int_to_string:
        mov ebx, 10             ; divisor 10
        xor ecx, ecx            ; limpar ecx (posição da string)
        mov edi, result         ; endereço do buffer de saida
    reverse_loop:
        xor edx, edx            ; limpar edx (restante do número)
        div ebx                 ; dividir eax por 10
        add dl, '0'             ; converter o dígito para ASCII
        mov [edi], dl           ; armazenar o dígito no buffer
        inc edi                 ; avançar no buffer
        test eax, eax           ; verificar se eax é zero
        jnz reverse_loop        ; se não for zero, continuar
        mov byte [edi], 0       ; adicionar o caractere nulo ao final
        mov eax, edi            ; endereço do fim da string
        sub eax, result         ; calcular comprimento subtraindo endereço inicial
        mov ecx, eax
        mov esi, edi
        mov edi, outputbuffer
        dec esi
    reverte_string:
        mov bl, [esi]
        mov [edi], bl
        dec esi
        inc edi
        loop reverte_string
        mov byte [edi], 0 
        jmp print
    negativo:
        neg eax
        int_to_string2:
            mov ebx, 10             ; divisor 10
            xor ecx, ecx            ; limpar ecx (posição da string)
            mov edi, result         ; endereço do buffer de saida
        reverse_loop2:
            xor edx, edx            ; limpar edx (restante do número)
            div ebx                 ; dividir eax por 10
            add dl, '0'             ; converter o dígito para ASCII
            mov [edi], dl           ; armazenar o dígito no buffer
            inc edi                 ; avançar no buffer
            test eax, eax           ; verificar se eax é zero
            jnz reverse_loop2        ; se não for zero, continuar
            mov byte [edi], 0       ; adicionar o caractere nulo ao final
            mov eax, edi            ; endereço do fim da string
            sub eax, result         ; calcular comprimento subtraindo endereço inicial
            mov ecx, eax
            mov esi, edi
            mov edi, outputbuffer
            dec esi
            mov byte [edi], '-'
            inc edi
            add eax, 1
        reverte_string2:
            mov bl, [esi]
            mov [edi], bl
            dec esi
            inc edi
            loop reverte_string2
            mov byte [edi], 0 
    print:    
        mov ecx, outputbuffer   ; endereço da string de resultado
        mov edx, eax         ; comprimento da string de resultado
        
        mov eax, 4              ; syscall número 4 (sys_write)
        mov ebx, 1              ; file descriptor 1 (stdout)
        int 80h                ; chamada de sistema
        
        pop edi
        pop esi
        pop ebx
        pop ecx
        pop edx

        mov esp, ebp            ; Restaurar o valor do stack pointer
        pop ebp                 ; Restaurar o valor original do base pointer

        ret

overflow:
        ; Rotina para lidar com overflow
        ; Aqui, podemos imprimir uma mensagem ou parar o programa

        ; Para fins de exemplo, imprimiremos uma mensagem de erro
        mov eax, 4          ; syscall número 4 (sys_write)
        mov ebx, 1          ; arquivo descriptor 1 (stdout)
        mov ecx, msg_overflow   ; ponteiro para a mensagem
        mov edx, tam_msg        ; comprimento da mensagem
        int 0x80            ; chamada de sistema
        ;stop
        mov eax, 1
        mov ebx, 0
        int 80h