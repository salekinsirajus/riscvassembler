#include <cstdint>
#include <vector>
#include <iostream>
#include "defs.h"
#include "utils.h"

// TODO: templatize so it can be used for 64-bit as well
class Section {
    public:
        uint32_t            offset;
        Elf32_Shdr         *header;

        // returns the size of the section in bytes
        size_t size_in_bytes() const;

        size_t last_index() const;
 
        size_t next_index() const;

        // TODO: use a bool/flag to signal success or failure
        uint32_t get_entry(size_t idx);

        bool update_entry(size_t idx, uint32_t val);

        /* at the end */
        bool push(uint32_t entry);

        /* at a specific location */
        bool insert(size_t idx, uint32_t value);

        void serialize(std::ostream &out, byte_order bo);

    private:
        std::vector<uint32_t> data;
};
