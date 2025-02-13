#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdint>
#include "elf.h"

// Make sure all the stuff is in place
void initialize_elf(ELF32& elf) {
    Elf32_Ehdr& elf_header = elf.elf_header;
    memset(&elf_header, 0, sizeof(elf_header));
    /*=============== ELF Header START ===============*/
    elf_header.e_ident[0] = 0x7f;
    elf_header.e_ident[1] = 'E';
    elf_header.e_ident[2] = 'L';
    elf_header.e_ident[3] = 'F';
    elf_header.e_ident[4] = 1;        // 32-bit
    elf_header.e_ident[5] = 1;        // Little-endian
    elf_header.e_ident[6] = 1;        // ELF version
    elf_header.e_type = ET_REL;       // Object file
    elf_header.e_machine = 0xF3;      // RISC-V (0xF3)
    elf_header.e_version = 1;         // ELF version
    // TODO Entry point - does an assembler make the decision
	// to set the entry point, such as _main? Generally, it
	// is the beginning of the text section
    elf_header.e_entry = 0;
    // TODO Program header table offset
    elf_header.e_phoff = 0;
    // TODO Section header table offset
    elf_header.e_shoff = 0;
    elf_header.e_flags = 0;
    //Usually 64 bytes in 64-bit ELF and 52 bytes for 32 bits)
    elf_header.e_ehsize = sizeof(Elf32_Ehdr); //52
    elf_header.e_phentsize = sizeof(Elf32_Phdr);
    elf_header.e_phnum = 1;           // Number of program headers
    elf_header.e_shentsize = sizeof(Elf32_Shdr); 
                                      // Section header entry size 
    elf_header.e_shnum = 1;           // Number of section headers
    elf_header.e_shstrndx = 0;        // Section header string index??
    /*================ ELF Header END ==================*/

    /*============== Program Header START ==============*/
    Elf32_Phdr prog_header;
    memset(&prog_header, 0, sizeof(prog_header));
    prog_header.p_type = PT_LOAD;    // Loadable segment
    prog_header.p_flags = PF_X | PF_R;  // Execute + Read
    prog_header.p_offset = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);  // Offset in file
    prog_header.p_vaddr = 0x1000;   // Virtual address in memory (just a dummy)
    prog_header.p_paddr = 0;        // Physical address (not used)
    prog_header.p_filesz = 0;       // Size of segment in file (empty)
    prog_header.p_memsz = 0;        // Size of segment in memory (empty)
    prog_header.p_align = 0x1000;   // Alignment (0x1000 bytes)
    /*================ Program Header END ==============*/

    elf.program_headers.push_back(prog_header);

	elf.data   = nullptr;
	elf.text   = nullptr;
	elf.bss    = nullptr;
	elf.rodata = nullptr;

	initialize_symbol_table(elf);
	initialize_string_table(elf);
}

void initialize_symbol_table(ELF32& elf){
	Elf32_Shdr symtab_sh;
	std::memset(&symtab_sh, 0, sizeof(Elf32_Shdr));
	symtab_sh.sh_size = 12; 	//TODO: placeholder
	symtab_sh.sh_type = SHT_SYMTAB;
	
	elf.section_headers.push_back(symtab_sh);
	elf.elf_header.e_shnum += 1; //make sure its posivite

	Elf32_Sym undefined;
	std::memset(&undefined, 0, sizeof(Elf32_Sym));
}

void initialize_string_table(ELF32& elf){
	Elf32_Shdr strtab_sh;
	std::memset(&strtab_sh, 0, sizeof(Elf32_Shdr));
	strtab_sh.sh_size = 10;		//TODO: placeholder
	strtab_sh.sh_type = SHT_STRTAB;

	elf.section_headers.push_back(strtab_sh);
	elf.elf_header.e_shnum += 1;
	//anything else?
}

void write_empty_elf(ELF32& elf, std::string filename) {
    std::ofstream elf_file(filename, std::ios::out | std::ios::binary);
    
    if (!elf_file) {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    // Write ELF header
    elf_file.write(
		reinterpret_cast<const char*>(&(elf.elf_header)), sizeof(Elf32_Ehdr)
	);
	// Program headers (since they are optional, we are skipping them)

	// Then Sections
	for (std::vector<Section>::iterator it=elf.sections.begin(); it != elf.sections.end(); ++it){
		elf_file.write(
			reinterpret_cast<const char*>(&(*it).data), sizeof(&(*it).data)
		);
	}

	// Then Section Headers
	for (std::vector<Elf32_Shdr>::iterator it=elf.section_headers.begin(); it != elf.section_headers.end(); ++it){
		elf_file.write(
			reinterpret_cast<const char*>(&(*it)), sizeof(&(*it))
		);
	}

	// TODO: Update all the offsets here
	/*
	size_t sh_offset = sizeof(Elf32_Ehdr);
	for (std::vector<Section>::iterator it=elf.sections.begin(); it != elf.sections.end(); ++it){
		sh_offset += it->data.size();
		std::cout << "it->data.size(): " << it->data.size() << std::endl;
	}
	elf.elf_header.e_shoff = (sh_offset / sizeof(const char));
	std::cout << "e_shoff: " << elf.elf_header.e_shoff << std::endl;
	*/

    elf_file.close();

    if (!elf_file.good()) {
        std::cerr << "Error occurred at writing time!\n";
    } else {
        std::cout << "ELF file written successfully.\n";
    }
}
