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
./riscvass tests/add.s
```
