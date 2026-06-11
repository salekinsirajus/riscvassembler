#include "section.h"

class StringTable : Section<char>
{
public:
    StringTable(bool _is_shstrtab=false);

    // Adds a null-terminated string and returns its offset
    size_t add_string(const char* str);

    // Access string at offset
    const char* get_string(size_t offset) const;

    // Returns total size including both nulls
    size_t size() const;

    size_t size_in_bytes() const override;

    // Write string table to output stream and update header
    void serialize(std::ostream &os, byte_order bo) override;

    // Debugging aid
    void print_content() const;

    void init_table();

    bool is_shstrtab() const;

    Elf32_Shdr* header;

private:
    std::vector<char> content;
    bool is_section_header_string_table;
};
