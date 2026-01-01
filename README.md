# RISC-V Assembler
A reference assembler for RISC-V backend. Primarily for learning
and hacking. Implements the grammar described in the 
[riscv-asm-manual](https://github.com/riscv-non-isa/riscv-asm-manual/blob/main/src/asm-manual.adoc).
ELF spec reference is [here](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-elf.adoc).

## How to build
Make sure `flex` and `bison` are present in your system.
```
make
```
## How to run
The test files are under `tests`. Once the build is made,
use one of the assembly files from there to generate an
object file.
```
./riscvass tests/helloworld.s
```

This will result in an object file called `out.o`. you
can open it with a hex viewer like `xxd out.o` to see
the content.


## Work in progress
- [x] initialize symbol table and string table
- [x] add entry to the strtab
- [x] proper offset calculation for section headers
- [x] add entry to the symtab
- [x] add instructions to the text section
- [x] address/ref resolution (single pass.)
- [x] Expand lexer to recognize all registers x0-x31 
- [x] Get lexer to recongize all aliases (t0-t6, s0-s11, a0-a7)
- [x] Implement alias recognition for commonly used registers (ra, sp, gp, tp)
- [ ] Extend immediate parsing to support negative numbers, hexadecimal (0x...), binary (0b...), and larger constants.
- [ ] Fix the offsets (sign-extension, alignment, rang-check) outside the emit functions
- [ ] Implement a deserialize function for J-type
- [ ] Implement label/symbol visibility
- [ ] Add unresolved symbols to rela.text 
- [ ] implement full RV32I instruction coverage (LUI, AUIPC, JALR, all arithmetic, shifts, branches, loads/stores).
- [ ] Implement proper pseudo-instruction expansion to multiple actual instructions (e.g., LI → LUI+ADDI).
- [ ] Implement .bss section with proper ELF flags and handling of uninitialized data.
- [ ] Implement .rodata section for read-only data
- [ ] Implement .word, .byte, .half, .long directives for data initialization.
- [ ] Implement .align directive for section alignment.
- [ ] Implement .comm and .size directives.
- [ ] Implement .equ/.set directives for defining constants.
- [ ] Extend label handling to support multiple sections (text, data, rodata, bss).
- [ ] Implement pseudo-instructions: LI, LA, NEG, NOT, SEQZ, SNEZ, SGTZ, etc.
- [ ] Implement macro (?)
- [ ] Update ELF header fields dynamically: e_shnum, e_shstrndx, e_shoff, and section flags.
- [ ] Implement arithmetic expressions for immediates and label offsets.
- [ ] Implement proper handling of global vs local symbols in the symbol table.
- [ ] Improve error handling with line numbers, semantic checks, and warnings for undefined symbols.
- [ ] add a test suite and example output files
- [ ] Implement ECALL and system instruction handling fully with proper encoding.
- [ ] Add support for forward-declared labels in data sections.
- [ ] Refactor ELF32 serialization to correctly update offsets, sizes, and section headers dynamically.
- [ ] verify proper handling of branch offsets for B-type and J-type instructions.
- [ ] Add logging/debugging support for unresolved instructions and symbol resolution.
- [ ] Add support for effectively required extensions
