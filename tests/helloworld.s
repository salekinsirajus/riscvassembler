.global _start

_start:
	# stdout = (fd) 1
	addi a7, zero, 64
	addi a0, zero, 1
	la a1, helloworld
	addi a2, zero, 15
	ecall

	# Exit with code 13
	addi a7, zero, 93
	addi a0, zero, 13
	ecall


helloworld:
	.ascii "Hello, World!\n"
