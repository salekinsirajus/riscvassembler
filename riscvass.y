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

    size_t            offset;
    std::string   temp_value;
    std::string currentSection;
    std::string currentLabel;

    std::vector<uint32_t> stk; //unused
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
%type <ival>  imm;
%type <opc_t> opcode;

%%
//actual grammar
//the rhs stuff are 1-indexed
program:
    statements
    {
        std::cout << "reading program" << std::endl;
        newElfContent.resolve_forward_decls();
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
        //TODO: check if the label exists
        newElfContent.store_label($1, false/*is_global*/);
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
        std::cout << "directive" << std::endl;
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
        newElfContent.store_label($2, true);
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
        std::cout << "opcode rx, ry, rz" << std::endl;
        temp_inst = emit_r_type_instruction(0, $4, $6, 0, $2, ($1).op);
        newElfContent.add_to_text(temp_inst); //TODO: add API

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode register COMMA register COMMA imm
    {
        //ADDI SUBI SLLI SLTI SLTUI XORI SRLI SRAI ORI ANDI
        std::cout << "opcode rx, ry, imm" << std::endl;
        temp_inst = emit_i_type_instruction($2, $4, $6, 0, ($1).op);
        newElfContent.add_to_text(temp_inst); //TODO: add API

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode register COMMA IMM PAREN_OPEN register PAREN_CLOSE
    {
        std::cout << "opcode rx, $8(ry)" << std::endl;
    }
    | opcode register COMMA register COMMA LABEL
    {
        std::cout << "opcode x1, x2, label" << std::endl;
        offset = newElfContent.resolve_label($6);
        temp_inst = emit_b_type_instruction(
            offset, $2, $4, ($1).funct3, ($1).op
        );
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
    | LB REG COMMA LABEL
    {
        std::cout << "lb rx, label" << std::endl;
        //TODO: differentiate between label and symbol
    }
    | JUMP LABEL
    {
        std::cout << "j label" << std::endl;
        // alais for risc-v 32base int ISA: jal x0 label
        offset = newElfContent.resolve_label($2); 
        std::cout << "resolved label to: " << offset << std::endl;
        // TODO: encode this
    }
    | MOV REG COMMA REG
    {
        std::cout << "mv rd, rs" << std::endl;
        //addi rd, rs, x0
        temp_inst = emit_i_type_instruction($2, $4, 0, 0x0/*funct3*/, 0x13/*addi*/);
        newElfContent.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | RET
    {
        std::cout << "jalr x0, x1, x0" << std::endl;
    }
    ;

opcode:
      ADDI  { $$ = {.op = 0x13, .funct3 = 0x0, .valid = 1}; }
    | SLTI  { $$ = {.op = 0x13, .funct3 = 0x2, .valid = 1}; }
    | SLTIU { $$ = {.op = 0x13, .funct3 = 0x3, .valid = 1}; }
    | XORI  { $$ = {.op = 0x13, .funct3 = 0x4, .valid = 1}; }
    | ORI   { $$ = {.op = 0x13, .funct3 = 0x5, .valid = 1}; }
    | ANDI  { $$ = {.op = 0x13, .funct3 = 0x6, .valid = 1}; }

    | BEQ   { $$ = {.op = 0x63, .funct3 = 0x0, .valid = 1}; }

    | ADD   { $$ = {.op = 0x33, .funct3 = 0x0, .funct7 = 0x00, .valid = 1}; }
    | SUB   { $$ = {.op = 0x33, .funct3 = 0x0, .funct7 = 0x20, .valid = 1}; }
    | ECALL { $$ = {.op = 0x73, .imm12  = 0x0, .valid = 1}; }
    ;

register:
    REG
    {
        $$ = $1;
    }
    ;
imm:
    IMM {
        $$ = $1;
    }

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
