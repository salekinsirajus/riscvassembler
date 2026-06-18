#ifndef ELF_H
#define ELF_H

#include <cstdint>
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
#include <cctype>

#include "defs.h"
#include "encoding.h"
#include "utils.h"
#include "section.h"
#include "symtab.h"
#include "strtab.h"
#include "text.h"
#include "data.h"

// what we need for the assembler and what we dont need
// Relocatable object files do not need a program header table. (solaris)
// A relocateble object must have a section header table

inline uint32_t hasher(std::string& input) {
    uint32_t FNV_prime = 16777619u;
    uint32_t offset_basis = 2166136261u;
    uint32_t hash = offset_basis;

    for (char c : input) {
        hash ^= static_cast<uint8_t>(c);
        hash *= FNV_prime;
    }

    return hash;
}


typedef struct UnresolvedInst32
{
    uint32_t          insn_number;  /*FIXME: should this be signed? */
    uint32_t          pc_insn_number;
    RISCV32_INST_TYPE insn_type;
    uint32_t          hash;
} UnresolvedInst32;


class ELF32
{
    public:
        ELF32(void);
        ~ELF32();

        size_t  sections_count() const;
        size_t  section_headers_count() const;
        void    init_label(std::string label, bool is_global, std::string section_name);
        bool    label_exists(std::string label);
        void    update_label_visibility(std::string label, bool is_global);
        size_t  store_regular_string(std::string str);
        size_t  store_section_name(std::string);
        int32_t resolve_label(std::string label, uint32_t& offset);
        bool    resolve_symbol(std::string symbol, uint32_t& offset, size_t& section_idx);
        bool    symbol_exists(std::string symbol);
        bool    symbol_resolved(std::string symbol);
        size_t get_next_insn_number(std::string section);

        void _resolve_unresolved_instructions();
        void add_to_unresolved_insns(
            int32_t insn_number, 
            RISCV32_INST_TYPE insn_type, 
            uint32_t hash, 
            uint32_t pc_insn_number
        );
        void add_to_text(uint32_t);

        void add_variable_to_symtab(
                 std::string name,
                 std::string value,
                 std::string section
             );
        // todo: we should use an enum class to indicate which section
        void add_program_data(std::string name, std::string val, std::string section);
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
        Elf32_Shdr*               header_shstrtab; /* ptr to shstrtab sec hdr*/
        std::vector<SectionBase *>sections;        /* Section ptrs           */

        Data                      *sec_data;
        Data                      *sec_bss;
        Data                      *sec_rodata;
        Text                      *sec_text;

        StringTable               *strtab;         /* string table (regular) */
        StringTable               *shstrtab;       /* section header strtab  */
        Symtab                    *symtab;         /* symbol table           */

        //FIXME: do we need two maps? resolved AND unresolved?
        std::map<std::string, uint32_t> resolved_labels;  /* resolved ones   */
        std::map<std::string, uint32_t> unresolved_labels;/* unresolved ones */
        std::map<std::string, uint32_t> section_to_idx;   /* name to shidx   */ 

        std::map<std::string, std::pair<uint32_t, size_t>> symbols;

        std::map<uint32_t, uint32_t> label_to_addr;    /* hash to address    */

        // <offset in .text, and the label>
        std::vector<std::pair<uint32_t, std::string>> forward_decls;
        std::vector<UnresolvedInst32> unresolved_instructions;

        // constants
        const uint32_t UNRESOLVED_ADDR = 0xDEADBEEF;
        const uint32_t UNRESOLVED_OFF  = 0x0FFFFFFF;
        const size_t   UNRESOLVED_IDX  = 0xFFFF;
};

void write_elf(ELF32& elf, std::string filename);

#endif
