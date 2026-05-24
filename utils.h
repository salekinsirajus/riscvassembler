// this is a catch-all for utilites that I needed. As it grows, we
// can move functions into a specailized file. For now, this works.
#pragma once

#include "encoding.h"
#include <iostream>
#include <string>

#define MAX_12_BIT_SIGNED  2047  // 2^12 - 1
#define MIN_12_BIT_SIGNED -2048  // -2^12
#define MAX_21_BIT_SIGNED  1048575  // 2^20 - 1
#define MIN_21_BIT_SIGNED -1048576  // -2^20

std::string generate_dest_filename(std::string source_filename);

void exit_with_message(
    int32_t line_no, int32_t col_no, std::string source_filename, 
    std::string offending_stmt, std::string error_token, int32_t error_code
);

inline bool is_within_range_12b(int32_t imm)
{
    return (imm >= MIN_12_BIT_SIGNED) && (imm <= MAX_12_BIT_SIGNED);
}

inline bool is_within_range_21b(int32_t imm)
{
    return (imm >= MIN_21_BIT_SIGNED) && (imm <= MAX_21_BIT_SIGNED);
}

// Function to print the instruction structure in hex
void print_instruction_hex(const rtype32_t& instr);

inline void print_instruction(const rtype32_t& instr) {
	printf("Instruction Encoded: opcode = %u, rd = %u, funct3 = %u, rs1 = %u, rs2 = %u, funct7 = %u\n",
		   instr.opcode, instr.rd, instr.funct3, instr.rs1, instr.rs2, instr.funct7);
	print_instruction_hex(instr);
}

template <typename T>
void write_le(std::ostream &out, T value)
{
    static_assert(std::is_integral<T>::value, "int type required.");

    unsigned char bytes[sizeof(T)];
	for (size_t i=0; i< sizeof(T); i++)
    {
       bytes[i] = (value >> (8 * i)) & 0xFF;
    }

    out.write(reinterpret_cast<const char*>(bytes), sizeof(T));
}
