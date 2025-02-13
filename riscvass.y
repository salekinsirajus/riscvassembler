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
    inst32_t instr;
    Section currentSection;
%}

%union {
    int         ival;
    float       fval;
    char       *sval;
    int        token;
}

//Terminal symbols
%token <token> ADD ADD_IMM
%token <token> SUB SUB_IMM
%token <token> SLL SLT AND XOR
%token <token> LOAD_IMM
%token <token> ECALL

%token <token> REG
%token <token> COMMA
%token <token> COLON
%token <ival>  IMM

%token <token> SECTION
%token <token> DIRECTIVE_COMMAND
%token <token> TEXT
%token <token> D_GLOBAL D_DATA D_TEXT D_SIZE D_RODATA D_BSS
%token <token> D_ASCII
%token <token> LABEL LABELLED
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
        for (std::vector<uint32_t>::iterator it=currentSection.data.begin();
             it != currentSection.data.end();
             ++it){
            std::cout << std::hex << *it << std::endl;
        }
        write_empty_elf(elfContent, "out.data");
    }
    ;

statements:
    statement
    | statements statement;

statement:
     LABEL instructions
        {
            cout<< ">> evaluating statement: instruction" << endl;
        }
    | LABEL directive
        {
            cout << ">> evaluating statement: directive" << endl;
        }
    ;

directive:
    SECTION D_TEXT
    {
        cout << ">> hardcoded .section .text" << endl;
        Section textSection;
        currentSection = textSection;
        Elf32_Shdr currentSectionHeader;

        currentSectionHeader.sh_type = SHT_PROGBITS;
        elfContent.section_headers.push_back(currentSectionHeader);
		elfContent.text = &textSection;
    }
    | SECTION D_DATA
    {
        std::cout << "section .data" << std::endl;
    }
    | D_GLOBAL ID
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
    }
    | D_ASCII STRING
    {
        //this will most likely be stored in the strtab
		//also in the symbol table
        printf("Store this string: %s\n", $2);
		Elf32_Sym currentString = {0, 0x1000, 4, 0, 0, 1};
		elfContent.symtab.push_back(currentString);
    }
    | ASSIGNMENT D_ASCII STRING
    {
		//same as above
        printf("assign this string to the var %s\n", $3);
    }
    ;

instructions:
	instruction
	| instructions instruction
	;

instruction:
    operand register COMMA register COMMA register
    {
        //cout << ">>>> evaluating instruction" << endl;
        instr.rs1 = $4;
        instr.rs2 = $6;

        //print_instruction_hex(instr);
		if (!elfContent.text){
			Section text;
			Elf32_Shdr text_sh;
			text_sh.sh_type = SHT_PROGBITS;
			elfContent.section_headers.push_back(text_sh);
			elfContent.text = &text;
		}

        elfContent.text->data.push_back(instr);
        std::memset(&instr, 0, sizeof(instr));
    }
    | operand register COMMA register COMMA imm
    {
        //cout << ">>>> op r, r, imm" << endl;
    }
    ;

operand:
    ADD {
            instr.opcode = 0b0110011;
        }
    | SUB
        {
            //cout << ">>>>>> SUB_OP" << endl;
            instr.opcode = 0b0110100;
        }
    ;

register:
    REG
        {
            //cout << ">>>>>>>> REG " << $1 << endl;
            $$ = $1;
        }
    ;
imm:
    IMM {
        //cout << ">>>>>>>> IMM " << $1 << endl;
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
