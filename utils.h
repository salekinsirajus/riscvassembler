#include "encoding.h"


// Function to print the instruction structure in hex
void print_instruction_hex(const inst32_t& instr) {
	uint32_t encoded = 0;
	encoded |= (instr.opcode & 0x7F);
	encoded |= (instr.rd & 0x1F) << 7;
	encoded |= (instr.funct3 & 0x7) << 12;
	encoded |= (instr.rs1 & 0x1F) << 15;
	encoded |= (instr.rs2 & 0x1F) << 20;
	encoded |= (instr.funct7 & 0x7F) << 25;

	printf("Inst: 0x%08X\n", encoded);
}

void print_instruction(const inst32_t& instr) {
	printf("Instruction Encoded: opcode = %u, rd = %u, funct3 = %u, rs1 = %u, rs2 = %u, funct7 = %u\n",
		   instr.opcode, instr.rd, instr.funct3, instr.rs1, instr.rs2, instr.funct7);
	print_instruction_hex(instr);
}

