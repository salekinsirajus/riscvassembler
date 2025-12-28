#ifndef OPCODES_H
#define OPCODES_H

// BASE 32I
#define LUI_32    0x37
#define AUIPC_32  0x17
#define JAL_32    0x6f
#define JALR_32   0x67
#define BEQ_32    0x63
#define BNE_32    0x63
#define BLT_32    0x63
#define BGE_32    0x63
#define BLTU_32   0x63
#define BGEU_32   0x63 
#define LB_32     0x03
#define LH_32     0x03
#define LW_32     0x03
#define LBU_32    0x03
#define LHU_32    0x03
#define SB_32     0x23
#define SH_32     0x23
#define SW_32     0x23
#define ADDI_32   0x13
#define SLTI_32   0x13
#define XORI_32   0x13
#define ORI_32    0x13
#define ANDI_32   0x13
#define SLLI_32   0x13
#define SRLI_32   0x13
#define SRAI_32   0x13
#define ADD_32    0x33
#define SUB_32    0x33
#define SLL_32    0x33
#define SLT_32    0x33
#define SLTU_32   0x33
#define SLTIU_32  0x33
#define XOR_32    0x33
#define SRL_32    0x33
#define SRA_32    0x33
#define OR_32     0x33
#define AND_32    0x33
#define FENCE_32  0x0f
#define FENCETSO_32  0x0f
#define PAUSE_32  0x0f
#define ECALL_32  0x73
#define EBREAK_32 0x73

#endif // OPCODES_H
