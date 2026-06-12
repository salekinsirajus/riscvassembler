#include "section.h"

class Data : public Section<uint8_t>
{
public:
    Data();

    uint8_t get_entry(size_t idx);

    size_t size() const;

    size_t size_in_bytes() const override; 

    void serialize(std::ostream &os, byte_order bo) override;

    size_t push(const uint32_t entry);  
    size_t push(const uint8_t entry);
    size_t push(std::string entry);

private:

    std::vector<uint8_t> data;
};
