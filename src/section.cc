#include "section.h"
#include "defs.h"

size_t Section::size_in_bytes() const {
    return data.size() * sizeof(uint32_t);
}

size_t Section::last_index() const {
    if (data.size() > 0){
        return data.size() - 1;
    }

    return 0;
}

size_t Section::next_index() const {
    return data.size();
}

uint32_t Section::get_entry(size_t idx) {
    if (idx >= data.size() || idx < 0) return 0xFFFFFFFF;

    return data[idx];
}

// TODO: remove this in favor of `insert`
bool Section::update_entry(size_t idx, uint32_t val){
    if (idx >= data.size() || idx < 0){
        return false;
    }

    data[idx] = val;
    return true;
}

bool Section::push(uint32_t entry) {
    data.push_back(entry);
    header->sh_size += sizeof(entry);
}

bool Section::insert(size_t idx, uint32_t value) {
    return update_entry(idx, value);
}

void Section::serialize(std::ostream &out, byte_order bo){
    for (uint32_t x: data)
    {
       write<uint32_t>(out, x, bo);
    } 
}
