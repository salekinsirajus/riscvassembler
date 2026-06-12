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

This will result in an object file called `tests/helloworld.o`. you
can open it with a hex viewer like `xxd test/helloworld.o` to dump
the content.

## IN PROGRESS
- [x] Refactor Sections, string tables and symbol tables to inherit from the same section class
- [x] Make sure an a string literal needed during runtimg goes into either .data or .rodata. (it goes to .data now)
- [ ] Implement a method to store int, float to program data

## TODO
- [ ] Refactor StringTable so that it always stays in a valid state
- [ ] Refactor ELF32 serialization to correctly update offsets, sizes, and section headers dynamically.
- [ ] Add unresolved symbols to rela.text
- [ ] Implement .word, .byte, .half, .long directives for data initialization.
- [ ] Implement .align directive for section alignment.
- [ ] Implement label/symbol visibility
- [ ] implement full RV32I instruction coverage (LUI, AUIPC, JALR, all arithmetic, shifts, branches, loads/stores).
- [ ] Implement pseudo-instructions: LI, LA, NEG, NOT, SEQZ, SNEZ, SGTZ, etc.
- [ ] Implement proper pseudo-instruction expansion to multiple actual instructions (e.g., LI → LUI+ADDI).
- [ ] verify proper handling of branch offsets for B-type and J-type instructions.
- [ ] Implement a deserialize function for J-type
- [ ] Carve out a space for adding custom instruction
- [ ] Implement .bss section with proper ELF flags and handling of uninitialized data.
- [ ] Implement .rodata section for read-only data
- [ ] Implement .comm and .size directives.
- [ ] Implement .equ/.set directives for defining constants.
- [ ] Extend label handling to support multiple sections (text, data, rodata, bss).
- [ ] Implement arithmetic expressions for immediates and label offsets.
- [ ] Implement proper handling of global vs local symbols in the symbol table.
- [ ] Improve error handling with warnings for undefined symbols.
- [ ] Add logging/debugging support for unresolved instructions and symbol resolution.
- [ ] Refactor error reporting/debug mode with multiple output streams so they can be turned on or off as needed
- [ ] add a test suite and example output files
- [ ] Implement ECALL and system instruction handling fully with proper encoding.
- [ ] Add support for forward-declared labels in data sections.
- [ ] Add support for effectively required extensions
- [ ] Spike: consider adding a middle layer/IR/AST between Parsing and Codegen
- [ ] Implement macro (?)
