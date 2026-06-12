#include "data.h"

Data::Data()
{

}

size_t Data::push(const uint32_t entry)
{
    size_t offset = data.size();
    // TODO: how do we know the endianness here?
    // TODO: once we know the byte order, we can use
    // TODO: the templatized write methods
    data.push_back((entry >> 24) & 0xFF);
    data.push_back((entry >> 16) & 0xFF);
    data.push_back((entry >> 8)  & 0xFF);
    data.push_back(entry         & 0xFF);

    header->sh_size = size_in_bytes();
    return offset;
}

size_t Data::push(std::string entry)
{
    std::vector<uint8_t> bin(entry.begin(), entry.end());
    size_t offset = data.size();
    data.insert(data.end(), bin.begin(), bin.end());

    header->sh_size = size_in_bytes();
    return offset;
}

// FIX this: the API is off
uint8_t Data::get_entry(size_t idx)
{
    return 0xDE;
}

size_t Data::size() const
{
    return data.size();
}

size_t Data::size_in_bytes() const
{
    return data.size() * sizeof(uint8_t);
}

void Data::serialize(std::ostream &os, byte_order bo)
{
    //TODO: also, what do we do with byte order?
    //it seems like we now is the time to accept value for byte-order from the user
    os.write(reinterpret_cast<const char*>(data.data()), data.size());
}
