#include "symtab.h"

Symtab::Symtab(){ }

void Symtab::push(Elf32_Sym& sym){
    data.push_back(sym);
    header->sh_size = size_in_bytes();
}

size_t Symtab::size_in_bytes() const {
    return (data.size() * sizeof(Elf32_Sym));
}

void Symtab::serialize(std::ostream& os, byte_order bo){
    if (data.size() > 0){
        printf("size of symtab: %lu\n", data.size());
        header->sh_offset = static_cast<uint32_t>(os.tellp());
        header->sh_size = size_in_bytes();
        for (Elf32_Sym s : data)
        {
            s.serialize(os, bo);
        }
    }
}
