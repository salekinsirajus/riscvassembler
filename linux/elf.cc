#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdint>
#include "elf.h"
#include "../encoding.h"

// Make sure all the stuff is in place
// TODO Entry point - does an assembler make the decision
// to set the entry point, such as _main? Generally, it
// is the beginning of the text section

void write_elf(ELF32& elf, std::string filename) {
    std::ostringstream elf_stream(std::ios::binary);
    elf.serialize(elf_stream);

    // After updating the ELF header, write the byte stream to the file
    std::ofstream elf_file(filename, std::ios::out | std::ios::binary);
    if (!elf_file) {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    // Write the byte stream to the file
    elf_file.write(elf_stream.str().data(), elf_stream.str().size());

    elf_file.close();

    if (!elf_file.good()) {
        std::cerr << "Error occurred during writing!\n";
    } else {
        std::cout << "ELF file written successfully.\n";
    }
}

ELF32::ELF32(void){
    init_elf_header();
    init_section_headers();
    init_strtables();
    init_symtab();
    init_text_section();
    init_data_section();
}

ELF32::~ELF32(){
    //TODO: cleanup
}

void ELF32::init_symtab(){
    Elf32_Shdr* sh = new Elf32_Shdr();
    std::memset(sh, 0, sizeof(Elf32_Shdr));

    sh->sh_size = 0;
    sh->sh_type = SHT_SYMTAB;
    sh->sh_entsize = sizeof(Elf32_Sym);
    //sh->sh_flag = SHF_ALLOC; // TODO: confirm this

    sh->sh_name = store_section_name(".symtab");
    sh->sh_link = 1; // FIXME: sh idx of whichever string table
    sh->sh_info = 1;  // last STB_LOCAL index in the symtab + 1
    std::cout << "Symtab sh_name: " << sh->sh_name << std::endl;

    Symtab* s = new Symtab();
    symtab = s;
    symtab->header = sh;

    Elf32_Sym first = {};
    first.st_shndx = SHN_UNDEF;
    symtab->push_back(first);

    std::cout << "SYMTAB: Section Header Idx: " << section_headers.size() << std::endl;
    section_headers.push_back(sh);
}

void ELF32::init_text_section(){
    Section* text = new Section();

    Elf32_Shdr* sh = new Elf32_Shdr();
    sh->sh_type = SHT_PROGBITS;
    sec_text = text;
    sec_text->header = sh;

    sections.push_back(text);
    sh->sh_name = store_section_name(".text");    
    std::cout << "TEXT: Section Header Idx: " << section_headers.size() << std::endl;
    section_headers.push_back(sh);
}

void ELF32::init_section_headers(){
    Elf32_Shdr *first_entry = new Elf32_Shdr;
    std::memset(first_entry, 0, sizeof(Elf32_Shdr));

    std::cout << "FIRST ENTRY: Section Header Idx: " << section_headers.size() << std::endl;
    section_headers.push_back(first_entry);
}

void ELF32::init_data_section(){
    Section* data = new Section();

    Elf32_Shdr* sh = new Elf32_Shdr();
    sh->sh_type = SHT_PROGBITS;
    sec_data = data;
    sec_data->header = sh;

    sections.push_back(data);      //is this working?

    sh->sh_name = store_section_name(".data");    
    std::cout << "data_sh " << sh->sh_name << std::endl;
    std::cout << "DATA: Section Header Idx: " << section_headers.size() << std::endl;
    section_headers.push_back(sh);
}

void ELF32::init_strtables(){
    Elf32_Shdr* sh_strtab = new Elf32_Shdr();
    Elf32_Shdr* sh_shstrtab = new Elf32_Shdr();
    strtab = new StringTable();
    shstrtab = new StringTable();    

    strtab->header = sh_strtab;
    shstrtab->header = sh_shstrtab;

    std::memset(sh_strtab, 0, sizeof(Elf32_Shdr));
    std::memset(sh_shstrtab, 0, sizeof(Elf32_Shdr));

    sh_strtab->sh_type = SHT_STRTAB;
    sh_shstrtab->sh_type = SHT_STRTAB;

    strtab->header->sh_size = strtab->get_size();
    shstrtab->header->sh_size = shstrtab->get_size();

    sh_shstrtab->sh_name = store_section_name(".shstrtab");
    sh_strtab->sh_name = store_section_name(".strtab");

    std::cout << "STRTAB: Section Header Idx: " << section_headers.size() << std::endl;
    section_headers.push_back(sh_strtab);
    std::cout << "SHSTRTAB: Section Header Idx: " << section_headers.size() << std::endl;
    section_headers.push_back(sh_shstrtab);
}

size_t ELF32::store_section_name(std::string name){
    size_t offset = shstrtab->add_string(name.c_str());
    shstrtab->header->sh_size = shstrtab->get_size();

    return offset;
}

size_t ELF32::store_regular_string(std::string str){
    size_t offset = strtab->add_string(str.c_str());
    strtab->header->sh_size = strtab->get_size();

    return offset; //index at which this string is stored
}

size_t ELF32::store_label(std::string the_label, bool is_global){
    size_t offset = store_regular_string(the_label); //FIXME: is it a regular string?

    Elf32_Sym sym = {};
    sym.st_name = offset;        // index into the string table
    sym.st_info = ELF32_ST_BIND(is_global ? STB_GLOBAL : STB_LOCAL);
    sym.st_shndx = 4;  //FIXME: for now, it's in relation to .text

    symtab->header->sh_size += sizeof(Elf32_Sym); //update size
    symtab->push_back(sym);
    return offset;
}

void ELF32::add_to_text(rtype32_t instr){
    sec_text->data.push_back(instr);
}

void ELF32::add_to_text(itype32_t instr){
    sec_text->data.push_back(instr);
}

void ELF32::add_to_data(){

}

void ELF32::add_to_symtab(Elf32_Sym& entry){
    symtab->push_back(entry);
}

//TODO: does the entry mean a label or a string?
//TODO: or a variable?
void ELF32::add_to_symtab(std::string entry){
    Elf32_Sym* sym = new Elf32_Sym();
}

void ELF32::update_elf_header(){
    // thigns to update:
    // - section header table offset
    // - e_phnum number of program headers
    // - e_shnum number pf section headers
    // - s_shstrndx section header string index
    elf_header.e_shoff = 1000; // FIXME section header table's file offset in bytes
    elf_header.e_shnum = 2;    // update this every time a new section is added
}

void ELF32::init_elf_header(){
    memset(&elf_header, 0, sizeof(elf_header));

    elf_header.e_ident[0] =   0x7f;
    elf_header.e_ident[1] =    'E';
    elf_header.e_ident[2] =    'L';
    elf_header.e_ident[3] =    'F';
    elf_header.e_ident[4] =      1;  // 32-bit
    elf_header.e_ident[5] =      1;  // Little-endian
    elf_header.e_ident[6] =      1;  // ELF version
    elf_header.e_type     = ET_REL;  // Object file
    elf_header.e_machine  =   0xF3;  // RISC-V (0xF3)
    elf_header.e_version  =      1;  // ELF version

    elf_header.e_entry =         0;  // relocatable fie, no entry point
    elf_header.e_phoff =         0;  // relocatable file, no prog header table
    elf_header.e_shoff =         0;  // TODO Section header table offset
    elf_header.e_flags =         0;

    elf_header.e_ehsize    = sizeof(Elf32_Ehdr); //52 (64 bytes in 64-bit ELF and 52 bytes for 32 bits)
    elf_header.e_phentsize =                  0; // size of 1 entry in prog header table.
    elf_header.e_shentsize = sizeof(Elf32_Shdr); // Section header table entry size in bytes.

    elf_header.e_phnum = 0;          // TODO: Number of program headers
    elf_header.e_shnum = 0;          // TODO: number of entries in the section header table
    elf_header.e_shstrndx = 0;       // TODO: Section header string index??
    //section header table index of the entry associated with the section name string table.
}

void ELF32::serialize(std::ostream& os){
    os.write(reinterpret_cast<const char*>(&elf_header), sizeof(Elf32_Ehdr));
    std::streampos pos = os.tellp();

    // Add sections
    std::cout << "# of sections: " << sections.size() << std::endl;
    for (std::vector<Section *>::iterator it = sections.begin(); it != sections.end(); ++it) {
        pos = os.tellp();
        size_t data_size = ((*it)->data.size() * sizeof(uint32_t));
        os.write(reinterpret_cast<const char*>((*it)->data.data()), data_size);
        (*it)->header->sh_offset = pos;
        std::cout << "position after section: " << pos << std::endl;
    }

    // Add string tables
    strtab->print_content();
    strtab->serialize(os);

    shstrtab->print_content();
    shstrtab->serialize(os);

    symtab->serialize(os);

    std::cout << "beginning of the section header here: " << os.tellp() << std::endl;
    elf_header.e_shoff = static_cast<uint32_t>(os.tellp());
    elf_header.e_shnum = section_headers.size();
    std::cout << "# of section headers: " << section_headers.size() << std::endl;

    // Add an empty header first?
    // TODO: update the ELF header's e_shoff field
    std::cout << "serializing the headers" << std::endl;
    for (std::vector<Elf32_Shdr *>::iterator it = section_headers.begin();
         it != section_headers.end(); ++it
        ){
        std::cout << "sh_name: " << (*it)->sh_name 
                  << ", sh_size: " << (*it)->sh_size 
                  << ", sh_offset: " << std::hex << (*it)->sh_offset << std::endl;
        if ((*it)->sh_type == SHT_STRTAB){
            elf_header.e_shstrndx = std::distance(section_headers.begin(), it);
        }
        os.write(reinterpret_cast<const char*>((*it)), sizeof(Elf32_Shdr));
        pos += sizeof(Elf32_Shdr);
    }
    
    os.seekp(0, std::ios_base::beg);
    os.write(reinterpret_cast<const char*>(&elf_header), sizeof(Elf32_Ehdr));
}
