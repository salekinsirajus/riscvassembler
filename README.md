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

## Limitations
aka the work in progress
- [ ] initialize symbol table and string table
- [ ] add entry to the symtab and strtab
- [ ] address/ref resolution (single or multi-pass?)
- [ ] implement all instruction formats
- [ ] proper offset calculation
- [ ] relocation info
- [ ] directive support
