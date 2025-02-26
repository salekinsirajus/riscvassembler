#include <iostream>
#include <vector>
#include <sstream>
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
    elf_header.e_phnum = 0;           // Number of program headers
    elf_header.e_shentsize = sizeof(Elf32_Shdr); 
                                      // Section header entry size 
    elf_header.e_shnum = 1;           // Number of section headers
    elf_header.e_shstrndx = 0;        // Section header string index??
    /*================ ELF Header END ==================*/

    elf.data   = nullptr;
    elf.text   = nullptr;
    elf.bss    = nullptr;
    elf.rodata = nullptr;

    initialize_symbol_table(elf);
    initialize_string_table(elf);
    initialize_text_section(elf);
}

void initialize_data_section(ELF32& elf){
    Section* data = new Section();

    Elf32_Shdr* data_sh = new Elf32_Shdr();
    data_sh->sh_type = SHT_PROGBITS;
    elf.data = data;
    data->header = data_sh;

    elf.sections.push_back(data);      //is this working?
    std::cout << "data_sh " << data_sh->sh_name << std::endl;
    elf.section_headers.push_back(data_sh);
}

void initialize_text_section(ELF32& elf){
    Section* text = new Section();

    Elf32_Shdr* text_sh = new Elf32_Shdr();
    text_sh->sh_type = SHT_PROGBITS;
    elf.text = text;
    text->header = text_sh;

    elf.sections.push_back(text);
    std::cout << "text_sh " << text_sh->sh_name << std::endl;
    elf.section_headers.push_back(text_sh);

}

void initialize_symbol_table(ELF32& elf){
    Elf32_Shdr* symtab_sh = new Elf32_Shdr();
    std::memset(symtab_sh, 0, sizeof(Elf32_Shdr));

    symtab_sh->sh_size = 0;     //TODO: update w/ actual count
    symtab_sh->sh_type = SHT_SYMTAB;
    std::cout << "symtab_sh " << symtab_sh->sh_name << std::endl;
    elf.section_headers.push_back(symtab_sh);

    Elf32_Sym undefined;
    std::memset(&undefined, 0, sizeof(Elf32_Sym));

    //TODO: add to the Section object
    //TODO: add the actual symbol table section to the sections
}

void initialize_string_table(ELF32& elf){
    Elf32_Shdr* strtab_sh = new Elf32_Shdr();
    std::memset(strtab_sh, 0, sizeof(Elf32_Shdr));
    strtab_sh->sh_size = 0;
    strtab_sh->sh_type = SHT_STRTAB;

    //linking the section header to the section so it can be
    //located easily
    elf.strtab.header = strtab_sh;

    std::cout << "strtab_sh " << strtab_sh->sh_name << std::endl;
    elf.section_headers.push_back(strtab_sh);
    //NOTE: string table gets added seperately
	//TODO: add the initial null terminator
}

size_t store_string(ELF32& elf, std::string the_string){
    //strtab contains null-terminated string

    size_t offset = elf.strtab.add_string(the_string.c_str());
    //symtab holds the metadata and index about the string
    elf.strtab.header->sh_size += 1;

    return offset;
}

size_t store_label(ELF32& elf, std::string the_label, bool is_global){
	size_t offset = store_string(elf, the_label);

	Elf32_Sym sym;
	sym.st_name = offset;		// index into the string table
	if (is_global){
		sym.st_info = ELF32_ST_BIND(STB_GLOBAL);
	} else {
		sym.st_info = ELF32_ST_BIND(STB_LOCAL);
	}

	elf.symtab.push_back(sym);	
	return offset;
}

void write_elf(ELF32& elf, std::string filename) {
    // Create an ostringstream to accumulate the byte stream
    std::ostringstream elf_stream(std::ios::binary);
    std::streampos pos = sizeof(Elf32_Ehdr);
    std::cout << "position after ELF header: " << pos << std::endl;

    // Add sections
    std::cout << "# of sections: " << elf.sections.size() << std::endl;
    for (std::vector<Section *>::iterator it = elf.sections.begin(); it != elf.sections.end(); ++it) {
        size_t data_size = ((*it)->data.size() * sizeof(uint32_t));
        elf_stream.write(reinterpret_cast<const char*>((*it)->data.data()), data_size);
        (*it)->header->sh_offset = pos;
        pos += data_size;
        std::cout << "position after section : " << pos << std::endl;
    }

    std::cout << "beginning of the section header here: " << pos << std::endl;
    elf.elf_header.e_shoff = static_cast<uint32_t>(pos);        //OFF BY ONE?
    elf.elf_header.e_shnum = elf.section_headers.size();
    std::cout << "# of section headers: " << elf.section_headers.size() << std::endl;

    // Add an empty header first?
    Elf32_Shdr first_entry;
    std::memset(&first_entry, 0, sizeof(Elf32_Shdr));
    elf_stream.write(reinterpret_cast<const char*>(&first_entry), sizeof(Elf32_Shdr));
    pos += sizeof(Elf32_Shdr);

    std::cout << "size of a section header: " << sizeof(Elf32_Shdr) << std::endl;
    for (std::vector<Elf32_Shdr *>::iterator it = elf.section_headers.begin(); it != elf.section_headers.end(); ++it){
        std::cout << "sh_name " << (*it)->sh_name << std::endl;
        elf_stream.write(reinterpret_cast<const char*>((*it)), sizeof(Elf32_Shdr));
        pos += sizeof(Elf32_Shdr);
        std::cout << "position after section header: " << pos << std::endl;
    }

    // Add strtab
    elf_stream.write(reinterpret_cast<const char*>(elf.strtab.get_all()), elf.strtab.get_size());
    pos += elf.strtab.get_size();
    std::cout << "position after strtab " << pos << std::endl;

    // After updating the ELF header, write the byte stream to the file
    std::ofstream elf_file(filename, std::ios::out | std::ios::binary);
    if (!elf_file) {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    // Write the ELF header again at the beginning of the file
    elf_file.write(reinterpret_cast<const char*>(&(elf.elf_header)), sizeof(Elf32_Ehdr));

    // Write the byte stream to the file
    elf_file.write(elf_stream.str().data(), elf_stream.str().size());

    elf_file.close();

    if (!elf_file.good()) {
        std::cerr << "Error occurred during writing!\n";
    } else {
        std::cout << "ELF file written successfully.\n";
    }
}
