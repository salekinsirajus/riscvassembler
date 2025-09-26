%{
    // riscvass.y - the bison file
    // for bison
    #include <stdio.h>
    #include <iostream>
    #include "encoding.h"
    #include "utils.h"
    #include "linux/elf.h"

    using namespace std;

    ELF32 newElfContent;

    // flex stuff bison needs
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
    void yyerror(const char *s);

    // go from left to right
    // TODO: remove the specific data type
    rtype32_t    rtype_instr;
    itype32_t    itype_instr;
    uint32_t     temp_inst;

    uint32_t offset;
    std::string   temp_value;
    std::string currentSection;
    std::string currentLabel;
    int32_t op_status;
%}

%union {
    int         ival;
    float       fval;
    char       *sval;
    int        token;
    opcode_t   opc_t;
}


//Terminal symbols 
// TODO - group them by types
%token <token> REG
%token <token> COMMA
%token <token> COLON
%token <ival>  IMM
%token <token> PAREN_OPEN PAREN_CLOSE
%token <token> ID  //Might need to update this
%token <token> ASSIGNMENT

%token <token> ADD ADDI AND ANDI SUB SUBI LI LA BGE BGEU
%token <token> BLT BLTU BEQ BNE LB LBU LH LHU LW OR ORI
%token <token> SB SH SLL SLLI SLT SLTI SLTIU SLTU SRA
%token <token> SRAI SRL SRLI SW XOR XORI
%token <token> ECALL

%token <token> JUMP MOV RET

%token <token> SECTION
%token <token> DIRECTIVE_COMMAND
%token <token> TEXT
%token <token> D_GLOBAL D_DATA D_TEXT D_SIZE D_RODATA D_BSS
%token <token> D_ASCII

//non-terminals
%token <sval> STRING
%token <sval> LABEL
%type <ival>  register;
%type <opc_t> opcode;

%%
//actual grammar
//the rhs stuff are 1-indexed
program:
    statements
    {
        std::cout << "reading program" << std::endl;
        newElfContent._resolve_unresolved_instructions();
        write_elf(newElfContent, "out.o");
    }
    ;

statements:
    statement
    | statements statement;

statement:
     LABEL COLON instructions
    {
        cout<< "LABEL: instructions" << endl;
        // This is where a label and associated instructions start
        newElfContent.init_label($1, false/*is_global*/, currentSection);
    }
    | LABEL COLON directive
    {
        currentLabel = $1;
        std::cout << "LABEL: directive (" << currentLabel << ")" << std::endl;
        //TODO: add other type of values besides string
        //FIXME: is this even correct?
        if (temp_value.size() > 0){
            newElfContent.add_variable_to_symtab(currentLabel, temp_value, ".data");    
            temp_value = "";  //reset
        }
    }
    | directive
    {
        std::cout << "directive (NYI)" << std::endl;
    }
    ;

directive:
     SECTION D_DATA
    {
        std::cout << "section .data" << std::endl;
        currentSection = ".data";
        currentLabel = ".data";
    }
    | SECTION D_TEXT
    {
        std::cout << "section .text" << std::endl;
        currentSection = ".text";
        currentLabel = ".text";
    }
    | D_TEXT
    {
        std::cout << ".text" << std::endl;
        currentLabel = ".text";
    }
    | D_DATA
    {
        std::cout << ".data" << std::endl;
        currentSection = ".data";
        currentLabel = ".data";
    }
    | D_GLOBAL LABEL
    {
        std::cout << ".globl LABEL (" << $2 << ")" << std::endl; 
        //newElfContent.update_label_visibility($2, true); //TODO: implemet API to modify visibility
    }
    | D_ASCII STRING
    {
        std::cout << ".ascii STRING: " << currentLabel << std::endl;
        temp_value = $2;
    }
    ;

instructions:
    instruction
    | instructions instruction
    ;

instruction:
    opcode register COMMA register COMMA register
    {
        //R-format: opcode dest, rs1, rs2
        //ADD SUB SLL SLT SLTU XOR SRL SRA OR AND
        std::cout << "r" << $2 << ", r" << $4 << ", r" << $6 << std::endl;
        temp_inst = emit_r_type_instruction(0, $4, $6, 0, $2, ($1).op);
        newElfContent.add_to_text(temp_inst); //TODO: add API

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode register COMMA register COMMA IMM
    {
        //ADDI SUBI SLLI SLTI SLTUI XORI SRLI SRAI ORI ANDI
        std::cout << "r" << $2 << ", r" << $4 << ", " << $6 << std::endl;
        temp_inst = emit_i_type_instruction($2, $4, $6, 0, ($1).op);
        newElfContent.add_to_text(temp_inst); //TODO: add API

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode register COMMA IMM PAREN_OPEN register PAREN_CLOSE
    {
        std::cout << "r" << $2 << " " << $4 << "(r" << $6 << ")" << std::endl;
        // I think these are I-type instructions
        // TODO: sign-extended and unsigned are handled appropriately
        temp_inst = emit_i_type_instruction($2, $6, $4, ($1).funct3, ($1).op);
        newElfContent.add_to_text(temp_inst);

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode register COMMA register COMMA LABEL
    {
        std::cout << "r"<< $2 << ", r" << $4 << ", " << $6 << std::endl;
        offset = 0;
        op_status = newElfContent.resolve_label($6, offset);
        if (op_status == -1){
            std::cout << "no valid address found: " << offset << std::endl;
            newElfContent.add_to_unresolved_insns(
                newElfContent.get_next_insn_number(currentSection), B_TYPE, offset,
                newElfContent.get_next_insn_number(currentSection) - 1 /* add api */
            );
        }
        temp_inst = emit_b_type_instruction(
            0x0, $2, $4, ($1).funct3, ($1).op
        );
        std::cout << "temp_inst: " << std::hex << temp_inst << std::endl;
        newElfContent.add_to_text(temp_inst);
    }
    | opcode
    {
        //syscalls
        std::cout << "ecall /syscall" << std::endl;
        temp_inst = emit_i_type_instruction(
           0,0,($1).imm12,0,($1).op
        );
        newElfContent.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
        ($1).valid = 0;
    }
    | psuedo_instruction
    {
        std::cout << "psuedo-instruction" << std::endl;
    }
    ;

psuedo_instruction:
      LA REG COMMA LABEL
    {
        std::cout << "la t0 addr" << std::endl;
        //auipc t0, %pcrel_hi(addr);
        //addi  t0, t0, %pcrel_lo(addr);
    }
    | JUMP LABEL
    {
        // alais for risc-v 32base int ISA: jal x0, label
        std::cout << "j " << $2 << std::endl;
        offset = 0;
        op_status = newElfContent.resolve_label($2, offset); 
        if (op_status == -1){
            std::cout << "no valid address found: " << offset << std::endl;
            newElfContent.add_to_unresolved_insns(
                newElfContent.get_next_insn_number(currentSection), U_TYPE, offset,
                newElfContent.get_next_insn_number(currentSection) - 1 /* add api */
            );
        }
        temp_inst = emit_u_type_instruction(0x0, 0 /*rd=x0*/, 0x6f); 

        std::cout << "temp_inst: " << std::hex << temp_inst << std::endl;
        newElfContent.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst)); 
        //TODO: remove hardcoded opcode and use an enum instead
    }
    | MOV REG COMMA REG
    {
        std::cout << "mv r"<< $2 << ", r" << $4 << std::endl;
        //TODO: make sure the MOV insturction is completely expanded
        temp_inst = emit_i_type_instruction($2, $4, 0, 0x0, 0x13);
        newElfContent.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | RET
    {
        std::cout << "jalr x0, x1, x0 [ret]" << std::endl;
        temp_inst = emit_i_type_instruction(
           0/*rd=x0*/, 1 /*rs1=x1*/, 0 /*imm*/, 0x0/*funct3*/, 0x67
        );
        newElfContent.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    ;

opcode:
      ADDI  { $$ = {.op = 0x13, .funct3 = 0x0, .valid = 1}; std::cout << "addi "; }
    | SLTI  { $$ = {.op = 0x13, .funct3 = 0x2, .valid = 1}; std::cout << "slti"; }
    | SLTIU { $$ = {.op = 0x13, .funct3 = 0x3, .valid = 1}; std::cout << "sltiu "; }
    | XORI  { $$ = {.op = 0x13, .funct3 = 0x4, .valid = 1}; std::cout << "xori "; }
    | ORI   { $$ = {.op = 0x13, .funct3 = 0x5, .valid = 1}; std::cout << "ori "; }
    | ANDI  { $$ = {.op = 0x13, .funct3 = 0x6, .valid = 1}; std::cout << "andi "; }
    | ECALL { $$ = {.op = 0x73, .imm12  = 0x0, .valid = 1}; std::cout << "ecall "; }

    | BEQ   { $$ = {.op = 0x63, .funct3 = 0x0, .valid = 1}; std::cout << "beq "; }

    | ADD   { $$ = {.op = 0x33, .funct3 = 0x0, .funct7 = 0x00, .valid = 1}; std::cout << "add "; }
    | SUB   { $$ = {.op = 0x33, .funct3 = 0x0, .funct7 = 0x20, .valid = 1}; std::cout << "sub "; }

    | LB    { $$ = {.op = 0x03, .funct3 = 0x0, .valid = 1}; std::cout << "lb "; }
    | LH    { $$ = {.op = 0x03, .funct3 = 0x1, .valid = 1}; std::cout << "lh "; }
    | LW    { $$ = {.op = 0x03, .funct3 = 0x2, .valid = 1}; std::cout << "lw "; }
    | LBU   { $$ = {.op = 0x03, .funct3 = 0x4, .valid = 1}; std::cout << "lbu "; }
    | LHU   { $$ = {.op = 0x03, .funct3 = 0x5, .valid = 1}; std::cout << "lhu "; }
    ;

register:
    REG
    {
        $$ = $1;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}

int main(int argc, char** argv){

    if (argc < 2){
        cout << "No source file passed\n" << endl;
        cout << "Usage: " << argv[0] << " file.s" << endl;
        return 1;
    }
    FILE *src = fopen(argv[1], "r");
    if (!src){
        cout << "Error: Cannot open file " << argv[1] << endl;
        return -1; //really -1?
    }

    // Set lex to read from the file instead of STDIN
    yyin = src;

    yyparse();
    while(yylex());

    //close file
    fclose(src);

    //std::string dst = "out.data";
    //write_empty_elf(dst);

    return 0;
}
