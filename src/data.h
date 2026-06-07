#include "section.h"

class Data : public Section<uint32_t>
{
public:
    Data();

    uint32_t get_entry(size_t idx);

    size_t size() const;

    size_t size_in_bytes() const override; 

    void serialize(std::ostream &os, byte_order bo) override;

    void push(const uint32_t instr);  

private:

    std::vector<uint32_t> data;
};
