#include "data.h"

Data::Data()
{

}

void Data::push(const uint32_t instr)
{
    // add to text
}

uint32_t Data::get_entry(size_t idx)
{
    return 0xDEADBEEF;
}

size_t Data::size() const
{
    return data.size();
}

size_t Data::size_in_bytes() const
{
    return data.size() * sizeof(uint32_t);
}

void Data::serialize(std::ostream &os, byte_order bo)
{

}
