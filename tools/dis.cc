#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include "linux/defs.h"  // Defines ELF32 structures and constants

void print_elf_header(const Elf32_Ehdr& hdr) {
    std::cout << "====== ELF32 Header ======\n";
    std::cout << "Magic:      ";
    for (int i = 0; i < EI_NIDENT; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(hdr.e_ident[i]) << ' ';
    std::cout << "\n";

    std::cout << "Class:      " << (hdr.e_ident[EI_CLASS] == ELFCLASS32 ? "ELF32" : "Invalid") << '\n';
    std::cout << "Data:       " << (hdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" : "Big Endian") << '\n';
    std::cout << "Version:    " << static_cast<int>(hdr.e_ident[EI_VERSION]) << '\n';
    std::cout << "OS/ABI:     " << static_cast<int>(hdr.e_ident[EI_OSABI]) << '\n';
    std::cout << "Type:       " << hdr.e_type << '\n';
    std::cout << "Machine:    " << hdr.e_machine << '\n';
    std::cout << "Entry:      0x" << std::hex << hdr.e_entry << '\n';
    std::cout << "PH offset:  " << std::dec << hdr.e_phoff << '\n';
    std::cout << "SH offset:  " << hdr.e_shoff << '\n';
    std::cout << "Flags:      0x" << std::hex << hdr.e_flags << '\n';
    std::cout << "EH size:    " << std::dec << hdr.e_ehsize << '\n';
    std::cout << "PH entsize: " << hdr.e_phentsize << '\n';
    std::cout << "PH num:     " << hdr.e_phnum << '\n';
    std::cout << "SH entsize: " << hdr.e_shentsize << '\n';
    std::cout << "SH num:     " << hdr.e_shnum << '\n';
    std::cout << "SH str idx: " << hdr.e_shstrndx << '\n';
    std::cout << "============================\n\n";
}

void print_section_headers(std::ifstream& file, const Elf32_Ehdr& hdr) {
    std::vector<Elf32_Shdr> sections(hdr.e_shnum);
    file.seekg(hdr.e_shoff, std::ios::beg);
    file.read(reinterpret_cast<char*>(sections.data()), hdr.e_shentsize * hdr.e_shnum);

    if (!file) {
        std::cerr << "Failed to read section headers.\n";
        return;
    }

    // Load section header string table (.shstrtab)
    const Elf32_Shdr& shstr_hdr = sections[hdr.e_shstrndx];
    std::vector<char> shstrtab(shstr_hdr.sh_size);
    file.seekg(shstr_hdr.sh_offset, std::ios::beg);
    file.read(shstrtab.data(), shstr_hdr.sh_size);

    std::cout << "======= Section Headers =======\n";
    std::cout << std::right
              << std::setw(4)  << "Idx" << " "
              << std::setw(18) << "Name" << " "
              << std::setw(10) << "Type" << " "
              << std::setw(10) << "Offset" << " "
              << std::setw(10) << "Size" << " "
              << std::setw(10) << "Addr"
              << "\n";
    std::cout << "---------------------------------------------------------------\n";

    for (size_t i = 0; i < sections.size(); ++i) {
        const Elf32_Shdr& sh = sections[i];
        const char* name = (sh.sh_name < shstrtab.size()) ? &shstrtab[sh.sh_name] : "<bad>";

        std::cout << std::right
                  << std::setw(4)  << i << " "
                  << std::setw(18) << name << " "
                  << std::setw(10) << std::hex << sh.sh_type << " "
                  << "0x" << std::setw(8) << std::setfill('0') << sh.sh_offset << " "
                  << "0x" << std::setw(8) << sh.sh_size << " "
                  << "0x" << std::setw(8) << sh.sh_addr
                  << std::dec << std::setfill(' ') << "\n";
    }

    std::cout << "===============================\n";
}

void print_text_section(std::ifstream& file, const Elf32_Ehdr& hdr) {
    // Load section headers
    std::vector<Elf32_Shdr> sections(hdr.e_shnum);
    file.seekg(hdr.e_shoff, std::ios::beg);
    file.read(reinterpret_cast<char*>(sections.data()), hdr.e_shentsize * hdr.e_shnum);

    // Load section header string table
    const Elf32_Shdr& shstr_hdr = sections[hdr.e_shstrndx];
    std::vector<char> shstrtab(shstr_hdr.sh_size);
    file.seekg(shstr_hdr.sh_offset, std::ios::beg);
    file.read(shstrtab.data(), shstr_hdr.sh_size);

    // Find the .text section
    const Elf32_Shdr* text_sh = nullptr;
    for (const auto& sh : sections) {
        const char* name = (sh.sh_name < shstrtab.size()) ? &shstrtab[sh.sh_name] : "<bad>";
        if (std::strcmp(name, ".text") == 0) {
            text_sh = &sh;
            break;
        }
    }

    if (!text_sh) {
        std::cerr << "No .text section found.\n";
        return;
    }

    // Read .text contents
    std::vector<uint8_t> text_data(text_sh->sh_size);
    file.seekg(text_sh->sh_offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(text_data.data()), text_sh->sh_size);

    std::cout << "======= .text Section (instructions) =======\n";
    std::cout << "Addr: 0x" << std::hex << text_sh->sh_addr << "\n";

    // Print 4 bytes per line as "instruction" (adjust if arch differs)
    for (size_t i = 0; i < text_data.size(); i += 4) {
        std::cout << "0x" << std::setw(8) << std::setfill('0')
                  << std::hex << (text_sh->sh_addr + i) << " : ";

        for (size_t j = 0; j < 4 && i + j < text_data.size(); ++j) {
            std::cout << std::setw(2) << static_cast<int>(text_data[i + j]) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "===========================================\n";
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <elf32_file>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << '\n';
        return 1;
    }

    Elf32_Ehdr hdr;
    file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!file) {
        std::cerr << "Failed to read ELF header.\n";
        return 1;
    }

    if (std::memcmp(hdr.e_ident, ELFMAG, SELFMAG) != 0) {
        std::cerr << "Not an ELF file.\n";
        return 1;
    }

    if (hdr.e_ident[EI_CLASS] != ELFCLASS32) {
        std::cerr << "Not a 32-bit ELF file.\n";
        return 1;
    }

    print_elf_header(hdr);
    print_section_headers(file, hdr);
    print_text_section(file, hdr);
    return 0;
}

