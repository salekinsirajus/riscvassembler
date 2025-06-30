# RISC-V Assembler
An assembler for RISC-V backend. Primarily for learning
and hacking.

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

This will result in an object file called `out.data`. you
can open it with a hex viewer like `xxd out.data` to see
the content.

## Features
aka the work in progress
- [x] initialize symbol table and string table
- [x] add entry to the strtab
- [x] proper offset calculation for section headers
- [x] add entry to the symtab
- [x] add instructions to the text section
- [ ] Implement all ISA instructions
- [ ] Implement macro/psuedo instructions
- [ ] address/ref resolution (single or multi-pass?)
- [ ] relocation info
- [ ] Use the symbol table to the full capacity
- [ ] directive support
