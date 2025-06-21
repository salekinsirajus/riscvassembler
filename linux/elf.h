#ifndef ELF_H
#define ELF_H

#include "defs.h"
#include "../encoding.h"

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <string>
#include <cstring>

class StringTable {
public:
    StringTable() {
        // Reserve space for initial null byte
        content.push_back('\0');
    }

    size_t add_string(const char* str) {
        //TODO: always add a null-terminator so you can serialize anytime
        size_t offset = content.size();
        size_t len = std::strlen(str); //TODO: double check if this is correct

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
        return content.size() + 1; //the final null-terminator
    }

    void print_content() const {
        printf("\n=========strtab============\n");
        for (int i=0; i < content.size(); i++){
            printf("|(%x %c %d)", content[i], content[i], i);
        }
        printf("\n===========================\n");
    }

    void serialize(std::ostream &os){
        //An additional null terminator is needed at the end
        content.push_back('\0');
        os.write(
            reinterpret_cast<const char*>(content.data()),
            get_size()
        );
        //TODO: should we update the header now?
    }

    Elf32_Shdr* header;

private:
    std::vector<char> content;
};


typedef struct Section {
    std::vector<uint32_t> data;
    uint32_t            offset;
    Elf32_Shdr         *header;

    // returns the size of the section in bytes
    size_t size() const {
        return data.size() * sizeof(uint32_t);
    }
} Section;

// what we need for the assembler and what we dont need
// Relocatable object files do not need a program header table. (solaris)
// A relocateble object must have a section header table

class ELF32{
    public:
        ELF32(void);
        ~ELF32();

        size_t sections_count() const;
        size_t section_headers_count() const;
        size_t section_content_size(Section& s) const;
        size_t store_label(std::string label, bool is_global);
        size_t store_regular_string(std::string str);
        size_t store_section_name(std::string);

        //TODO: templatize the following two
        void   add_to_text(itype32_t);
        void   add_to_text(rtype32_t);

        void   add_to_data();
        void   add_to_symtab(Elf32_Sym& symbol);

        void   serialize(std::ostream& os);
        void   deserialize();

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

        Elf32_Shdr                *symtab_sh;

        StringTable strtab;                        /* string table (regular) */
        StringTable shstrtab;                      /* section header strtab  */
        std::vector<Elf32_Sym>    symtab;          /* symbol table           */
};

void write_elf(ELF32& elf, std::string filename);

#endif
