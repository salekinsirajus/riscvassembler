%{
    // riscvass.y - the bison file
    // for bison
    #include <stdio.h>
    #include <iostream>
    #include "src/opcodes.h"
    #include "src/encoding.h"
    #include "src/utils.h"
    #include "src/elf.h"
    #include "parser_incl.h"

    using namespace std;

    ELF32 elf;

    // flex stuff bison needs
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
    void yyerror(const char *s);
    extern int linenum;
    extern int charnum;

    // go from left to right
    uint32_t  temp_inst;

    std::string temp_value;
    std::string currentSection;
    std::string currentLabel;

    uint32_t offset;
    uint32_t temp_opcode;
    int32_t op_status;
    int64_t temp_int_literal;
    size_t  section_idx;

    std::string source_filename;
    std::string out_filename;
    std::string current_stmt; //TODO: try use flex/bison provided variables
%}

%union {
    int         ival;
    float       fval;
    char       *sval;
    int        token;
    opcode_t   opc_t;
    imm_kind   imm_k;
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

// opcode mnemonics - 32-bit base
// arithmetic/logic
%token <token> ADD ADDI AND ANDI SUB OR ORI XOR XORI
// shifts
%token <token> SLL SLLI SRL SRLI SRA SRAI
// compare
%token <token> BLT BLTU BEQ BNE BGE BGEU
// loads and stores
%token <token> LW LH LHU LB LBU SW SH SB SLT SLTI SLTIU SLTU JAL JALR
// system and memory
%token <token> ECALL EBREAK FENCE
// upper immediate
%token <token> LUI AUIPC
// psuedo instructions
%token <token> JUMP MOV RET LI LA

%token <token> SECTION
%token <token> DIRECTIVE_COMMAND
%token <token> TEXT
%token <token> D_GLOBAL D_DATA D_TEXT D_SIZE D_RODATA D_BSS D_ASCII
%token <token> PCT HI LO

//non-terminals
%token <sval> STRING
%token <sval> SYMBOL
%type <ival>  register;
%type <opc_t> opcode;    // generic opcodes
%type <opc_t> opcode_load_store opcode_immediate opcode_branch;
%type <imm_k> immediate_kind;
%type <imm_k> modifier;

%%
//actual grammar
//the rhs stuff are 1-indexed
program:
    statements
    {
        elf._resolve_unresolved_instructions();
        write_elf(elf, out_filename);
    }
    ;

statements:
    statement
    | statements statement;

statement:
     SYMBOL COLON instructions
    {
        elf.init_label($1, false/*is_global*/, currentSection);
    }
    | SYMBOL COLON directive
    {
        std::cout << "SYMBOL: directive " << std::endl;
        currentLabel = $1;
        if (temp_value.size() > 0){
            elf.add_program_data($1, temp_value, currentSection);
            temp_value = "";  //reset
        }
    }
    | directive
    ;

directive:
     SECTION D_DATA
    {
        currentSection = ".data";
        currentLabel = ".data";
    }
    | SECTION D_TEXT
    {
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
        currentSection = ".data";
        currentLabel = ".data";
    }
    | D_GLOBAL SYMBOL
    {
        elf.update_label_visibility($2, true);
    }
    | D_ASCII STRING
    {
        currentSection = ".data"; // could be .rodata
        // TODO: currentSection is a bit misleading
        temp_value = $2;
        // TODO: temp_value should be renamed to terminal value or somthing similar
        std::cout << ".ascii STRING: " << temp_value << " will be stored to " << currentSection <<  std::endl;
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
        temp_inst = emit_r_type_instruction(0, $4, $6, ($1).funct3, $2, ($1).op);
        elf.add_to_text(temp_inst); //TODO: add API

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode_immediate register COMMA register COMMA immediate_kind
    {
        //ADDI SLLI SLTI SLTUI XORI SRLI SRAI ORI ANDI
        std::cout << "I-type" << std::endl;
        current_stmt += "x" + std::to_string($2)
                     +  ", x" + std::to_string($4)
                     +  ", " + std::to_string(static_cast<int>($6));
        switch($6)
        {
            case imm_kind::IMM_INT:
                if (!is_within_range_12b(temp_int_literal)){
                   exit_with_message(linenum, charnum, source_filename, current_stmt, std::to_string(temp_int_literal) ,0);
                }
                offset = (uint32_t)(temp_int_literal & 0xFFF); // 12-bit?
                temp_inst = emit_i_type_instruction($2, $4, offset, ($1).funct3, ($1).op);
                elf.add_to_text(temp_inst); //TODO: add API
                break;
            case imm_kind::IMM_SYMBOL:
                // TODO: enforce the right kind of modifiers that can be accepted here for I-types
                std::cout << "Symbol as immediate is IN-PROGRESS" << std::endl;
                if (!elf.symbol_exists(currentLabel)){
                    std::cout << "Symbol not found" << std::endl;
                    //TODO: add to the list of symbols
                }
                else {
                    elf.resolve_symbol(currentLabel, offset, section_idx);
                    temp_inst = emit_i_type_instruction($2, $4, offset, ($1).funct3, ($1).op);
                    elf.add_to_text(temp_inst);
                }
                break;
            case imm_kind::MODIFIER_ABS_HI:
                std::cout << "\%hi() modifier NYI" << std::endl;
                break;
            default:
                std::cout << "Other imm kind: NYI" << std::endl;
                break;
        }

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode_load_store register COMMA IMM PAREN_OPEN register PAREN_CLOSE
    {
        std::cout << "r" << $2 << " " << $4 << "(r" << $6 << ")" << std::endl;
        // TODO: ensure sign-extended and unsigned are handled appropriately
        temp_opcode = ($1).op;
        if (is_load(temp_opcode))
        {
           temp_inst = emit_i_type_instruction($2, $6, $4, ($1).funct3, temp_opcode);
        }
        else
        {
           // immediate is byte-addressable so no need for alignment
           // TODO: look into word vs halfword distinctions
           temp_inst = emit_s_type_instruction($2, $6, $4, ($1).funct3, temp_opcode);
        }
        elf.add_to_text(temp_inst);

        ($1).valid = 0;
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | opcode_branch register COMMA register COMMA SYMBOL
    {
        std::cout << "B-type" << std::endl;
        // These are typically branch instructions
        std::cout << "r"<< $2 << ", r" << $4 << ", " << $6 << std::endl;
        offset = 0;
        op_status = elf.resolve_label($6, offset);
        if (op_status == -1){
            std::cout << "no valid address found: " << offset << std::endl;
            elf.add_to_unresolved_insns(
                elf.get_next_insn_number(currentSection), B_TYPE, offset,
                elf.get_next_insn_number(currentSection) - 1 /* add api */
            );
        }
        temp_inst = emit_b_type_instruction(
            0x0, $2, $4, ($1).funct3, ($1).op
        );
        std::cout << "temp_inst: " << std::hex << temp_inst << std::endl;
        elf.add_to_text(temp_inst);
    }
    | opcode
    {
        //syscalls
        std::cout << "ecall /syscall" << std::endl;
        temp_inst = emit_i_type_instruction(
           0,0,($1).imm12,($1).funct3,($1).op
        );
        elf.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
        ($1).valid = 0;
    }
    | psuedo_instruction
    {
        std::cout << "psuedo-instruction" << std::endl;
    }
    ;

psuedo_instruction:
      LA REG COMMA SYMBOL
    {
        std::cout << "la x" << $2 << " " << $4 << std::endl;
        // TODO: find the address of
        // TODO: if not, add to the unresolved instructions set
        //auipc t0, %pcrel_hi(addr);
        //addi  t0, t0, %pcrel_lo(addr);
    }
    | JUMP SYMBOL
    {
        // alias of: jal x0, label
        std::cout << "j " << $2 << std::endl;
        offset = 0;
        op_status = elf.resolve_label($2, offset);
        if (op_status == -1)
        {
            std::cout << "no valid address found: " << offset << std::endl;
            elf.add_to_unresolved_insns(
                elf.get_next_insn_number(currentSection), J_TYPE, offset,
                elf.get_next_insn_number(currentSection) - 1 /* add api */
            );
        } 
        else
        {
            offset = offset << 1; // 2-bit alignment
            offset = (uint32_t)(offset & 0x1FFFFF); // sign-check
            if (!is_within_range_21b(offset)){
               exit_with_message(linenum, charnum, source_filename, "j label", std::to_string(offset) ,0);
            }

            temp_inst = emit_j_type_instruction(offset, 0 /*rd=x0*/, JAL_32);
            std::cout << "temp_inst: " << std::hex << temp_inst << std::endl;
            elf.add_to_text(temp_inst);
            std::memset(&temp_inst, 0, sizeof(temp_inst));
        }
    }
    | MOV REG COMMA REG
    {
        std::cout << "mv x"<< $2 << ", x" << $4 << std::endl;
        //TODO: make sure the MOV insturction is completely expanded
        temp_inst = emit_i_type_instruction($2, $4, 0, 0x0, 0x13);
        elf.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    | RET
    {
        std::cout << "jalr x0, x1, x0 [ret]" << std::endl;
        temp_inst = emit_i_type_instruction(
           0/*rd=x0*/, 0 /*rs1=x0*/, 1 /*imm*/, 0x0/*funct3*/, JALR_32
        ); // FIXME: the annotations of params are wrong
        elf.add_to_text(temp_inst);
        std::memset(&temp_inst, 0, sizeof(temp_inst));
    }
    ;

opcode:
      SLTIU { $$ = {.op = SLTIU_32, .funct3 = 0x3, .valid = 1}; current_stmt = "sltiu "; }
    | ECALL { $$ = {.op = ECALL_32, .imm12  = 0x0, .valid = 1}; current_stmt =  "ecall "; }
    | ADD   { $$ = {.op = ADD_32,   .funct3 = 0x0, .funct7 = 0x00, .valid = 1}; current_stmt = "add "; }
    | SUB   { $$ = {.op = SUB_32,   .funct3 = 0x0, .funct7 = 0x20, .valid = 1}; current_stmt = "sub "; }
    ;

opcode_load_store:
      LB    { $$ = {.op = LB_32,    .funct3 = 0x0, .valid = 1}; current_stmt = "lb "; }
    | LH    { $$ = {.op = LH_32,    .funct3 = 0x1, .valid = 1}; current_stmt = "lh "; }
    | LW    { $$ = {.op = LW_32,    .funct3 = 0x2, .valid = 1}; current_stmt = "lw "; }
    | LBU   { $$ = {.op = LBU_32,   .funct3 = 0x4, .valid = 1}; current_stmt = "lbu "; }
    | LHU   { $$ = {.op = LHU_32,   .funct3 = 0x5, .valid = 1}; current_stmt = "lhu "; }
    | SB    { $$ = {.op = SB_32,    .funct3 = 0x0, .valid = 1}; current_stmt = "sb "; }
    | SH    { $$ = {.op = SH_32,    .funct3 = 0x1, .valid = 1}; current_stmt = "sh "; }
    | SW    { $$ = {.op = SW_32,    .funct3 = 0x2, .valid = 1}; current_stmt = "sw "; }
    ;

opcode_immediate:
      ADDI  { $$ = {.op = ADDI_32,  .funct3 = 0x0, .valid = 1}; current_stmt = "addi "; }
    | SLTI  { $$ = {.op = SLTI_32,  .funct3 = 0x2, .valid = 1}; current_stmt = "slti"; }
    | ORI   { $$ = {.op = ORI_32,   .funct3 = 0x5, .valid = 1}; current_stmt =  "ori "; }
    | XORI  { $$ = {.op = XORI_32,  .funct3 = 0x4, .valid = 1}; current_stmt =  "xori "; }
    | ANDI  { $$ = {.op = ANDI_32,  .funct3 = 0x6, .valid = 1}; current_stmt =  "andi "; }
    ;

opcode_branch:

    BEQ   { $$ = {.op = BEQ_32,   .funct3 = 0x0, .valid = 1}; current_stmt =  "beq "; }
    ;

register:
    REG
    {
        $$ = $1;
    }
    ;

immediate_kind:
    IMM 
    {
       // TODO: call it something else instead of IMM, it's too loaded a term
       currentLabel = "";
       temp_int_literal = $1;
       $$ = imm_kind::IMM_INT;
    }
    | modifier
    {
       std::cout << "a modifier" << std::endl;
    }
    | SYMBOL
    {
       std::cout << "a symbol" << std::endl;
       currentLabel = $1;
       $$ = imm_kind::IMM_SYMBOL;
    }
    ;

modifier:
    PCT HI PAREN_OPEN SYMBOL PAREN_CLOSE
    {
        // should it be label or symbol
        currentLabel = $4;
        $$ = imm_kind::MODIFIER_ABS_HI;
    }

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(int argc, char** argv){

    if (argc < 2){
        cout << "No source file passed\n" << endl;
        cout << "Usage: " << argv[0] << " file.s -o out.o" << endl;
        return 1;
    }
    FILE *src = fopen(argv[1], "r");
    if (!src){
        cout << "Error: Cannot open file " << argv[1] << endl;
        return -1; //really -1?
    }

    if (argc == 4)
    {
       out_filename = argv[3];
    }
    else
    {
       // set source and output filenames
       source_filename = std::string(argv[1]);
       out_filename = generate_dest_filename(source_filename);
    }

    // Set lex to read from the file instead of STDIN
    yyin = src;

    yyparse();
    while(yylex());

    fclose(src);

    return 0;
}
