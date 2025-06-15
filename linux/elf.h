#ifndef ELF_H
#define ELF_H

#include "defs.h"

#include <stdint.h>
#include <string>
#include <cstring>
#include <vector>


class StringTable {
public:
    StringTable() {
        // Reserve space for initial null byte
        content.push_back('\0');
    }

    size_t add_string(const char* str) {
        size_t offset = content.size();
        size_t len = std::strlen(str);
        // +1 for null terminator
        content.insert(content.end(), str, str + len + 1);
        return offset;
    }

    const char* get_string(size_t offset) const {
        return &content[offset];
    }

    const char* get_all() const {
        return content.data();
    }

    size_t get_size() const {
        return content.size();
    }

    Elf32_Shdr* header;

private:
    std::vector<char> content;
};


typedef struct Section {
    std::vector<uint32_t> data;
    uint32_t            offset;
    Elf32_Shdr         *header;
} Section;

// what we need for the assembler and what we dont need
// Relocatable object files do not need a program header table. (solaris)
// A relocateble object must have a section header table
typedef struct ELF32 {
    Elf32_Ehdr elf_header;                     /* ELF File Header        */
    std::vector<Elf32_Phdr>   program_headers; /* Program Headers (opt)  */
    std::vector<Elf32_Shdr *> section_headers; /* Section Headers (req)  */
    std::vector<Section *>    sections;        /* Section ptrs           */
    std::vector<Elf32_Sym>    symtab;          /* symbol table           */
    StringTable               strtab;          /* string table           */

    Section*                  data;            /* ptr to data            */
    Section*                  bss;             /* ptr to bss             */
    Section*                  rodata;          /* ptr to rodata          */
    Section*                  text;            /* ptr to code            */
} ELF32;

void write_elf(ELF32& elf, std::string filename);
void initialize_elf(ELF32& elf);
void initialize_symbol_table(ELF32& elf);
void initialize_string_table(ELF32& elf);
void initialize_text_section(ELF32& elf);
void initialize_data_section(ELF32& elf);
size_t store_string(ELF32& elf, std::string the_string);
size_t store_label(ELF32& elf, std::string the_label, bool is_global);

#endif
