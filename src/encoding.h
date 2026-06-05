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

// good for n < 64
#define BIT_MASK(n) ((1ULL << (n)) - 1) 

// Instruction types and details
//R-type
typedef struct rtype32_t {
    uint32_t opcode :7;
    uint32_t rd     :5;
    uint32_t funct3 :3;
    uint32_t rs1    :5;
    uint32_t rs2    :5;
    uint32_t funct7 :7;

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
    uint32_t opcode: 7;
    uint32_t rd    : 5;
    uint32_t funct3: 3;
    uint32_t rs1   : 5;
    uint32_t imm   :12;

    operator uint32_t() const {
        return ((imm << 20)|(rs1 << 15)|(funct3 << 12) | (rd << 7)| opcode);
    }
} itype32_t;

uint32_t emit_i_type_instruction(
    unsigned rd, unsigned rs1, unsigned imm,
    unsigned funct3, unsigned opcode
);

typedef struct stype32_t{
    uint32_t opcode: 7;
    uint32_t imm_lo: 5;
    uint32_t funct3: 3;
    uint32_t rs1   : 5;
    uint32_t rs2   : 5;
    uint32_t imm_hi: 7;

    operator uint32_t() const {
         return ((imm_hi << 25) |(rs2 << 20) |(rs1 << 15) | (funct3 << 12) |(imm_lo <<7)| opcode);
    }

    static stype32_t deserialize(uint32_t val){
        stype32_t i;

        i.opcode = (val      ) & BIT_MASK(7);
        i.imm_lo = (val >>  7) & BIT_MASK(5);
        i.funct3 = (val >> 12) & BIT_MASK(3);
        i.rs1    = (val >> 15) & BIT_MASK(5);
        i.rs2    = (val >> 20) & BIT_MASK(5);
        i.imm_hi = (val >> 25) & BIT_MASK(7);

        return i;
    }

} stype32_t;

uint32_t emit_s_type_instruction(uint32_t rs2, uint32_t rs1, uint32_t imm, uint32_t funct3, uint32_t opcode);

typedef struct utype32_t{
    uint32_t opcode:  7;
    uint32_t     rd:  5;
    uint32_t    imm: 20;

    operator uint32_t() const {
        return (imm << 12)|(rd <<  7)| opcode;
    }
 
    static utype32_t deserialize(uint32_t val){
        utype32_t i;
 
        i.opcode = (val      ) & BIT_MASK(7); 
        i.rd     = (val >> 7 ) & BIT_MASK(5);
        i.imm    = (val >> 12) & BIT_MASK(20);

        return i;
    }

} utype32_t;
uint32_t emit_u_type_instruction(unsigned imm, unsigned rd, unsigned opcode);

typedef struct btype32_t{
    uint32_t  opcode: 7;
    uint32_t   imm11: 1;
    uint32_t  imm4_1: 4;
    uint32_t  funct3: 3;
    uint32_t     rs1: 5;
    uint32_t     rs2: 5;
    uint32_t imm10_5: 6;
    uint32_t   imm12: 1;

    operator uint32_t() const {
        return ((imm12 << 31)|(imm10_5 << 25)| (rs2 << 20) |(rs1 << 15)|(funct3 << 12) |(imm4_1 << 8) |(imm11<< 7)
                |opcode);
    }
   
    static btype32_t deserialize(uint32_t val) {
        btype32_t i{};

        i.opcode   = (val      ) & BIT_MASK(7);        // bits 6:0
        i.imm11    = (val >>  7) & BIT_MASK(1);        // bit 7
        i.imm4_1   = (val >>  8) & BIT_MASK(4);        // bits 11:8
        i.funct3   = (val >> 12) & BIT_MASK(3);        // bits 14:12
        i.rs1      = (val >> 15) & BIT_MASK(5);        // bits 19:15
        i.rs2      = (val >> 20) & BIT_MASK(5);        // bits 24:20
        i.imm10_5  = (val >> 25) & BIT_MASK(6);        // bits 30:25
        i.imm12    = (val >> 31) & BIT_MASK(1);        // bit 31
        return i;
    }

} btype32_t;

uint32_t emit_b_type_instruction(
    unsigned imm, unsigned rs1, unsigned rs2,
    unsigned funct3, unsigned opcode
);

typedef struct jtype32_t {
    uint32_t opcode  : 7;
    uint32_t rd      : 5;
    uint32_t imm12_19: 8;
    uint32_t imm11   : 1;
    uint32_t imm1_10 :10;
    uint32_t imm20   : 1;

    operator uint32_t () const {
       return
          (imm20   << 31) | 
          (imm1_10 << 21) | 
          (imm11   << 20) | 
          (imm12_19<< 12) |
          (rd      << 7 ) |
          opcode;
    }

    static jtype32_t deserialize(uint32_t val){
        jtype32_t i{};

        i.opcode   = (val >>  0) & BIT_MASK(7);
        i.rd       = (val >>  7) & BIT_MASK(5);
        i.imm12_19 = (val >> 12) & BIT_MASK(8);
        i.imm11    = (val >> 20) & BIT_MASK(1);
        i.imm1_10  = (val >> 21) & BIT_MASK(10);
        i.imm20    = (val >> 31) & BIT_MASK(1);

        return i;
    }
 
} jtype32_t;


uint32_t emit_j_type_instruction(unsigned imm, unsigned rd, unsigned opcode);

// IMPORTANT - this is for the bison non-terminal to be able to
// return complex data. TODO: move this from here to somewhere more appropriate
typedef struct opcode_t {
    uint32_t op;
    uint32_t funct3;
    uint32_t funct5;
    uint32_t funct7;
    uint32_t imm12;
    int32_t  valid;
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
