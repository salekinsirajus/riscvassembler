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

const size_t UNRESOLVED_IDX = 0xFAF0;

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

typedef struct Sym
{
    size_t   section_idx;   /* which section is this related to */
    uint32_t offset_idx;    /* offset from section beginning: NOT SIZE but count */
    size_t   symtab_idx;    /* index in the ELF symbol table section (TODO: is it needed?) */
    bool     resolved;      /* is the symbol resolved */
    bool     label;         /* is it a code label */
} Sym;

class ELF32
{
    public:
        ELF32(void);
        ~ELF32();

        /* How many valid sections do we have */
        size_t  sections_count() const;

        /* How many section headers do we have */
        size_t  section_headers_count() const;

        /* Initialize a label
         * @param label std::string name of the label
         * @param is_gloabl bool visibility (local for only this file, global for linkers)
         * @param section_name std::string which section is this related to (TODO: get index/ptr instead)
         */
        void    init_label(std::string label, bool is_global, std::string section_name);

        /* Check if a label exists
         * @param label std::string name of the label
         * @returns true if it exists, false otherwise
         */
        bool    label_exists(std::string label);

        /* Change the visibility level of a label
         * @param label std::string name of the label
         * @param is_global bool set it to global if it's true, set to local otherwise
         */
        void    update_label_visibility(std::string label, bool is_global);

        /* Stores a string into the generic string table
         * @param str std::string the string to store
         * @returns the index of the string table it's stored at.
         */
        size_t  store_regular_string(std::string str);

        /* Stores a string into a special string table where only the section names are stored
         * @param str std::string the string to store
         * @returns the index of the string table it's stored at.
         */
        size_t  store_section_name(std::string str);

        /* Given a section name, get its index. Returns a sentinel when not found
         * @param section_name std::string name of the section
         * @returns index into vector that contains the sections
         */
        size_t  get_section_idx(std::string section_name);

        /* TODO
         */
        int32_t resolve_label(std::string label, uint32_t& offset);

        /* Add a new symbol to the program
         * @param
         */
        void    add_symbol(
                    std::string sym_text, size_t section_idx, uint32_t offset_idx, 
                    size_t symtab_idx,  bool is_resolved, bool is_label
                );
        bool    resolve_symbol(std::string symbol, Sym& _sym);
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

        std::map<std::string, Sym> symbols;

        std::map<uint32_t, uint32_t> label_to_addr;    /* hash to address    */

        std::vector<UnresolvedInst32> unresolved_instructions;

        // constants
        const uint32_t UNRESOLVED_ADDR = 0xDEADBEEF;
        const uint32_t UNRESOLVED_OFF  = 0x0FFFFFFF;
        const size_t   UNRESOLVED_IDX  = 0xFFFF;
};

void write_elf(ELF32& elf, std::string filename);

#endif
