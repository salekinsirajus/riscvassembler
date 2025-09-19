#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdint>
#include "elf.h"
#include "../encoding.h"

StringTable::StringTable(void) {
    // Start with a null terminator
    content.push_back('\0');
}

size_t StringTable::add_string(const char* str) {
    if (!str) throw std::invalid_argument("Null string passed");

    // Insert before the final null terminator
    size_t offset = content.size() - 1;
    size_t len = std::strlen(str);

    content.insert(content.end() - 1, str, str + len);
    content.insert(content.end() - 1, '\0');

    return offset;
}
const char* StringTable::get_string(size_t offset) const {
    if (offset >= content.size()) throw std::out_of_range("Offset out of bounds");
    return &content[offset];
}

const char* StringTable::get_all() const {
    return content.data();
}

size_t StringTable::get_size() const {
    return content.size();
}

void StringTable::serialize(std::ostream &os) const {
    if (!header) throw std::runtime_error("Header not set");
    os.seekp(0, std::ios_base::end); //get to the end

    header->sh_offset = static_cast<uint32_t>(os.tellp());
    header->sh_size = get_size();

    os.write(reinterpret_cast<const char*>(content.data()), content.size());
}

void StringTable::print_content() const {
    const size_t columns = 10;
    size_t size = content.size();

    // Print column headers
    printf("\n========= strtab (size: %zu) =========\n", size);
    printf("Offset |");
    for (size_t col = 0; col < columns; ++col) {
        printf(" %zu ", col);
    }
    printf("\n-------+------------------------------\n");

    // Print content in rows of 10
    for (size_t i = 0; i < size; i += columns) {
        printf("%06zu |", i);  // Offset

        for (size_t j = 0; j < columns; ++j) {
            if (i + j < size) {
                char c = content[i + j];
                if (std::isprint(static_cast<unsigned char>(c))) {
                    printf(" %c ", c);
                } else {
                    printf(" . ");
                }
            } else {
                printf("   "); // Blank for missing cells
            }
        }
        printf("\n");
    }

    printf("=======================================\n");
}


Symtab::Symtab(){
}

void Symtab::push_back(Elf32_Sym& sym){
    data.push_back(sym);
}

size_t Symtab::get_size() const {
    return (data.size() * sizeof(Elf32_Sym));
}

void Symtab::serialize(std::ostream& os){
    if (data.size() > 0){
        printf("size of symtab: %lu\n", data.size());
        header->sh_offset = static_cast<uint32_t>(os.tellp());
        header->sh_size = get_size();
        os.write(reinterpret_cast<const char*>(data.data()), get_size());
    }
}

ELF32::ELF32(void){
    init_elf_header();
    init_section_headers();
    init_strtables();
    init_symtab();
    init_text_section();
    init_data_section();
    printf("\n\n");
}

ELF32::~ELF32(){
    //TODO: cleanup
}

void ELF32::init_symtab(){
    Elf32_Shdr* sh = new Elf32_Shdr();
    std::memset(sh, 0, sizeof(Elf32_Shdr));

    std::string section_name = ".symtab";

    sh->sh_size = 0;
    sh->sh_type = SHT_SYMTAB;
    sh->sh_entsize = sizeof(Elf32_Sym);
    //sh->sh_flag = SHF_ALLOC; // TODO: confirm this

    sh->sh_name = store_section_name(section_name);
    sh->sh_link = 1; // FIXME: sh idx of whichever string table
    sh->sh_info = 1;  // last STB_LOCAL index in the symtab + 1
    std::cout << "Symtab sh_name: " << sh->sh_name << std::endl;

    Symtab* s = new Symtab();
    symtab = s;
    symtab->header = sh;

    Elf32_Sym first = {};
    first.st_shndx = SHN_UNDEF;
    symtab->push_back(first);

    section_to_idx[section_name] = section_headers.size();
    section_headers.push_back(sh);
}

void ELF32::init_text_section(){
    Section* text = new Section();
    std::string section_name = ".text";

    Elf32_Shdr* sh = new Elf32_Shdr();
    sh->sh_type = SHT_PROGBITS;
    sec_text = text;
    sec_text->header = sh;

    sections.push_back(text);
    sh->sh_name = store_section_name(section_name);
    section_to_idx[section_name] = section_headers.size();
    section_headers.push_back(sh);
}

void ELF32::init_section_headers(){
    Elf32_Shdr *first_entry = new Elf32_Shdr;
    std::memset(first_entry, 0, sizeof(Elf32_Shdr));

    section_headers.push_back(first_entry);
}

void ELF32::init_data_section(){
    Section* data = new Section();
    std::string section_name = ".data";

    Elf32_Shdr* sh = new Elf32_Shdr();
    sh->sh_type = SHT_PROGBITS;
    sec_data = data;
    sec_data->header = sh;

    sections.push_back(data);      //is this working?

    sh->sh_name = store_section_name(section_name);
    section_to_idx[section_name] = section_headers.size();
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

    section_to_idx[".shstrtab"] = section_headers.size();
    section_headers.push_back(sh_strtab);
    section_to_idx[".strtab"] = section_headers.size();
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

size_t ELF32::init_label(std::string the_label, bool is_global, std::string section_name){
    std::cout << "init_label: " << the_label << ", section: " << section_name << std::endl;
    size_t idx_strtab = store_regular_string(the_label); //FIXME: is it a regular string?

    Elf32_Sym sym = {};
    sym.st_name = idx_strtab;
    sym.st_info = ELF32_ST_BIND(is_global ? STB_GLOBAL : STB_LOCAL);
    // find what st_shndx this section_name string belongs to?
    if (section_to_idx.count(section_name) > 0){
       sym.st_shndx = section_to_idx[section_name];
    } // TODO: should we do an else in case it fails?

    // check if there is any forward references to this label 
    if (unresolved_labels.count(the_label) > 0){
       unresolved_labels.erase(the_label); // remove it
    }

    resolved_labels[the_label] = sec_text->last_index(); //FIXME: remove section hardcoding
    label_to_addr[hasher(the_label)] = sec_text->last_index();

    symtab->header->sh_size += sizeof(Elf32_Sym); //update size
    symtab->push_back(sym);
    return idx_strtab;
}

int32_t ELF32::resolve_label(std::string label, uint32_t &offset){
    //TODO: figure out how to differentiate between the data (string) and code labels
    std::cout << "====Existing Labels=====" << std::endl;
    for (auto &pair : resolved_labels){
        std::cout << pair.first << ": " << std::hex << pair.second << std::endl;
    }
    std::cout << "========================" << std::endl;
    //if the label already exists, return the offset
    std::cout << "resolve_label(" << label << ")" << std::endl;
    if (resolved_labels.count(label) > 0){
        std::cout << "found a resolved label: " << resolved_labels[label] << std::endl;
        offset = resolved_labels[label];
        return 0;
    }

    unresolved_labels[label] = hasher(label);
    offset = unresolved_labels[label];

    return -1;
}

 void ELF32::add_to_unresolved_insns(uint32_t insn_number, RISCV32_INST_TYPE insn_type, uint32_t hash){
    UnresolvedInst32 i;
    i.insn_number = insn_number;
    i.insn_type = insn_type;
    i.hash = hash;

	unresolved_instructions.push_back(i);
}

void ELF32::_resolve_unresolved_instructions()
{
    std::cout << "_resolve_unresolved_instr()" << std::endl;
	std::cout << "resolved_labels: " << std::endl;
    for (auto &pair : resolved_labels){
        std::cout << pair.first << ": " << std::hex << pair.second << std::endl;
    }
 
    uint32_t offset = 0;
    btype32_t b; 
    utype32_t u; 

   //TODO: if it's still not resolved, it's most likely would be resolved by the linker
   //TODO: make appropriate structure for the object file
    for (auto &entry : unresolved_instructions){
        std::cout << "Type: " << entry.insn_type 
                  << ", Number: " << entry.insn_number 
                  << ", hash: " << entry.hash << std::endl;
        uint32_t insn = sec_text->get_entry(entry.insn_number);
        uint32_t resolved_addr;
        std::cout << "instruction before: " << std::hex << insn << std::endl;
        switch (entry.insn_type){
            case B_TYPE:
               b = btype32_t::deserialize(insn);
               resolved_addr = label_to_addr[entry.hash];
               std::cout << "resolved addr: " << std::hex << resolved_addr << std::endl;
               insn = emit_b_type_instruction(
                   resolved_addr, b.rs1, b.rs2, b.funct3, b.opcode
               );
               std::cout << "instruction after: " << std::hex << insn << std::endl;
               sec_text->update_entry(entry.insn_number, insn);
               std::cout << "done" << std::endl;
               break;
            case U_TYPE:
               u = utype32_t::deserialize(insn);
               resolved_addr = label_to_addr[entry.hash];
               std::cout << "resolved addr: " << std::hex << resolved_addr << std::endl;
               insn = emit_u_type_instruction(resolved_addr, u.rd, u.opcode);
               std::cout << "instruction after: " << std::hex << insn << std::endl;
               sec_text->update_entry(entry.insn_number, insn);
               std::cout << "done" << std::endl;
               break;
        }
    }
}

size_t ELF32::get_next_insn_number(std::string section){
    // TODO: reduce hardcoding
    if (section == ".text"){
        return sec_text->next_index();
    }

    return 0;
}

void ELF32::add_to_text(uint32_t instr){
    sec_text->data.push_back(instr);
    sec_text->header->sh_size += sizeof(instr);
}

void ELF32::add_to_data(){

}

void ELF32::add_to_symtab(Elf32_Sym& entry){
    symtab->push_back(entry);
}

//TODO: does the entry mean a label or a string?
//TODO: or a variable?
void ELF32::add_variable_to_symtab(
    std::string name,
    std::string value,
    std::string section){

    //We'll start with hardcoding to .data section
    Elf32_Sym sym = {};
    sym.st_name = store_regular_string(name); // strtab idx
    sym.st_info = ELF32_ST_BIND(STB_LOCAL);   // TODO: accommodate global later)
    sym.st_shndx = 1; // .data FIXME: find section index from the section value
    sym.st_value = store_regular_string(value);   // offset in relation to the section identified in st_shndx

    symtab->push_back(sym);
    symtab->header->sh_size += sizeof(Elf32_Sym);
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
