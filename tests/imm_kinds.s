.globl _main
_main:
    addi x1, x2, %hi(hello)

.data
hello: .ascii "hola" 
