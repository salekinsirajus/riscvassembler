%{
	//for bison
	#include <stdio.h>
	#include <iostream>
	#include "encoding.h"
	#include "utils.h"

	using namespace std;

	//flex stuff bison needs
	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;

	void yyerror(const char *s);
	//go from left to right
	inst32_t add_instr;
%}

%union {
	int			ival;
	float 		fval;
	char	   *sval;
    int        token;
}

//Terminal symbols
%token <token> ADD ADD_IMM
%token <token> SUB SUB_IMM
%token <token> SLL SLT AND XOR

%token <token> REG
%token <token> COMMA
%token <ival>  IMM

//non-terminals
%type <ival> register;
%type <ival> imm;

%%
//actual grammar
program:
	  instruction  { cout<< "evaluating instruction" << endl; }
	| program instruction { cout << "recursive program" << endl; }
    ;

instruction:
	operand register COMMA register COMMA register
	{ 
		cout << "evaluating instruction" << endl; 
		add_instr.rs1 = $4;
		add_instr.rs2 = $6;
		print_instruction(add_instr);
	}
	| operand register COMMA register COMMA imm {
		cout << "op r, r, imm" << endl;
	}
	;

operand:
	ADD { 
			cout << "ADD_OP" << endl; 
			add_instr.opcode = 0b0110011; 
		}
	;

register:
	REG { 
		cout << "REG " << $1 << endl;
		$$ = $1;
		}
	;
imm:
	IMM {
		cout << "IMM " << $1 << endl;
	}

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}
