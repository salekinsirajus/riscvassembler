#ifndef ENCODING_H
#define ENCODING_H

typedef struct inst32_t {
	unsigned opcode :7;
	unsigned rd     :5;
	unsigned funct3 :3;
	unsigned rs1    :5;
	unsigned rs2    :5;
	unsigned funct7 :7;
} inst32_t;

#endif
