global  effectASMSIMD
    section .text
; extern void effectASMSIMD(float *A,float *B,float *C,float alpha, int imgSize);
; rdi== *A -- rsi==*B -- rdx == *C -- xmm0==alpha -- rcx==imgSize
effectASMSIMD:
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor rax, rax
    shufps  xmm0,xmm0,0h
loop:
    cmp ecx, r8d
    jle final
    movaps  xmm1,[rdi] ; xmm1: A
    movaps  xmm2,[rsi] ; xmm2: B
    movaps  xmm3, xmm2 ; xmm3: B
    subps   xmm2, xmm1 ; xmm2: B-A
    mulss   xmm2, xmm0 ; xmm2: (B-A)*alpha
    addps   xmm2, xmm3 ; xmm2: (B-A)*alpha + B
    movaps  [rdx],xmm2 ; C: (B-A)*alpha + B
    add     rdi,    16
    add     rsi,    16
    add     rdx,    16
    add     r8,     4
    jmp loop
final:
    ret