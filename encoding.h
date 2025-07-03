// implements the instructions based on the following:
//
// The RISC-V Instruction Set Manual
// Volume I
// Unprivileged Architecture
// Version 20250508

// TODO: reorder the arguments - opcode at the right is not natural
// TODO: even tho it aligns left to right visual placement in diagrams
#ifndef ENCODING_H
#define ENCODING_H

#include <cstdint>

//R-type
typedef struct rtype32_t {
    unsigned opcode :7;
    unsigned rd     :5;
    unsigned funct3 :3;
    unsigned rs1    :5;
    unsigned rs2    :5;
    unsigned funct7 :7;

    operator uint32_t() const {
        return  (funct7 << 25)|(rs2 << 20)|(rs1 << 15)
               |(funct3 << 12)|(rd << 7)  | opcode;
    }
} rtype32_t;

uint32_t emit_r_type_instruction(
    unsigned funct7, unsigned rs1, unsigned rs2,
    unsigned funct3, unsigned rd, unsigned opcode
);

//I-type
typedef struct itype32_t{
    unsigned opcode: 7;
    unsigned rd    : 5;
    unsigned funct3: 3;
    unsigned rs1   : 5;
    unsigned imm   :12;

    operator uint32_t() const {
        return  (imm << 20)|(rs1 << 15)|(funct3 << 12)
               |  (rd << 7)|opcode;
    }
} itype32_t;

uint32_t emit_i_type_instruction(
    unsigned rd, unsigned rs1, unsigned imm,
    unsigned funct3, unsigned opcode
);

typedef struct stype32_t{
    unsigned opcode: 7;
    unsigned imm_lo: 5;
    unsigned funct3: 3;
    unsigned rs1   : 5;
    unsigned rs2   : 5;
    unsigned imm_hi: 7;

    operator uint32_t() const {
         return  (imm_hi << 27)|(rs2 << 20) |(rs1 << 15)
                |(funct3 << 12)|(imm_lo <<7)|opcode;
    }

} stype32_t;

uint32_t emit_s_type_instruction(unsigned rd, unsigned imm, unsigned opcode);

typedef struct utype32_t{
    unsigned opcode:  7;
    unsigned     rd:  5;
    unsigned    imm: 20;

    operator uint32_t() const {
        return (imm << 12)|(rd <<  7)| opcode;
    }

} utype32_t;
uint32_t emit_u_type_instruction(unsigned jump_offset, unsigned opcode);

typedef struct btype32_t{
    unsigned  opcode: 7;
    unsigned   imm11: 1;
    unsigned  imm4_1: 4;
    unsigned  funct3: 3;
    unsigned     rs1: 5;
    unsigned     rs2: 5;
    unsigned imm10_5: 6;
    unsigned   imm12: 1;

    operator uint32_t() const {
        return ((imm12 << 31)|(imm10_5 << 25)| (rs2 << 20)
                 |(rs1 << 15)|(funct3 << 12) |(imm4_1 << 8)
                 |(imm11<< 7)|opcode);
    }
} btype32_t;

uint32_t emit_b_type_instruction(
    unsigned imm, unsigned rs1, unsigned rs2,
    unsigned funct3, unsigned opcode
);

// IMPORTANT - this is for the bison non-terminal to be able to
// return complex data.
typedef struct opcode_t {
    unsigned op;
    unsigned funct3;
    unsigned funct5;
    unsigned funct7;
    unsigned imm12;
    int      valid;
} opcode_t;

#endif //ENCODING_H
