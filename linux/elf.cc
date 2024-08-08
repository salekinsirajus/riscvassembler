#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include "elf.h"


void write_empty_elf(const char* filename) {
    std::ofstream elf_file(filename, std::ios::out | std::ios::binary);
    
    if (!elf_file) {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    Elf64_Ehdr elf_header;
    memset(&elf_header, 0, sizeof(elf_header));
    
    // ELF magic number
    elf_header.e_ident[0] = 0x7f;
    elf_header.e_ident[1] = 'E';
    elf_header.e_ident[2] = 'L';
    elf_header.e_ident[3] = 'F';
    elf_header.e_ident[4] = 2;  // 64-bit
    elf_header.e_ident[5] = 1;  // Little-endian
    elf_header.e_ident[6] = 1;  // ELF version
    elf_header.e_type = 2;      // Executable file
    elf_header.e_machine = 0xF3;// RISC-V (0xF3)
    elf_header.e_version = 1;   // ELF version
    elf_header.e_entry = 0;     // Entry point
    elf_header.e_phoff = sizeof(Elf64_Ehdr); // Program header table offset
    elf_header.e_shoff = 0;     // Section header table offset (not used)
    elf_header.e_flags = 0;
    elf_header.e_ehsize = sizeof(Elf64_Ehdr);
    elf_header.e_phentsize = sizeof(Elf64_Phdr);
    elf_header.e_phnum = 1;     // Number of program headers
    elf_header.e_shentsize = 0;
    elf_header.e_shnum = 0;
    elf_header.e_shstrndx = 0;

    Elf64_Phdr prog_header;
    memset(&prog_header, 0, sizeof(prog_header));
    prog_header.p_type = 1;    // Loadable segment
    prog_header.p_flags = 5;   // Execute + Read
    prog_header.p_offset = 0;  // Offset in file
    prog_header.p_vaddr = 0;   // Virtual address in memory
    prog_header.p_paddr = 0;   // Physical address (not used)
    prog_header.p_filesz = 0;  // Size of segment in file
    prog_header.p_memsz = 0;   // Size of segment in memory
    prog_header.p_align = 0x1000; // Alignment

    // Write headers to file
    elf_file.write(reinterpret_cast<const char*>(&elf_header), sizeof(elf_header));
    elf_file.write(reinterpret_cast<const char*>(&prog_header), sizeof(prog_header));

    elf_file.close();

    if (!elf_file.good()) {
        std::cerr << "Error occurred at writing time!\n";
    } else {
        std::cout << "ELF file written successfully.\n";
    }
}

int main() {
    write_empty_elf("empty.elf");
    return 0;
}
