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

## WIP: Reference Resolution
_This portion is for scratch notes as I am working through
a feature or a component_

- Added a map for keeping track of forward declarations
- Calling a function at the end to resolve them
- However, I don't know how to deserialize, update addr, and reserialize

## Features
aka the work in progress
- [x] initialize symbol table and string table
- [x] add entry to the strtab
- [x] proper offset calculation for section headers
- [x] add entry to the symtab
- [x] add instructions to the text section
- [ ] address/ref resolution (single or multi-pass?)
- [ ] Implement all ISA instructions
- [ ] Implement macro/psuedo instructions
- [ ] relocation info
- [ ] Use the symbol table to the full capacity
- [ ] directive support
