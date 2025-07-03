section .text
global branchTorture

; Branch prediction torture - realistic patterns from real applications
; rdi = iterations, rsi = pattern_type
branchTorture:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    mov r15, rdi                   ; iterations
    mov r14, rsi                   ; pattern type
    
    ; Initialize counters and data
    xor rax, rax                   ; result accumulator
    mov rbx, 1                     ; counter
    mov rcx, 0x12345678            ; pseudo-random seed
    
    ; Jump to specific pattern based on type
    cmp r14, 1
    je .gaming_pattern
    cmp r14, 2  
    je .database_pattern
    cmp r14, 3
    je .compiler_pattern
    jmp .mixed_pattern
    
.gaming_pattern:
.game_loop:
    ; Generate pseudo-random number
    mov r12, rcx
    imul r12, 1103515245
    add r12, 12345
    mov rcx, r12
    
    ; Branch pattern 1: Deeper, more complex random decisions
    test r12, 0x1
    jz .ai_path_a
    test r12, 0x2
    jz .ai_path_b
    test r12, 0x4
    jz .ai_path_c
    jmp .ai_path_d
    
.ai_path_a:
    add rax, rbx
    test r12, 0x8
    jz .game_next
    sub rax, 1
    jmp .game_next
.ai_path_b:
    sub rax, rbx
    test r12, 0x10
    jz .game_next
    add rax, 1
    jmp .game_next
.ai_path_c:
    xor rax, rbx
    test r12, 0x20
    jz .game_next
    not rax
    jmp .game_next
.ai_path_d:
    imul rax, rbx
    test r12, 0x40
    jz .game_next
    neg rax
    
.game_next:
    inc rbx
    dec r15
    jnz .game_loop
    jmp .done
    
.database_pattern:
.db_loop:
    ; Generate pseudo-random number
    mov r12, rcx
    imul r12, 1664525
    add r12, 1013904223
    mov rcx, r12
    
    ; Deeper nested WHERE conditions
    mov r11, r12
    and r11, 0xFF
    cmp r11, 128
    jle .db_skip_age
    
    mov r10, r12
    shr r10, 8
    and r10, 0xFF
    cmp r10, 64
    jle .db_skip_salary
    
    mov r9, r12
    shr r9, 16
    and r9, 0xFF
    cmp r9, 32
    jle .db_skip_location
    
    ; All conditions met
    add rax, r11
    sub rax, r10
    xor rax, r9
    jmp .db_next
    
.db_skip_age:
    add rax, 1
    cmp r11, 64
    jle .db_next
    sub rax, 1
    jmp .db_next
.db_skip_salary:
    add rax, 2
    cmp r10, 32
    jle .db_next
    sub rax, 2
    jmp .db_next
.db_skip_location:
    add rax, 3
    cmp r9, 16
    jle .db_next
    sub rax, 3
    
.db_next:
    inc rbx
    dec r15
    jnz .db_loop
    jmp .done
    
.compiler_pattern:
.compiler_loop:
    ; Generate pseudo-random number
    mov r12, rcx
    imul r12, 69069
    add r12, 1
    mov rcx, r12
    
    ; Deeper token type decision (switch-like pattern)
    mov r11, r12
    and r11, 0xF
    
    cmp r11, 0
    je .token_identifier
    cmp r11, 1
    je .token_number
    cmp r11, 2
    je .token_string
    cmp r11, 3
    je .token_operator
    cmp r11, 4
    je .token_keyword
    cmp r11, 5
    je .token_comment
    jmp .token_default
    
.token_identifier:
    add rax, rbx
    test r12, 0x100
    jz .compiler_next
    sub rax, 1
    jmp .compiler_next
.token_number:
    sub rax, rbx
    test r12, 0x200
    jz .compiler_next
    add rax, 1
    jmp .compiler_next
.token_string:
    xor rax, rbx
    test r12, 0x400
    jz .compiler_next
    not rax
    jmp .compiler_next
.token_operator:
    imul rax, 3
    test r12, 0x800
    jz .compiler_next
    neg rax
    jmp .compiler_next
.token_keyword:
    add rax, 0x123
    jmp .compiler_next
.token_comment:
    sub rax, 0x123
    jmp .compiler_next
.token_default:
    neg rax
    
.compiler_next:
    inc rbx
    dec r15
    jnz .compiler_loop
    jmp .done
    
.mixed_pattern:
.mixed_loop:
    ; Generate pseudo-random number
    mov r12, rcx
    imul r12, 214013
    add r12, 2531011
    mov rcx, r12
    
    ; Rotate between different patterns based on the random number
    mov r11, r12
    and r11, 0x3
    
    cmp r11, 0
    je .mixed_random
    cmp r11, 1
    je .mixed_nested
    cmp r11, 2
    je .mixed_switch
    jmp .mixed_loop_pattern
    
.mixed_random:
    test r12, 0x10
    jz .rand_a
    add rax, r12
    test rax, 0x1
    jz .mixed_next
    sub rax, rbx
    jmp .mixed_next
.rand_a:
    sub rax, r12
    test rax, 0x2
    jz .mixed_next
    add rax, rbx
    jmp .mixed_next
    
.mixed_nested:
    cmp r12, 0x80000000
    jae .nest_high
    cmp r12, 0x40000000
    jae .nest_mid
    add rax, 1
    cmp rax, rbx
    jle .mixed_next
    sub rax, 1
    jmp .mixed_next
.nest_mid:
    add rax, 2
    cmp rax, rbx
    jle .mixed_next
    sub rax, 2
    jmp .mixed_next
.nest_high:
    add rax, 3
    cmp rax, rbx
    jle .mixed_next
    sub rax, 3
    jmp .mixed_next
    
.mixed_switch:
    mov r10, r12
    and r10, 0xF
    cmp r10, 0
    je .sw_0
    cmp r10, 1
    je .sw_1
    cmp r10, 2
    je .sw_2
    jmp .sw_default
.sw_0:
    shl rax, 1
    jmp .mixed_next
.sw_1:
    shr rax, 1
    jmp .mixed_next
.sw_2:
    rol rax, 1
    jmp .mixed_next
.sw_default:
    xor rax, r12
    jmp .mixed_next
    
.mixed_loop_pattern:
    ; Simple loop pattern
    add rax, rbx
    cmp rax, r12
    jle .mixed_next
    neg rax
    
.mixed_next:
    inc rbx
    dec r15
    jnz .mixed_loop
    
.done:
    ; Store result to prevent optimization
    mov [rsp-8], rax
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret