%{
    // riscvass.y - the bison file
    //for bison
    #include <stdio.h>
    #include <iostream>
    #include "encoding.h"
    #include "utils.h"
    #include "linux/elf.h"

    using namespace std;

    ELF32 elfContent;
    //flex stuff bison needs
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;

    void yyerror(const char *s);
    //go from left to right
    rtype32_t rtype_instr;
    itype32_t itype_instr;
    size_t  offset;
    Section currentSection;
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
%token <token> LABEL
%token <token> ID
%token <token> ASSIGNMENT

//non-terminals
%token <sval> STRING
%type <ival> register;
%type <ival> imm;

%%
//actual grammar
//the rhs stuff are 1-indexed
program:
    statements
    {
        std::cout << "reading the section" << std::endl;
        write_elf(elfContent, "out.data");
    }
    ;

statements:
    statement
    | statements statement;

statement:
     LABEL COLON instructions
    {
        cout<< ">> evaluating statement: instruction" << endl;
    }
    | LABEL directive
    {
        cout << ">> evaluating statement: directive" << endl;
    }
    | directive
    {
        cout << ">> evaluating directive" << std::endl;
    }
    ;

directive:
     SECTION D_DATA
    {
        std::cout << "section .data" << std::endl;
    }
    | D_GLOBAL LABEL
    {
        std::cout << ".globl LABEL" << std::endl;
    }
    | D_TEXT
    {
        std::cout << ".text (emit and make current) " << std::endl;
    }
    | D_DATA
    {
        std::cout << ".data (emit and make current) " << std::endl;
        initialize_data_section(elfContent);
    }
    | D_ASCII STRING
    {
        printf("Store this string: %s\n", $2);
        Elf32_Sym currentString = {0, 0x1000, 4, 0, 0, 1};
        elfContent.symtab.push_back(currentString);
        store_string(elfContent, $2); //TODO: check whether it worked or failed
    }
    | ASSIGNMENT D_ASCII STRING
    {
        //same as above
        printf("assign this string to the var %s\n", $3);
        Elf32_Sym currentString = {0, 0x1000, 4, 0, 0, 1};
        elfContent.symtab.push_back(currentString);
        store_string(elfContent, $3); //TODO: check whether it worked or failed
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
        std::cout << "R-format instruction " << std::endl;
        rtype_instr.rs1 = $4;
        rtype_instr.rs2 = $6;
        elfContent.text->data.push_back(rtype_instr);
        std::cout << "instr: " << rtype_instr << std::endl;
        std::memset(&rtype_instr, 0, sizeof(rtype_instr));
    }
    | opcode register COMMA register COMMA imm
    {
        //cout << ">>>> op r, r, imm" << endl;
        //ADDI SUBI SLLI SLTI SLTUI XORI SRLI SRAI ORI ANDI
        std::cout << "I-format instruction" << std::endl;
        itype_instr.rs1 = $4;
        itype_instr.imm = $6;
        elfContent.text->data.push_back(itype_instr);
        std::cout << "instr: " << itype_instr << std::endl;
        std::memset(&itype_instr, 0, sizeof(itype_instr));
    }
    | opcode register COMMA IMM PAREN_OPEN register PAREN_CLOSE
    {
        std::cout << "S-format instruction " << std::endl;
    }
    | ECALL
    {
        //is this goign to affect stuff?
        std::cout << "ecall (syscall) invocation" << std::endl;
        std::cout << "instr: " << rtype_instr << std::endl;
        elfContent.text->data.push_back(rtype_instr);
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
    initialize_elf(elfContent);

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
