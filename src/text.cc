#include "text.h"

Text::Text()
{

}

void Text::push(const uint32_t instr)
{
    // add to text
    data.push_back(instr);

    header->sh_size = size_in_bytes();
}

size_t Text::size_in_bytes() const {
    return data.size() * sizeof(uint32_t);
}

// TODO: do it properly
void Text::serialize(std::ostream &os, byte_order bo)
{
    for (uint32_t t: data)
    {
        write<uint32_t>(os, t, bo);
    }
}
