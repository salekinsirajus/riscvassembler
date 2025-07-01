#ifndef ENCODING_H
#define ENCODING_H

#include <cstdint>

// implements the instructions based on the following:
//
// The RISC-V Instruction Set Manual
// Volume I
// Unprivileged Architecture
// Version 20250508

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

//R-type
uint32_t emit_r_type_instruction(
    unsigned funct7, unsigned rs1, unsigned rs2, 
    unsigned funct3, unsigned rd, unsigned opcode
);

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

//I-type
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
    unsigned opcode: 7;
    unsigned  imm41: 5;
    unsigned funct3: 3;
    unsigned    rs1: 5;
    unsigned    rs2: 5;
    unsigned  imm12: 7;    

    operator uint32_t() const {
        return ((imm12 < 25)|(rs2 << 20)|(rs1 << 15)|(funct3 << 12)|(imm41 << 7)|opcode);
    }
} btype32_t;

uint32_t emit_b_type_instruction(
    unsigned imm12, unsigned rs1, unsigned rs2, 
    unsigned funct3, unsigned imm41, unsigned opcode
);

// IMPORTANT - this is for the bison rules to be able to 
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
