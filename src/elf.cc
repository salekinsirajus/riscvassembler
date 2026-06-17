#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdint>

#include "elf.h"
#include "utils.h"
#include "encoding.h"

void Elf32_Ehdr::serialize(std::ostream &out)
{
    out.write(reinterpret_cast<const char*>(e_ident), EI_NIDENT);

    write<Elf32_Half>(out, e_type, LE);
    write<Elf32_Half>(out, e_machine, LE);
    write<Elf32_Word>(out, e_version, LE);
    write<Elf32_Addr>(out, e_entry, LE);
    write<Elf32_Off>(out, e_phoff, LE);
    write<Elf32_Off>(out, e_shoff, LE);
    write<Elf32_Word>(out, e_flags, LE);
    write<Elf32_Half>(out, e_ehsize, LE);
    write<Elf32_Half>(out, e_phentsize, LE);
    write<Elf32_Half>(out, e_phnum, LE);
    write<Elf32_Half>(out, e_shentsize, LE);
    write<Elf32_Half>(out, e_shnum, LE);
    write<Elf32_Half>(out, e_shstrndx, LE);
}

void Elf32_Shdr::serialize(std::ostream &out, byte_order order)
{
    write<Elf32_Word>(out, sh_name, order);
    write<Elf32_Word>(out, sh_type, order);
    write<Elf32_Word>(out, sh_flags, order);
    write<Elf32_Addr>(out, sh_addr, order);
    write<Elf32_Off>(out, sh_offset, order);
    write<Elf32_Word>(out, sh_size, order);
    write<Elf32_Word>(out, sh_link, order);
    write<Elf32_Word>(out, sh_info, order);
    write<Elf32_Word>(out, sh_addralign, order);
    write<Elf32_Word>(out, sh_entsize, order);
}

void Elf32_Sym::serialize(std::ostream &out, byte_order bo)
{
    write<Elf32_Word>(out, st_name, bo);
    write<Elf32_Addr>(out, st_value, bo);
    write<Elf32_Word>(out, st_size, bo);
    write<uint8_t>(out, st_info, bo);
    write<uint8_t>(out, st_other, bo);
    write<Elf32_Half>(out, st_shndx, bo);
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
    sh->sh_flags = 0;

    /* ref: https://refspecs.linuxfoundation.org/LSB_2.1.0/LSB-Core-generic/LSB-Core-generic/specialsections.html */
    sh->sh_entsize = sizeof(Elf32_Sym);

    sh->sh_name = store_section_name(section_name);
    sh->sh_link = 1; // FIXME: sh idx of whichever string table
    sh->sh_info = 1;  // last STB_LOCAL index in the symtab + 1

    Symtab* s = new Symtab();
    symtab = s;
    symtab->header = sh;

    Elf32_Sym first = {};
    first.st_shndx = SHN_UNDEF;
    symtab->push(first);

    section_to_idx[section_name] = section_headers.size();
    section_headers.push_back(sh);
}

void ELF32::init_text_section(){
    //Section<uint32_t>* text = new Section<uint32_t>();
    Text* text = new Text();
    std::string section_name = ".text";

    Elf32_Shdr* sh = new Elf32_Shdr();
    sh->sh_type = SHT_PROGBITS;
    sh->sh_flags = SHF_ALLOC | SHF_EXECINSTR; // is this correct?

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
    Data* data = new Data();
    std::string section_name = ".data";

    Elf32_Shdr* sh = new Elf32_Shdr();
    sh->sh_type = SHT_PROGBITS;
    sh->sh_flags = SHF_ALLOC | SHF_WRITE;

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
    this->header_shstrtab = sh_shstrtab;  // saving it

    strtab = new StringTable();
    shstrtab = new StringTable(true);

    strtab->header = sh_strtab;
    shstrtab->header = sh_shstrtab;

    std::memset(sh_strtab, 0, sizeof(Elf32_Shdr));
    std::memset(sh_shstrtab, 0, sizeof(Elf32_Shdr));

    sh_strtab->sh_type = SHT_STRTAB;
    sh_shstrtab->sh_type = SHT_STRTAB;

    strtab->header->sh_size = strtab->size_in_bytes();
    shstrtab->header->sh_size = shstrtab->size_in_bytes();

    sh_shstrtab->sh_name = store_section_name(".shstrtab");
    sh_strtab->sh_name = store_section_name(".strtab");

    section_to_idx[".shstrtab"] = section_headers.size();
    section_headers.push_back(sh_strtab);
    section_to_idx[".strtab"] = section_headers.size();
    section_headers.push_back(sh_shstrtab);
}

size_t ELF32::store_section_name(std::string name){
    size_t offset = shstrtab->add_string(name.c_str());

    return offset;
}

size_t ELF32::store_regular_string(std::string str){
    size_t offset = strtab->add_string(str.c_str());
    strtab->header->sh_size = strtab->size_in_bytes();

    return offset; //index at which this string is stored
}

void ELF32::init_label(std::string the_label, bool is_global, std::string section_name){
    //FIXME: review this later on, if the label already exists because of a directive, don't
    //FIXME: no need to create it. check if this still holds true when things get complicated
    if (label_exists(the_label)) { return; }

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
    symtab->push(sym);
    //return idx_strtab;
}

void ELF32::update_label_visibility(std::string label, bool is_global)
{
    if (label_exists(label))
    {
        std::cout << "(NYI) go find the label in symtab and update it's visibility" << std::endl;
    } else
    {
        init_label(label, is_global, ".text"); //FIXME: remove hardcoded section_name  
    }
}

bool ELF32::label_exists(std::string label){
    return (resolved_labels.count(label) > 0);
}

bool ELF32::symbol_exists(std::string symbol)
{
    std::map<std::string, std::pair<uint32_t, size_t>>::iterator it = symbols.find(symbol);
    if (it == symbols.end()) return false;

    return true;
}

bool ELF32::symbol_resolved(std::string symbol)
{
    if (!symbol_exists(symbol)) return false;

    if (symbols[symbol].first == UNRESOLVED_OFF)
    {
        return false;
    }

    return true;
}

bool ELF32::resolve_symbol(std::string symbol, uint32_t& offset, size_t& section_idx)
{
    bool exists = symbol_exists(symbol);

    if (!exists)
    {
        offset = UNRESOLVED_OFF;
        section_idx = UNRESOLVED_IDX;
        symbols[symbol] = std::make_pair(offset, section_idx);

        return false;
    }

    offset = symbols[symbol].first;
    section_idx = symbols[symbol].second;

    return true;
}

int32_t ELF32::resolve_label(std::string label, uint32_t &offset){
    //TODO: figure out how to differentiate between the data (string) and code labels
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

void ELF32::add_to_unresolved_insns(
    int32_t insn_number, RISCV32_INST_TYPE insn_type, 
    uint32_t hash, uint32_t pc_insn_number
){
    UnresolvedInst32 i;
    i.insn_number = insn_number; // what is the use of this?
    i.pc_insn_number = pc_insn_number;
    i.insn_type = insn_type;
    i.hash = hash;

    unresolved_instructions.push_back(i);
}

void ELF32::_resolve_unresolved_instructions()
{
    for (auto &pair : resolved_labels){
        std::cout << pair.first << ": " << std::hex << pair.second << std::endl;
    }
 
    uint32_t offset = 0;
    btype32_t b; 
    utype32_t u; 

   //TODO: if it's still not resolved, it's would be resolved by the linker
   //TODO: add a rela.text section
    for (auto &entry : unresolved_instructions){
        /*
        std::cout << "Type: "     << entry.insn_type 
                  << ", Number: " << entry.insn_number 
                  << ", PC: "     << entry.pc_insn_number 
                  << ", hash: "   << entry.hash << std::endl;
        */
        uint32_t insn;
        sec_text->get_entry(entry.insn_number, insn);
        uint32_t resolved_insn_number;
        int32_t  resolved_effective_offset;
        //std::cout << "instruction before: " << std::hex << insn << std::endl;
        switch (entry.insn_type){
            case B_TYPE:
               b = btype32_t::deserialize(insn);
               resolved_insn_number = label_to_addr[entry.hash];
               //std::cout << "resolved label to: " << resolved_insn_number << std::endl;
               // TODO: figure out how this type of instruction offsets are encoded
               resolved_effective_offset = ((resolved_insn_number - entry.pc_insn_number)) * INSTRUCTION_WIDTH / 2;
               //std::cout << "resolved_effective_offset: " << resolved_effective_offset << std::endl;
               // FIXME: check the emit_b.. function to see how the imm is being
               // FIXME: encoded. We had issues with U_TYPE. Check if it's correct 
               // FIXME: for other types.
               insn = emit_b_type_instruction(
                   resolved_effective_offset, b.rs1, b.rs2, b.funct3, b.opcode
               );
               //std::cout << "instruction after: " << std::hex << insn << std::endl;
               sec_text->update_entry(entry.insn_number, insn);
               //std::cout << "done" << std::endl;
               break;
            case U_TYPE:
               u = utype32_t::deserialize(insn);
               resolved_insn_number = label_to_addr[entry.hash];
               //std::cout << "resolved label to: " << resolved_insn_number << std::endl;
               resolved_effective_offset = ((resolved_insn_number - entry.pc_insn_number)) * INSTRUCTION_WIDTH / 2;
               //std::cout << "resolved_effective_offset: " << resolved_effective_offset << std::endl;
               insn = emit_u_type_instruction(resolved_effective_offset, u.rd, u.opcode);
               //std::cout << "instruction after: " << std::hex << insn << std::endl;
               sec_text->update_entry(entry.insn_number, insn);
               //std::cout << "done" << std::endl;
               break;
            case J_TYPE:
               jtype32_t unresolved_inst = jtype32_t::deserialize(insn);
               resolved_insn_number = label_to_addr[entry.hash];
               resolved_effective_offset = ((resolved_insn_number - entry.pc_insn_number)) * INSTRUCTION_WIDTH / 2;
               insn = emit_j_type_instruction(resolved_effective_offset, unresolved_inst.rd, unresolved_inst.opcode);
               sec_text->update_entry(entry.insn_number, insn);
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

// Eventually phase this out as well
void ELF32::add_to_text(uint32_t instr){
    sec_text->push(instr);
}

void ELF32::add_to_symtab(Elf32_Sym& entry){
    symtab->push(entry);
}

//TODO: does the entry mean a label or a string?
//TODO: or a variable?
//TODO: delete this version
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

    symtab->push(sym);
    symtab->header->sh_size += sizeof(Elf32_Sym);
}

void ELF32::add_program_data(std::string var_name, std::string value, std::string section)
{
    Elf32_Sym sym = {};
    sym.st_name = store_regular_string(var_name); // strtab idx
    sym.st_info = ELF32_ST_BIND(STB_LOCAL);

    sym.st_shndx = section_to_idx[section];
    sym.st_value = sec_data->push(value);   // offset in relation to the section identified in st_shndx

    symtab->push(sym);
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

    elf_header.e_phnum = 0;          // Number of program headers (should be always zero)
    elf_header.e_shnum = 0;          // number of entries in the section header table
    elf_header.e_shstrndx = 0;       // index for string table that contains section names
    //section header table index of the entry associated with the section name string table.
}

void ELF32::serialize(std::ostream& os){
    byte_order bo = LE;
    elf_header.serialize(os);
    std::streampos pos = os.tellp();

    // Add sections
    std::cout << "# of sections: " << sections.size() << std::endl;
    for (std::vector<SectionBase *>::iterator it = sections.begin(); it != sections.end(); ++it) {
        pos = os.tellp();
        (*it)->serialize(os, LE); 
        // .text in RISC-V needs to be LE, data could be any. For now, we are doing all LE
        (*it)->header->sh_offset = pos;
        std::cout << "position after section: " << pos << std::endl;
    }

    // Add string tables
    //strtab->print_content();
    strtab->serialize(os, bo);

    //shstrtab->print_content();
    shstrtab->serialize(os, bo);

    symtab->serialize(os, bo);

    std::cout << "beginning of the section header here: " << os.tellp() << std::endl;
    elf_header.e_shoff = static_cast<uint32_t>(os.tellp());
    elf_header.e_shnum = section_headers.size();
    //TODO: write properly
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
        if ((*it)->sh_type == SHT_STRTAB && *it == this->header_shstrtab){
            elf_header.e_shstrndx = std::distance(section_headers.begin(), it);
            std::cout << "e_shstrndx: " << elf_header.e_shstrndx << std::endl;
        }

        (*it)->serialize(os, bo);
        pos += sizeof(Elf32_Shdr);
    }

    //rewrite the ELF header with updated info
    os.seekp(0, std::ios_base::beg);
    elf_header.serialize(os);
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
