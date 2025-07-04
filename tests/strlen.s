# Taken from here: https://www.riscvschool.com/2022/10/27/risc-v-example-assembly-programs/
# 
# Original C program:
# int strlen(const char *str) {
#   int i;
#   for (i = 0;str[i] != '\0';i++);
#   return i;
# }

.section .text
.global strlen
strlen:
    # a0 = const char *str
    add  t0, zero, zero # i = 0
L1: # Start of for loop  # TODO: support numeric labels as in asm manuals
    add  t1, t0, a0      # Add the byte offset for str[i]
#    lb   t1, 0(t1)       # Dereference str[i]
    beq  t1, zero, L1    # if str[i] == 0, break for loop
    addi t0, t0, 1       # Add 1 to our iterator
#    jal  zero, 1b        # Jump back to condition (1 backwards)
L1: # End of for loop
    addi a0, t0, 0       # Move t0 into a0 to return
#    jalr zero, ra        # Return back via the return address register
