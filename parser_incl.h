#pragma once
#include <cstdint>

typedef struct opcode_t {
    uint32_t op;
    uint32_t funct3;
    uint32_t funct5;
    uint32_t funct7;
    uint32_t imm12;
    int32_t  valid;
} opcode_t;

enum class imm_kind {
    IMM_INT = 1,
    IMM_SYMBOL = 2,
    MODIFIER_ABS_HI = 3,
    MODIFIER_ABS_LO = 4,
    MODIFIER_PCREL_HI = 5,
    MODIFIER_PCREL_LO = 6,
};
