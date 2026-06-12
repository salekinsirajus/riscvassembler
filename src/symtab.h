#include "section.h"
#include "defs.h"

class Symtab : Section<Elf32_Sym>
{
    public:
        Symtab();
    
        void push(Elf32_Sym& sym);
        void serialize(std::ostream& os, byte_order bo) override;
        size_t size() const;
        size_t size_in_bytes() const override;
    
        Elf32_Shdr* header;
    
    private:
         std::vector<Elf32_Sym> data;
};

