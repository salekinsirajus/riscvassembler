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
    rtype32_t    rtype_instr;
    itype32_t    itype_instr;
    size_t            offset;
    std::string   temp_value;
	std::string currentSection;
    std::string currentLabel;
%}

%union {
    int         ival;
    float       fval;
    char       *sval;
    int        token;
}

//Terminal symbols
%token <token> ADD ADDI AND SUB SUBI LI BEQ BGE BGEU
%token <token> BLT BLTU BNE LB LBU LH LHU LW OR ORI
%token <token> SB SH SLL SLLI SLT SLTI SLTIU SLTU SRA
%token <token> SRAI SRL SRLI SW XOR XORI

%token <token> ECALL

%token <token> REG
%token <token> COMMA
%token <token> COLON
%token <ival>  IMM
%token <token> PAREN_OPEN PAREN_CLOSE

%token <token> SECTION
%token <token> DIRECTIVE_COMMAND
%token <token> TEXT
%token <token> D_GLOBAL D_DATA D_TEXT D_SIZE D_RODATA D_BSS
%token <token> D_ASCII
%token <token> ID  //Might need to update this
%token <token> ASSIGNMENT

//non-terminals
%token <sval> STRING
%token <sval> LABEL
%type <ival> register;
%type <ival> imm;

%%
//actual grammar
//the rhs stuff are 1-indexed
program:
    statements
    {
        std::cout << "reading program" << std::endl;
        write_elf(newElfContent, "out.data");
    }
    ;

statements:
    statement
    | statements statement;

statement:
     LABEL COLON instructions
    {
        cout<< "LABEL: instructions" << endl;
    }
    | LABEL COLON directive
    {
        currentLabel = $1;
        std::cout << "LABEL: directive (" << currentLabel << ")" << std::endl;
        //TODO: add other type of values besides string
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
        //R-format
        //ADD SUB SLL SLT SLTU XOR SRL SRA OR AND
        std::cout << "opcode rx, ry, rz" << std::endl;
        rtype_instr.rs1 = $4;
        rtype_instr.rs2 = $6;
        newElfContent.add_to_text(rtype_instr); //TODO: add API
        std::memset(&rtype_instr, 0, sizeof(rtype_instr));
    }
    | opcode register COMMA register COMMA imm
    {
        //ADDI SUBI SLLI SLTI SLTUI XORI SRLI SRAI ORI ANDI
        std::cout << "opcode rx, ry, $1" << std::endl;
        itype_instr.rs1 = $4;
        itype_instr.imm = $6;
        newElfContent.add_to_text(itype_instr); //TODO: add API
        std::memset(&itype_instr, 0, sizeof(itype_instr));
    }
    | opcode register COMMA IMM PAREN_OPEN register PAREN_CLOSE
    {
        std::cout << "opcode rx, $8(ry)" << std::endl;
    }
    | ECALL
    {
        //is this goign to affect stuff?
        std::cout << "ecall (syscall)" << std::endl;
        newElfContent.add_to_text(rtype_instr);
        std::memset(&rtype_instr, 0, sizeof(rtype_instr));
    }
    ;

opcode:
    ADD
    {
        rtype_instr.opcode = 0b0110011;
        rtype_instr.funct7 = 0b0000000;
    }
    | SUB
    {
        rtype_instr.opcode = 0b0110011;
        rtype_instr.funct7 = 0b0100000;
    }
    | ADDI
    {
        itype_instr.opcode = 0b0010011;
    }
    | ECALL
    {
        rtype_instr.opcode = 0b1110011;
    }
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
