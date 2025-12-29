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

#define INSTRUCTION_WIDTH sizeof(uint32_t)

// Instruction types and details
//R-type
typedef struct rtype32_t {
    unsigned opcode :7;
    unsigned rd     :5;
    unsigned funct3 :3;
    unsigned rs1    :5;
    unsigned rs2    :5;
    unsigned funct7 :7;

    operator uint32_t() const {
        return  ((funct7 << 25)|(rs2 << 20)|(rs1 << 15) |(funct3 << 12)|(rd << 7) | opcode);
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
        return  ((imm << 20)|(rs1 << 15)|(funct3 << 12) | (rd << 7)| opcode);
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
         return ((imm_hi << 27)|(rs2 << 20) |(rs1 << 15) |(funct3 << 12)|(imm_lo <<7)|opcode);
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
 
    static utype32_t deserialize(uint32_t val){
        utype32_t i;
 
        i.opcode = val & 0x7F; 
        i.rd     = (val >> 7) & 0x1F;
        i.imm    = (val >> 12) & 0xFFFFF;

        return i;
    }

} utype32_t;
uint32_t emit_u_type_instruction(unsigned imm, unsigned rd, unsigned opcode);

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
        return ((imm12 << 31)|(imm10_5 << 25)| (rs2 << 20) |(rs1 << 15)|(funct3 << 12) |(imm4_1 << 8) |(imm11<< 7)
                |opcode);
    }
   
    static btype32_t deserialize(uint32_t val) {
        btype32_t i;
        i.opcode   = val & 0x7F;                // bits 6:0
        i.imm11    = (val >> 7) & 0x1;          // bit 7
        i.imm4_1   = (val >> 8) & 0xF;          // bits 11:8
        i.funct3   = (val >> 12) & 0x7;         // bits 14:12
        i.rs1      = (val >> 15) & 0x1F;        // bits 19:15
        i.rs2      = (val >> 20) & 0x1F;        // bits 24:20
        i.imm10_5  = (val >> 25) & 0x3F;        // bits 30:25
        i.imm12    = (val >> 31) & 0x1;         // bit 31
        return i;
    }

} btype32_t;

uint32_t emit_b_type_instruction(
    unsigned imm, unsigned rs1, unsigned rs2,
    unsigned funct3, unsigned opcode
);

typedef struct jtype32_t {
    unsigned opcode  : 7;
    unsigned rd      : 5;
    unsigned imm12_19: 8;
    unsigned imm11   : 1;
    unsigned imm1_10 :10;
    unsigned imm20   : 1;

    operator uint32_t const () {
       return (imm20 << 31 | imm12_19 << 23 | imm11 << 22 | imm1_10 << 12 | rd << 7 |opcode);
    }

    //TODO: deserialize
} jtype32_t;


uint32_t emit_j_type_instruction(unsigned imm, unsigned rd, unsigned opcode);

// IMPORTANT - this is for the bison non-terminal to be able to
// return complex data. TODO: move this from here to somewhere more
// appropriate perhaps?
typedef struct opcode_t {
    unsigned op;
    unsigned funct3;
    unsigned funct5;
    unsigned funct7;
    unsigned imm12;
    int      valid;
} opcode_t;

enum RISCV32_INST_TYPE {
	B_TYPE,
	R_TYPE,
	I_TYPE,
	S_TYPE,	
	U_TYPE,
    J_TYPE,
};

#endif //ENCODING_H
