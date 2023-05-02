.globl main
.data
gbl_func:	.quad 0
gbl_main:	.quad 0
str_2:
	.asciz "\n"
str_1:
	.asciz "\n"
str_0:
	.asciz "\n"
.align 8
.text
fun_func: subq $8, %rsp
movq %rbp, (%rsp)
movq %rsp, %rbp
addq $16, %rbp
subq $192, %rsp
#Fn body func
#            getarg 1 [a]
movq %rdi, 72(%rbp)
#            getarg 2 [b]
movq %rsi, 64(%rbp)
#            getarg 3 [c]
movq %rdx, 56(%rbp)
#            getarg 4 [d]
movq %rcx, 48(%rbp)
#            getarg 5 [e]
movq %r8, 40(%rbp)
#            getarg 6 [f]
movq %r9, 32(%rbp)
#            getarg 7 [g]
#            getarg 8 [j]
#            getarg 9 [m]
#            getarg 10 [n]
#            [h] := [g]
movq 24(%rbp), %rax
movq %rax, -112(%rbp)
#            [i] := 0
movq $0, %rax
movq %rax, -120(%rbp)
lbl_1: #lbl_1:      nop
nop
#            [tmp0] := [i] LT64 5
movq -120(%rbp), %rax
movq $5, %rbx
cmpq %rbx, %rax
setl %al
movq %rax, %rdx
movq %rax, -24(%rbp)
#            IFZ [tmp0] GOTO lbl_2
movq -24(%rbp), %r11
cmpq $0, %r11
je lbl_2
#            REPORT [i]
movq -120(%rbp), %rdi
callq printInt
#            REPORT str_0
movq $str_0, %rdi
callq printString
#            [i] := [i] ADD64 1
movq -120(%rbp), %rax
movq $1, %rbx
addq %rbx, %rax
movq %rax, -120(%rbp)
#            goto lbl_1
jmp lbl_1
lbl_2: #lbl_2:      nop
nop
#            [i] := 0
movq $0, %rax
movq %rax, -120(%rbp)
lbl_3: #lbl_3:      nop
nop
#            [tmp1] := [i] NEQ64 10
movq -120(%rbp), %rax
movq $10, %rbx
cmpq %rbx, %rax
setne %al
movq %rax, %rdx
movq %rax, -32(%rbp)
#            IFZ [tmp1] GOTO lbl_4
movq -32(%rbp), %r11
cmpq $0, %r11
je lbl_4
#            REPORT [i]
movq -120(%rbp), %rdi
callq printInt
#            REPORT str_1
movq $str_1, %rdi
callq printString
#            [i] := [i] ADD64 1
movq -120(%rbp), %rax
movq $1, %rbx
addq %rbx, %rax
movq %rax, -120(%rbp)
#            goto lbl_3
jmp lbl_3
lbl_4: #lbl_4:      nop
nop
#            [tmp2] := [a] ADD64 [b]
movq 72(%rbp), %rax
movq 64(%rbp), %rbx
addq %rbx, %rax
movq %rax, -40(%rbp)
#            [tmp3] := [tmp2] ADD64 [c]
movq -40(%rbp), %rax
movq 56(%rbp), %rbx
addq %rbx, %rax
movq %rax, -48(%rbp)
#            [tmp4] := [tmp3] ADD64 [d]
movq -48(%rbp), %rax
movq 48(%rbp), %rbx
addq %rbx, %rax
movq %rax, -56(%rbp)
#            [tmp5] := [tmp4] ADD64 [e]
movq -56(%rbp), %rax
movq 40(%rbp), %rbx
addq %rbx, %rax
movq %rax, -64(%rbp)
#            [tmp6] := [tmp5] ADD64 [f]
movq -64(%rbp), %rax
movq 32(%rbp), %rbx
addq %rbx, %rax
movq %rax, -72(%rbp)
#            [tmp7] := [tmp6] ADD64 [h]
movq -72(%rbp), %rax
movq -112(%rbp), %rbx
addq %rbx, %rax
movq %rax, -80(%rbp)
#            [tmp8] := [tmp7] ADD64 [j]
movq -80(%rbp), %rax
movq 16(%rbp), %rbx
addq %rbx, %rax
movq %rax, -88(%rbp)
#            [tmp9] := [tmp8] ADD64 [m]
movq -88(%rbp), %rax
movq 8(%rbp), %rbx
addq %rbx, %rax
movq %rax, -96(%rbp)
#            [tmp10] := [tmp9] ADD64 [n]
movq -96(%rbp), %rax
movq 0(%rbp), %rbx
addq %rbx, %rax
movq %rax, -104(%rbp)
#            setret [tmp10]
movq -104(%rbp), %rax
#            goto lbl_0
jmp lbl_0
#Fn epilogue func
lbl_0: addq $192, %rsp
movq (%rsp), %rbp
addq $8, %rsp
retq
main: subq $8, %rsp
movq %rbp, (%rsp)
movq %rsp, %rbp
addq $16, %rbp
subq $64, %rsp
#Fn body main
#            [a] := 4
movq $4, %rax
movq %rax, -32(%rbp)
#            [c] := 5
movq $5, %rax
movq %rax, -40(%rbp)
#            REPORT str_2
movq $str_2, %rdi
callq printString
#            setarg 1 1
movq $1, %rdi
#            setarg 2 2
movq $2, %rsi
#            setarg 3 3
movq $3, %rdx
#            setarg 4 4
movq $4, %rcx
#            setarg 5 5
movq $5, %r8
#            setarg 6 6
movq $6, %r9
#            setarg 7 7
pushq $7
#            setarg 8 8
pushq $8
#            setarg 9 [a]
movq -32(%rbp), %r12
pushq %r12
#            setarg 10 [a]
movq -32(%rbp), %r12
pushq %r12
#            call func
callq fun_func
addq $32, %rsp
#            getret [tmp0]
movq %rax, -24(%rbp)
#            REPORT [tmp0]
movq -24(%rbp), %rdi
callq printInt
#            setret 0
movq $0, %rax
#            goto lbl_5
jmp lbl_5
#Fn epilogue main
lbl_5: addq $64, %rsp
movq (%rsp), %rbp
addq $8, %rsp
retq
