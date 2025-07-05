    .section .text
    .globl strlen
strlen:
    # a0 = const char *str
    add     t0, zero, zero     # i = 0

loop:
    add     t1, t0, a0         # t1 = str + i
#    lb      t1, 0(t1)          # t1 = str[i]
    beq     t1, zero, done     # if str[i] == 0, break
    addi    t0, t0, 1          # i++
#    j       loop               # repeat

done:
#    mv      a0, t0             # return i in a0
    addi    t0, t0, 1           # placeholder TODO: remove
#    ret                        # jalr zero, ra
