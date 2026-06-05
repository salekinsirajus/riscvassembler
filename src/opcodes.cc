#include "opcodes.h"

bool is_load(uint32_t opcode)
{
    if (opcode == LB_32 || opcode == LH_32 || opcode == LW_32 || opcode == LBU_32 || opcode == LHU_32 || opcode == LHU_32)
    {
        return true;
    }

    return false;
}
