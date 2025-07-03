global renderPixel
section .text

renderPixel:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    mov r15, rdi                   ; iterations
    mov r14, rsi                   ; thread_id
    
    ; Initialize brutal math constants
    cvtsi2ss xmm0, r14             ; thread_id as float
    addss xmm0, dword [rel one]    ; +1 to avoid zero
    
.main_loop:
    movss xmm1, xmm0
    mulss xmm1, dword [rel pi]     ; * pi
    movss xmm2, xmm0
    mulss xmm2, dword [rel e]      ; * e
    movss xmm3, xmm0
    mulss xmm3, dword [rel phi]    ; * golden ratio
    
    ; BRUTAL: Triple nested loops of mathematical hell
    mov r13, 50                    ; Outer torture loop
.torture_outer:
    mov r12, 20                    ; Middle torture loop
.torture_middle:
    mov r11, 10                    ; Inner torture loop
.torture_inner:
    
    ; Expensive polynomial calculations (x^7 + x^5 + x^3 + x)
    movss xmm4, xmm1               ; x
    movss xmm5, xmm4
    mulss xmm5, xmm4               ; x²
    movss xmm6, xmm5
    mulss xmm6, xmm4               ; x³
    movss xmm7, xmm6
    mulss xmm7, xmm5               ; x⁵
    movss xmm8, xmm7
    mulss xmm8, xmm5               ; x⁷
    
    ; Combine polynomial terms
    addss xmm8, xmm7               ; x⁷ + x⁵
    addss xmm8, xmm6               ; + x³
    addss xmm8, xmm4               ; + x
    
    ; DEATH: Chain of expensive operations
    sqrtss xmm9, xmm8              ; sqrt(polynomial)
    divss xmm9, xmm2               ; / e
    sqrtss xmm10, xmm9             ; sqrt(sqrt(...))
    divss xmm10, xmm3              ; / phi
    sqrtss xmm11, xmm10            ; Another sqrt
    divss xmm11, dword [rel pi]    ; / pi
    
    ; Reciprocal hell (most expensive SSE operation)
    rcpss xmm12, xmm11             ; 1/x (expensive)
    rcpss xmm13, xmm12             ; 1/(1/x) = x (double expensive)
    rcpss xmm14, xmm13             ; Triple reciprocal torture
    
    ; Trigonometric torture (Taylor series to 9th order)
    movss xmm15, xmm1              ; x
    movss xmm4, xmm1
    mulss xmm4, xmm4               ; x²
    movss xmm5, xmm4
    mulss xmm5, xmm1               ; x³
    divss xmm5, dword [rel six]    ; x³/6
    subss xmm15, xmm5              ; x - x³/6
    
    movss xmm6, xmm5
    mulss xmm6, xmm4               ; x⁵
    divss xmm6, dword [rel onehundredtwenty] ; x⁵/120
    addss xmm15, xmm6              ; + x⁵/120
    
    movss xmm7, xmm6
    mulss xmm7, xmm4               ; x⁷
    divss xmm7, dword [rel fivethousand] ; x⁷/5040
    subss xmm15, xmm7              ; - x⁷/5040
    
    movss xmm8, xmm7
    mulss xmm8, xmm4               ; x⁹
    divss xmm8, dword [rel threehundredsixtythousand] ; x⁹/362880
    addss xmm15, xmm8              ; + x⁹/362880
    
    ; Logarithm approximation torture
    movss xmm9, xmm15
    subss xmm9, dword [rel one]    ; x-1
    movss xmm10, xmm9
    mulss xmm10, xmm9              ; (x-1)²
    divss xmm10, dword [rel two]   ; (x-1)²/2
    subss xmm9, xmm10              ; (x-1) - (x-1)²/2
    
    ; Exponential approximation torture
    movss xmm11, xmm1              ; x
    movss xmm12, xmm1
    mulss xmm12, xmm1              ; x²
    divss xmm12, dword [rel two]   ; x²/2
    addss xmm11, xmm12             ; x + x²/2
    movss xmm13, xmm12
    mulss xmm13, xmm1              ; x³
    divss xmm13, dword [rel six]   ; x³/6
    addss xmm11, xmm13             ; + x³/6
    
    ; Combine all torture results
    addss xmm0, xmm14              ; Add reciprocal result
    addss xmm0, xmm15              ; Add trig result
    addss xmm0, xmm9               ; Add log result
    addss xmm0, xmm11              ; Add exp result
    
    ; Update torture variables
    mulss xmm1, dword [rel chaos]  ; Chaotic multiplier
    mulss xmm2, dword [rel decay]  ; Decay
    mulss xmm3, dword [rel growth] ; Growth
    
    dec r11
    jnz .torture_inner
    dec r12
    jnz .torture_middle
    dec r13
    jnz .torture_outer
    
    ; Final torture: normalize result to prevent overflow
    mulss xmm0, dword [rel normalize]
    
    dec r15
    jnz .main_loop
    
    ; Store final result
    movss [rdx], xmm0
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

section .data
    align 16
one: dd 1.0
two: dd 2.0
six: dd 6.0
onehundredtwenty: dd 120.0
fivethousand: dd 5040.0
threehundredsixtythousand: dd 362880.0
pi: dd 3.14159265
e: dd 2.71828182
phi: dd 1.61803398
chaos: dd 1.001
decay: dd 0.999
growth: dd 1.002
normalize: dd 0.0001