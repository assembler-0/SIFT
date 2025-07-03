section .text
global cacheL1Test, cacheL2Test, cacheL3Test, memoryLatencyTest

; L1 Cache Test - 32KB working set (fits in L1)
; rdi = iterations, rsi = buffer
cacheL1Test:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    
    mov r12, rdi                   ; iterations
    mov r13, rsi                   ; buffer
    mov rax, 0xdeadbeefcafebabe
    
.l1_loop:
    ; L1 cache friendly access pattern (32KB working set)
    mov rbx, 0                     ; offset
    mov rcx, 4096                  ; 32KB / 8 bytes = 4096 iterations
    
.l1_inner:
    mov [r13 + rbx], rax           ; Write to L1
    mov rdx, [r13 + rbx]           ; Read from L1 (should hit)
    add rax, rdx                   ; Use the data
    add rbx, 8                     ; Next 8 bytes
    dec rcx
    jnz .l1_inner
    
    dec r12
    jnz .l1_loop
    
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

; L2 Cache Test - 512KB working set (fits in L2, misses L1)
; rdi = iterations, rsi = buffer
cacheL2Test:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    
    mov r12, rdi                   ; iterations
    mov r13, rsi                   ; buffer
    mov rax, 0xbaadf00dcafebabe
    
.l2_loop:
    ; L2 cache access pattern (512KB working set)
    mov rbx, 0                     ; offset
    mov rcx, 65536                 ; 512KB / 8 bytes = 65536 iterations
    
.l2_inner:
    mov [r13 + rbx], rax           ; Write to L2
    mov rdx, [r13 + rbx]           ; Read from L2 (L1 miss, L2 hit)
    add rax, rdx                   ; Use the data
    add rbx, 8                     ; Next 8 bytes
    dec rcx
    jnz .l2_inner
    
    dec r12
    jnz .l2_loop
    
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

; L3 Cache Test - 8MB working set (fits in L3, misses L1/L2)
; rdi = iterations, rsi = buffer
cacheL3Test:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    
    mov r12, rdi                   ; iterations
    mov r13, rsi                   ; buffer
    mov rax, 0xfeedface87654321
    
.l3_loop:
    ; L3 cache access pattern (8MB working set)
    mov rbx, 0                     ; offset
    mov rcx, 1048576               ; 8MB / 8 bytes = 1048576 iterations
    
.l3_inner:
    mov [r13 + rbx], rax           ; Write to L3
    mov rdx, [r13 + rbx]           ; Read from L3 (L1/L2 miss, L3 hit)
    add rax, rdx                   ; Use the data
    add rbx, 8                     ; Next 8 bytes
    dec rcx
    jnz .l3_inner
    
    dec r12
    jnz .l3_loop
    
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

; Memory Latency Test - Random access pattern (cache hostile)
; rdi = iterations, rsi = buffer, rdx = buffer_size
memoryLatencyTest:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    mov r12, rdi                   ; iterations
    mov r13, rsi                   ; buffer
    mov r14, rdx                   ; buffer_size
    mov r15, 1103515245            ; LCG multiplier
    mov rax, 12345                 ; LCG seed
    
.latency_loop:
    mov rbx, r12                   ; Use iteration as part of randomness
    
    ; Generate pseudo-random offset
    imul rax, r15                  ; LCG: a * seed
    add rax, rbx                   ; + iteration
    mov rcx, rax
    and rcx, 0x7FFFFFFF            ; Keep positive
    xor rdx, rdx
    div r14                        ; rdx = offset % buffer_size
    and rdx, 0xFFFFFFF8            ; Align to 8 bytes
    
    ; Random memory access (cache hostile)
    mov r8, [r13 + rdx]            ; Random read (likely cache miss)
    add r8, rax                    ; Use the data
    mov [r13 + rdx], r8            ; Random write
    
    ; Generate another random offset
    imul rax, r15
    add rax, r8
    mov rcx, rax
    and rcx, 0x7FFFFFFF
    xor rdx, rdx
    div r14
    and rdx, 0xFFFFFFF8
    
    ; Another random access
    mov r9, [r13 + rdx]            ; Another random read
    xor r9, r8                     ; Use both values
    mov [r13 + rdx], r9            ; Another random write
    
    ; Chain dependency to prevent optimization
    add rax, r9
    
    dec r12
    jnz .latency_loop
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret