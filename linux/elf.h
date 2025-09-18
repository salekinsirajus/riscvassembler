#ifndef ELF_H
#define ELF_H

#include "defs.h"
#include "../encoding.h"

#include <stdint.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
#include <string>
#include <cstring>
#include <map>

#include <cctype> // for std::isprint

//TODO: move the implementation to a separate file
class StringTable
{
public:
    StringTable();

    // Adds a null-terminated string and returns its offset
    size_t add_string(const char* str);

    // Access string at offset
    const char* get_string(size_t offset) const;

    // Returns entire string table buffer
    const char* get_all() const;

    // Returns total size including both nulls
    size_t get_size() const;

    // Write string table to output stream and update header
    void serialize(std::ostream &os) const;

    // Debugging aid
    void print_content() const;

    Elf32_Shdr* header;

private:
    std::vector<char> content;
};

class Symtab
{
public:
    Symtab();

    // TODO: consider if the name is okay since it's overload of a term
    void push_back(Elf32_Sym& sym);
    void serialize(std::ostream& os);
    size_t get_size() const;

    Elf32_Shdr* header;

private:
     std::vector<Elf32_Sym> data;
};

typedef struct Section {
    std::vector<uint32_t> data;
    uint32_t            offset;
    Elf32_Shdr         *header;

    // returns the size of the section in bytes
    size_t size() const {
        return data.size() * sizeof(uint32_t);
    }

    int last_index() const {
        return data.size() - 1;
    }

} Section;

// what we need for the assembler and what we dont need
// Relocatable object files do not need a program header table. (solaris)
// A relocateble object must have a section header table

typedef struct UnresolvedInst32
{
    uint32_t          insn_number;
    RISCV32_INST_TYPE insn_type;
} UnresolvedInst32;

class ELF32
{
    public:
        ELF32(void);
        ~ELF32();

        size_t sections_count() const;
        size_t section_headers_count() const;
        size_t section_content_size(Section& s) const;
        size_t init_label(std::string label, bool is_global, std::string sectio_name);
        size_t store_regular_string(std::string str);
        size_t store_section_name(std::string);
        int32_t resolve_label(std::string label, uint32_t &offset);
        size_t get_next_insn_number(std::string section);

        void _resolve_unresolved_instructions();
        void add_to_unresolved_insns(uint32_t insn_number, RISCV32_INST_TYPE insn_type);
        void add_to_text(uint32_t);
        void add_to_data();
        void add_to_symtab(Elf32_Sym& symbol);
        void add_variable_to_symtab(
                 std::string name,
                 std::string value,
                 std::string section
             );
        void serialize(std::ostream& os);
        void deserialize();

        Elf32_Ehdr& get_elf_header(){ return elf_header; }

    private:
        void init_elf_header();
        void init_elf();

        void init_symtab();
        void init_strtables();
        void init_text_section();
        void init_data_section();

        void update_elf_header();

        void init_section_headers();
        size_t serialize_section_headers(std::ostream os);

    protected:
        Elf32_Ehdr elf_header;                     /* ELF File Header        */

        std::vector<Elf32_Shdr *> section_headers; /* Section Headers (req)  */
        std::vector<Section *>    sections;        /* Section ptrs           */

        Section                   *sec_data;
        Section                   *sec_bss;
        Section                   *sec_rodata;
        Section                   *sec_text;

        StringTable               *strtab;         /* string table (regular) */
        StringTable               *shstrtab;       /* section header strtab  */
        Symtab                    *symtab;         /* symbol table           */

        std::map<std::string, uint32_t> resolved_labels;  /* resolved ones   */
        std::map<std::string, uint32_t> unresolved_labels;/* unresolved ones */
        std::map<std::string, uint32_t> section_to_idx;   /* name to shidx   */ 

        // <offset in .text, and the label>
        std::vector<std::pair<uint32_t, std::string>> forward_decls;
        std::vector<UnresolvedInst32> unresolved_instructions;

        // constants
        const uint32_t UNRESOLVED_ADDR = 0xDEADBEEF;
};

void write_elf(ELF32& elf, std::string filename);

#endif
