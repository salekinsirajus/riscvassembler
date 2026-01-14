#include "encoding.h"
#include <iostream>
#include <string>

#define MAX_12BIT_SIGNED  2047
#define MIN_12BIT_SIGNED -2048

std::string generate_dest_filename(std::string source_filename)
{
    size_t last_dot = source_filename.find_last_of("."); // last occurance of the dot
    std::string result;
    const std::string ext = ".o";
    if (last_dot == std::string::npos)
    {
        return source_filename + ext;
    } 

    // is a name like .x is going to cause problem?
    return source_filename.substr(0, last_dot) + ext;
}

void exit_with_message(
    int32_t line_no, int32_t col_no, std::string source_filename, 
    std::string offending_stmt, std::string error_token, int32_t error_code
)
{
   // TODO: add a data structure to pull the message string instead of hardcoding
   // TODO: them into this function utilize the error_code for this purpose.
   std::string MSG = "value is out of the acceptable range";
   std::cout << source_filename << ":" << line_no << ":" << " " << MSG << " ";
   std::cout << error_token << " in " << offending_stmt << std::endl;
   exit(1);
}

bool is_within_range_12b(int32_t imm)
{
    return (imm >= MIN_12BIT_SIGNED) && (imm <= MAX_12BIT_SIGNED);
}

// Function to print the instruction structure in hex
void print_instruction_hex(const rtype32_t& instr) {
	uint32_t encoded = 0;
	encoded |= (instr.opcode & 0x7F);
	encoded |= (instr.rd & 0x1F) << 7;
	encoded |= (instr.funct3 & 0x7) << 12;
	encoded |= (instr.rs1 & 0x1F) << 15;
	encoded |= (instr.rs2 & 0x1F) << 20;
	encoded |= (instr.funct7 & 0x7F) << 25;

	printf("Inst: 0x%08X\n", encoded);
}

void print_instruction(const rtype32_t& instr) {
	printf("Instruction Encoded: opcode = %u, rd = %u, funct3 = %u, rs1 = %u, rs2 = %u, funct7 = %u\n",
		   instr.opcode, instr.rd, instr.funct3, instr.rs1, instr.rs2, instr.funct7);
	print_instruction_hex(instr);
}
