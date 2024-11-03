#ifndef ELF_H
#define ELF_H

#include <stdint.h>

//The following stuff are for 32-bit little-endian
typedef uint16_t Elf32_Half;   /* unsigned half int */
typedef uint32_t Elf32_Off;    /* unsigned offset */
typedef uint32_t Elf32_Addr;   /* unsigned address */
typedef uint32_t Elf32_Word;   /* unsigned word */
typedef int32_t  Elf32_Sword;  /* signed word */

#define ELF_NIDENT 16
typedef struct {
    uint8_t    e_ident[ELF_NIDENT];/* ELF identification */
    Elf32_Half e_type;            /* Object file type */
    Elf32_Half e_machine;         /* Machine type */
    Elf32_Word e_version;         /* Object file version */
    Elf32_Addr e_entry;           /* Entry point address */
    Elf32_Off  e_phoff;           /* Program header offset */
    Elf32_Off  e_shoff;           /* Section header offset */
    Elf32_Word e_flags;           /* Processor-specific flags */
    Elf32_Half e_ehsize;          /* ELF header size */
    Elf32_Half e_phentsize;       /* Size of program header entry */
    Elf32_Half e_phnum;           /* Number of program header entries */
    Elf32_Half e_shentsize;       /* Size of section header entry */
    Elf32_Half e_shnum;           /* Number of section header entries */
    Elf32_Half e_shstrndx;        /* Section name string table index */
} Elf64_Ehdr;

enum Elf_Ident {
	EI_MAG0		= 0, 			 // 0x7F
	EI_MAG1 	= 1, 			 // 'E'
	EI_MAG2 	= 2, 			 // 'L'
	EI_MAG3 	= 3, 			 // 'F'
	EI_CLASS 	= 4,			 // Arch (32/64)
	EI_DATA		= 5,             // Byte Order
	EI_OSABI    = 7,             // OS Specific
	EI_ABIVERSION=8,             // OS Specific
	EI_PAD		= 9				 // Padding
};

enum E_Type {
	ET_NONE		= 0,			// No file type
	ET_REL		= 1,			// Relocatable File
	ET_EXEC		= 2,			// Executable file
	ET_DYN		= 3,			// Shared object file
	ET_CORE		= 4,			// Core file	
	ET_LOPROC	= 0xff00,		// Processor-specific
	ET_HIPROC	= 0xffff        // Processor-specific
};

// WIP

typedef struct {
    uint32_t p_type;    /* Type of segment */
    uint32_t p_flags;   /* Segment attributes */
    uint64_t p_offset;  /* Offset in file */
    uint64_t p_vaddr;   /* Virtual address in memory */
    uint64_t p_paddr;   /* Physical address (not used) */
    uint64_t p_filesz;  /* Size of segment in file */
    uint64_t p_memsz;   /* Size of segment in memory */
    uint64_t p_align;   /* Alignment of segment */
} Elf64_Phdr;

#endif
