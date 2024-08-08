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
%token <token> ADD_OP
%token <token> REG
%token <token> COMMA

//non-terminals
%type <ival>  register;

%%
//actual grammar
expression:
	instruction  { cout<< "evaluating expression" << endl; }
    ;

instruction:
	operand register COMMA register COMMA register
	{ 
		cout << "evaluating instruction" << endl; 
		add_instr.rs1 = $4;
		add_instr.rs2 = $6;
		print_instruction(add_instr);
	}
	;

operand:
	ADD_OP       { 
			cout << "ADD_OP" << endl; 
			add_instr.opcode = 0b0110011; 
		}
	;

register:
	REG          { 
		cout << "REG " << $1 << endl;
		$$ = $1;
	}
	;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}
