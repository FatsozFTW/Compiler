.globl main
.data

gbl_main:	.quad 0
.align 8
.text
main: subq $8, %rsp
movq %rbp, (%rsp)
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
#Fn body main
#            [b] := 0
movq $0, %rax
movq %rax, -32(%rbp)
#            [a] := 4
movq $4, %rax
movq %rax, -24(%rbp)
#            REPORT [a]
movq -24(%rbp), %rdi
callq printInt
#            REPORT [b]
movq -32(%rbp), %rdi
callq printBool
#Fn epilogue main
lbl_0: addq $16, %rsp
movq (%rsp), %rbp
addq $8, %rsp
retq
