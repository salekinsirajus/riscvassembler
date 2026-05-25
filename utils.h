// this is a catch-all for utilites that I needed. As it grows, we
// can move functions into a specailized file. For now, this works.
#pragma once

#include "encoding.h"
#include <iostream>
#include <string>
#include <type_traits>

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

/**
 *@brief writes any int types into the `ostream` in a little-endian layout.
 *@param out a reference to an std::ostream
 *@param value any int-types you want to serialize
 */
template <typename T>
void write_le(std::ostream &out, T value)
{
    static_assert(std::is_integral<T>::value, "int type required.");

    typedef typename std::make_unsigned<T>::type UnsignedT;
    UnsignedT u_value = static_cast<UnsignedT>(value);

    unsigned char bytes[sizeof(T)];
	for (size_t i=0; i<sizeof(T); i++)
    {
       bytes[i] = (u_value >> (8 * i)) & 0xFF;
       // 0xABCDEF01 (LSB at the smallest address: 01 at 0x0)
       // 0x0: 01
       // 0x1: EF
       // 0x2: CD
       // 0x3: AB
    }

    out.write(reinterpret_cast<const char*>(bytes), sizeof(T));
}

/**
 *@brief writes any int types into the `ostream` in a big-endian layout.
 *@param out a reference to an std::ostream
 *@param value any int-types you want to serialize
 */
template <typename T>
void write_be(std::ostream &out, T value)
{
    static_assert(std::is_integral<T>::value, "int type required.");

    typedef typename std::make_unsigned<T>::type UnsignedT;
    UnsignedT u_value = static_cast<UnsignedT>(value);

    unsigned char bytes[sizeof(T)];
	for (size_t i=0; i < sizeof(T); i++)
    {
       bytes[sizeof(T)-i-1] = (u_value >> (8 * i)) & 0xFF;
    }
    // 0xABCDEF01 (MSB at the smallest address, AB at 0x0)
    // 0x3 = 01
    // 0x2 = EF
    // 0x1 = CD
    // 0x0 = AB

    out.write(reinterpret_cast<const char*>(bytes), sizeof(T));
}
