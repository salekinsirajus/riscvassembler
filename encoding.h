#ifndef ENCODING_H
#define ENCODING_H

#include <cstdint>

typedef struct rtype32_t {
    unsigned opcode :7;
    unsigned rd     :5;
    unsigned funct3 :3;
    unsigned rs1    :5;
    unsigned rs2    :5;
    unsigned funct7 :7;

    operator uint32_t() const {
        return (funct7 << 25)|
                  (rs2 << 20)|
                  (rs1 << 15)|
               (funct3 << 12)|
                    (rd << 7)|
                        opcode;
    }
} rtype32_t;

typedef struct itype32_t{
    unsigned opcode: 7;
    unsigned rd    : 5;
    unsigned funct3: 3;
    unsigned rs1   : 5;
    unsigned imm   :12;

    operator uint32_t() const {
        return  (imm << 20)|
                (rs1 << 15)|
             (funct3 << 12)|
                  (rd << 7)|
                     opcode;
    }
} itype32_t;

#endif //ENCODING_H
