#include "encoding.h"

uint32_t emit_l_type_instruction(unsigned rd, unsigned lui_imm, unsigned opcode){
}

uint32_t emit_i_type_instruction(
    unsigned rd,
    unsigned rs1,
    unsigned imm,
    unsigned funct3,
    unsigned opcode){

    itype32_t i;
    i.rd = rd;
    i.rs1 = rs1;
    i.imm = imm;
    i.funct3 = funct3;
    i.opcode = opcode;

    return static_cast<uint32_t>(i);
}

uint32_t emit_r_type_instruction(
    unsigned funct7,
    unsigned rs1,
    unsigned rs2,
    unsigned funct3,
    unsigned rd,
    unsigned opcode
)
{
    rtype32_t i;

    i.rd = rd;
    i.rs1 = rs1;
    i.rs2 = rs2;
    i.funct7 = funct7;
    i.funct3 = funct3;
    i.opcode = opcode;

    return static_cast<uint32_t>(i);
}

uint32_t emit_b_type_instruction(
    unsigned imm, unsigned rs2, unsigned rs1,
    unsigned funct3, unsigned opcode
)
{
    btype32_t i;

    i.opcode = opcode;
    i.funct3 = funct3;
    i.rs2 = rs2;
    i.rs1 = rs1;

    // imm:  C B A 9 8 7 6 5 4 3 2 1 0
    // imm:  0 0 0 0 0 0 0 0 0 4 3 2 1
    //                         1 1 1 1
    i.imm4_1 = (imm >>  1) & 0xF;
    // imm:  0 0 0 0 0 0 0 0 0 0 0 C B
    i.imm11  = (imm >> 11) & 0x1;
    // imm:  0 0 0 0 0 0 0 0 0 0 0 0 C
    i.imm12  = (imm >> 12) & 0x1;
    // imm:  0 0 0 0 0 C B A 9 8 7 6 5
    //                     1 1 1 1 1 1
    i.imm10_5=  (imm >> 5) & 0x3F;

    return static_cast<uint32_t>(i);
}
