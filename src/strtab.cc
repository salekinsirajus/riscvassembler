#include "strtab.h"

StringTable::StringTable(bool _is_shstrtab)
{
    init_table();
    is_section_header_string_table = _is_shstrtab;
}

void StringTable::init_table(){
    if (content.empty()){
       content.push_back('\0');
    }
}

bool StringTable::is_shstrtab() const {
    return is_section_header_string_table;
}

size_t StringTable::add_string(const char* str) {
    if (!str) throw std::invalid_argument("Null string passed");

    size_t offset;
    size_t begin;
	size_t len = std::strlen(str);

    // special case for empty table
    if (content.size() < 2){
        offset = 1;
        content.insert(content.end(), '\0');
    }
    else {
        offset = content.size() - 1;
    }

    content.insert(content.end() - 1, str, str + len);
    content.insert(content.end() - 1, '\0');

    header->sh_size = size_in_bytes();
    return offset;
}
const char* StringTable::get_string(size_t offset) const {
    if (offset >= content.size()) throw std::out_of_range("Offset out of bounds");
    return &content[offset];
}

size_t StringTable::size() const {
    return content.size();
}

size_t StringTable::size_in_bytes() const {
    return content.size() * sizeof(char);
}

void StringTable::serialize(std::ostream &os, byte_order bo) {
    if (!header)
    {
       throw std::runtime_error("Header not set");
    }

    os.seekp(0, std::ios_base::end); //get to the end

    header->sh_offset = static_cast<uint32_t>(os.tellp());
    header->sh_size = size_in_bytes();

    //endianness adjustment is not needed for a char (so we are ignoring the byte order param
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
