section .data
    seed dd 0                    ; Seed value initialized to 0 changing this doesn't really matter.
    charset db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' ; Load characters in memory
            db 'abcdefghijklmnopqrstuvwxyz'
            db '0123456789'
    charset_len equ $ - charset  ; Calculate the length of the charset (62)
    total_bytes equ 3000         ; Total bytes to generate you can change this value

section .bss
    password resb 16             ; Reserve 16 bytes for each chunk (Saves memory in the allocator)
    total_passwords resb total_bytes  ; Reserve 3000 bytes for the full sequence

section .text
    global _start

_start:
    mov eax, 13                  ; sys_time (Linux syscall number for time)
    xor ebx, ebx                 ; Pass NULL to get current time
    int 0x80                     ; Make the syscall
    mov [seed], eax              ; Store the current time in the seed variable

    ; Generate 3000 random characters
    mov edi, total_passwords     ; Pointer to where we store the random sequence
    mov ecx, total_bytes         ; Set counter for 3000 characters

generate_random_char:
    ; Load the seed as a value in memory
    mov eax, [seed]
    
    ; Constants for LCG
    mov ebx, 1664525             ; Multiplier
    mov edx, 0                   ; Clear EDX for division
    add eax, 1013904223          ; Add increment
    mul ebx                      ; EAX = EAX * EBX (EAX now contains the result)
    mov [seed], eax              ; Update seed with the new value

    ; Get a random index into the charset
    xor edx, edx                 ; Clear EDX for division
    mov esi, charset_len         ; Length of charset
    div esi                      ; Divide EAX by charset_len, EDX = EAX % charset_len
    mov bl, [charset + edx]      ; Get the character from charset at index EDX
    
    ; Store the character in the total_passwords array
    mov [edi], bl
    inc edi                      ; Move to the next byte
    
    ; Decrement counter and loop if not finished
    loop generate_random_char

    ; Write the 3000-byte random sequence to the console
    mov eax, 4                   ; sys_write (Linux syscall number for write)
    mov ebx, 1                   ; File descriptor 1 (stdout)
    mov ecx, total_passwords     ; Pointer to the total_passwords array
    mov edx, total_bytes         ; Number of bytes to write (3000)
    int 0x80                     ; Make the syscall

    ; Exit program
    mov eax, 1                   ; sys_exit (Linux syscall number for exit)
    xor ebx, ebx                 ; Exit code 0
    int 0x80                     ; Make the syscall
