%{
    // riscvass.y - the bison file
    //for bison
    #include <stdio.h>
    #include <iostream>
    #include "encoding.h"
    #include "utils.h"
    #include "linux/elf.h"

    using namespace std;

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
    int            ival;
    float         fval;
    char       *sval;
    int        token;
}

//Terminal symbols
%token <token> ADD ADD_IMM
%token <token> SUB SUB_IMM
%token <token> SLL SLT AND XOR

%token <token> REG
%token <token> COMMA
%token <token> COLON
%token <ival>  IMM

%token <token> SECTION
%token <token> TEXT
%token <token> GLOBAL
%token <token> START 
%token <token> LABEL

//non-terminals
%type <ival> register;
%type <ival> imm;

%%
//actual grammar
program:
    statements 
    {
        std::cout << "reading the section" << std::endl;
        for (std::vector<uint32_t>::iterator it=currentSection.data.begin(); 
             it != currentSection.data.end(); 
             ++it){
            std::cout << std::hex << *it << std::endl;
        }
    }
    ;

statements:
    statement
    | statement statements;

statement:
     instruction  
        { 
            cout<< ">> evaluating statement: instruction" << endl; 
        }
    | directive
        { 
            cout << ">> evaluating statement: directive" << endl;
        }
    ;

directive: 
    SECTION TEXT
    {
        cout << ">> hardcoded .section .text" << endl;
        Section textSection;
        currentSection = textSection;
    }
    | GLOBAL START {
        cout << ">> hardcoded .globl .start" << endl;
    } 
    | LABEL {
        cout << ">> identify the label: " << endl;
    }
    ;

instruction:
    operand register COMMA register COMMA register
    { 
        cout << ">>>> evaluating instruction" << endl; 
        instr.rs1 = $4;
        instr.rs2 = $6;

        print_instruction_hex(instr);
        currentSection.data.push_back(instr);
        std::memset(&instr, 0, sizeof(instr));
    }
    | operand register COMMA register COMMA imm {
        cout << ">>>> op r, r, imm" << endl;
    }
    ;

operand:
    ADD { 
            cout << ">>>>>> ADD_OP" << endl; 
            instr.opcode = 0b0110011; 
        }
    | SUB 
        {
            cout << ">>>>>> SUB_OP" << endl; 
            instr.opcode = 0b0110100;
        }
    ;

register:
    REG 
        { 
            cout << ">>>>>>>> REG " << $1 << endl;
            $$ = $1;
        }
    ;
imm:
    IMM {
        cout << ">>>>>>>> IMM " << $1 << endl;
    }

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}
