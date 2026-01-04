.globl _start      # Provide program starting address to linker

_start: addi  a0, x0, 0b01     # 1 = StdOut
        la    a1, helloworld   # load address of helloworld
        addi  a2, x0, 13       # length of our string
        addi  a7, x0, 64       # linux write system call
        ecall                  # Call linux to output the string
        addi    a0, x0, 0      # Use 0 return code
        addi    a7, x0, 0x5D   # Service command code 93 terminates
        ecall                  # Call linux to terminate the program

.data 
message:      .ascii "Hello World!\n"
message2:     .ascii "second world."
# TODO: differentiate between a variable label and section label
