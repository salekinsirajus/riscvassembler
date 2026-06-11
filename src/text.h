#include "section.h"

class Text : public Section<uint32_t>
{
public:
    Text();

    void serialize(std::ostream &os, byte_order bo) override;

    void push(const uint32_t instr);

    size_t size_in_bytes() const override;
private:

    std::vector<uint32_t> data;
};
