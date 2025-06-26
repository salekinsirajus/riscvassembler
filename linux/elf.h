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

#include <cctype> // for std::isprint

//TODO: move the implementation to a separate file
class StringTable {
public:
    StringTable() {
        // Start with a null terminator
        content.push_back('\0');
    }

    // Adds a null-terminated string and returns its offset
    size_t add_string(const char* str) {
        if (!str) throw std::invalid_argument("Null string passed");

        // Insert before the final null terminator
        size_t offset = content.size() - 1;
        size_t len = std::strlen(str);

        content.insert(content.end() - 1, str, str + len);
        content.insert(content.end() - 1, '\0');

        return offset;
    }

    // Access string at offset
    const char* get_string(size_t offset) const {
        if (offset >= content.size()) throw std::out_of_range("Offset out of bounds");
        return &content[offset];
    }

    // Returns entire string table buffer
    const char* get_all() const {
        return content.data();
    }

    // Returns total size including both nulls
    size_t get_size() const {
        return content.size();
    }

    // Write string table to output stream and update header
    void serialize(std::ostream &os) const {
        if (!header) throw std::runtime_error("Header not set");
        os.seekp(0, std::ios_base::end); //get to the end

        header->sh_offset = static_cast<uint32_t>(os.tellp());
        header->sh_size = get_size();

        os.write(reinterpret_cast<const char*>(content.data()), content.size());
    }

    void print_content() const {
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


    Elf32_Shdr* header;

private:
    std::vector<char> content;
};

class Symtab {
public:
    Symtab(){}

    void push_back(Elf32_Sym& sym){
        data.push_back(sym);
    }

    size_t get_size() const {
        return (data.size() * sizeof(Elf32_Sym));
    }

    void serialize(std::ostream& os){
        if (data.size() > 0){
            printf("size of symtab: %lu", data.size());
            header->sh_offset = static_cast<uint32_t>(os.tellp());
            header->sh_size = get_size();
            os.write(reinterpret_cast<const char*>(data.data()), get_size());
        }
    }

    Elf32_Shdr* header;

private:
     std::vector<Elf32_Sym>    data;          /* symbol table           */
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
        void   add_variable_to_symtab(
                   std::string name,
                   std::string value,
                   std::string section
               );

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

        StringTable               *strtab;         /* string table (regular) */
        StringTable               *shstrtab;       /* section header strtab  */
        Symtab                    *symtab;         /* symbol table           */
};

void write_elf(ELF32& elf, std::string filename);

#endif
