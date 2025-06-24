#ifndef DEFS_H
#define DEFS_H

#define ELFCLASS32    1
#define ELFCLASS64    2

#define ELFDATA2LSB   1
#define ELFDATA2MSB   2

#define ELFMAG        "\177ELF"
#define SELFMAG       4

/* These constants are for the segment types stored in the image headers */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7               /* Thread local storage segment */
#define PT_LOOS    0x60000000      /* OS-specific */
#define PT_HIOS    0x6fffffff      /* OS-specific */
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff
#define PT_GNU_EH_FRAME    (PT_LOOS + 0x474e550)
#define PT_GNU_STACK    (PT_LOOS + 0x474e551)
#define PT_GNU_RELRO    (PT_LOOS + 0x474e552)
#define PT_GNU_PROPERTY    (PT_LOOS + 0x474e553)

/* Enums */
// File types.
// See current registered ELF types at:
//    http://www.sco.com/developers/gabi/latest/ch4.eheader.html
enum {
  ET_NONE = 0,        // No file type
  ET_REL = 1,         // Relocatable file
  ET_EXEC = 2,        // Executable file
  ET_DYN = 3,         // Shared object file
  ET_CORE = 4,        // Core file
  ET_LOOS = 0xfe00,   // Beginning of operating system-specific codes
  ET_HIOS = 0xfeff,   // Operating system-specific
  ET_LOPROC = 0xff00, // Beginning of processor-specific codes
  ET_HIPROC = 0xffff  // Processor-specific
};

// e_ident size and indices.
enum {
  EI_MAG0 = 0,       // File identification index.
  EI_MAG1 = 1,       // File identification index.
  EI_MAG2 = 2,       // File identification index.
  EI_MAG3 = 3,       // File identification index.
  EI_CLASS = 4,      // File class.
  EI_DATA = 5,       // Data encoding.
  EI_VERSION = 6,    // File version.
  EI_OSABI = 7,      // OS/ABI identification.
  EI_ABIVERSION = 8, // ABI version.
  EI_PAD = 9,        // Start of padding bytes.
  EI_NIDENT = 16     // Number of bytes in e_ident.
};

// Segment flag bits.
enum : unsigned {
  PF_X = 1,                // Execute
  PF_W = 2,                // Write
  PF_R = 4,                // Read
  PF_MASKOS = 0x0ff00000,  // Bits for operating system-specific semantics.
  PF_MASKPROC = 0xf0000000 // Bits for processor-specific semantics.
};

//sh-type definitions
#define SHT_NULL                0        /* Section header table entry unused */
#define SHT_PROGBITS            1        /* Program data */
#define SHT_SYMTAB              2        /* Symbol table */
#define SHT_STRTAB              3        /* String table */
#define SHT_RELA                4        /* Relocation entries with addends */
#define SHT_HASH                5        /* Symbol hash table */
#define SHT_DYNAMIC             6        /* Dynamic linking information */
#define SHT_NOTE                7        /* Notes */
#define SHT_NOBITS              8        /* Program space with no data (bss) */
#define SHT_REL                 9        /* Relocation entries, no addends */
#define SHT_SHLIB              10        /* Reserved */
#define SHT_DYNSYM             11        /* Dynamic linker symbol table */
#define SHT_INIT_ARRAY         14        /* Array of constructors */
#define SHT_FINI_ARRAY         15        /* Array of destructors */
#define SHT_PREINIT_ARRAY      16        /* Array of pre-constructors */
#define SHT_GROUP              17        /* Section group */
#define SHT_SYMTAB_SHNDX       18        /* Extended section indeces */
#define SHT_NUM                19        /* Number of defined types.  */
#define SHT_LOOS           0x60000000    /* Start OS-specific.  */
#define SHT_GNU_ATTRIBUTES 0x6ffffff5    /* Object attributes.  */
#define SHT_GNU_HASH       0x6ffffff6    /* GNU-style hash table.  */
#define SHT_GNU_LIBLIST    0x6ffffff7    /* Prelink library list */
#define SHT_CHECKSUM       0x6ffffff8    /* Checksum for DSO content.  */
#define SHT_LOSUNW         0x6ffffffa    /* Sun-specific low bound.  */
#define SHT_SUNW_move      0x6ffffffa
#define SHT_SUNW_COMDAT    0x6ffffffb
#define SHT_SUNW_syminfo   0x6ffffffc
#define SHT_GNU_verdef     0x6ffffffd    /* Version definition section.  */
#define SHT_GNU_verneed    0x6ffffffe    /* Version needs section.  */
#define SHT_GNU_versym     0x6fffffff    /* Version symbol table.  */
#define SHT_HISUNW         0x6fffffff    /* Sun-specific high bound.  */
#define SHT_HIOS           0x6fffffff    /* End OS-specific type */
#define SHT_LOPROC         0x70000000    /* Start of processor-specific */
#define SHT_HIPROC         0x7fffffff    /* End of processor-specific */
#define SHT_LOUSER         0x80000000    /* Start of application-specific */
#define SHT_HIUSER         0x8fffffff    /* End of application-specific */ 

//sh-flag definitions
#define SHF_WRITE             (1 << 0)    /* Writable */
#define SHF_ALLOC             (1 << 1)    /* Occupies memory during execution */
#define SHF_EXECINSTR         (1 << 2)    /* Executable */
#define SHF_MERGE             (1 << 4)    /* Might be merged */
#define SHF_STRINGS           (1 << 5)    /* Contains nul-terminated strings */
#define SHF_INFO_LINK         (1 << 6)    /* `sh_info' contains SHT index */
#define SHF_LINK_ORDER        (1 << 7)    /* Preserve order after combining */
#define SHF_OS_NONCONFORMING  (1 << 8)    /* Non-standard OS specific handling required */
#define SHF_GROUP             (1 << 9)    /* Section is member of a group.  */
#define SHF_TLS              (1 << 10)    /* Section hold thread-local data.  */
#define SHF_COMPRESSED       (1 << 11)    /* Section with compressed data. */
#define SHF_MASKOS          0x0ff00000    /* OS-specific.  */
#define SHF_MASKPROC        0xf0000000    /* Processor-specific */
#define SHF_ORDERED          (1 << 30)    /* Special ordering requirement (Solaris).  */
#define SHF_EXCLUDE         (1U << 31)    /* Section is excluded unless referenced or allocated (Solaris).*/

// ELF Special Section Indices
#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_BEFORE    0xff00
#define SHN_AFTER     0xff01
#define SHN_HIPROC    0xff1f
#define SHN_LOOS      0xff20
#define SHN_HIOS      0xff3f
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_XINDEX    0xffff
#define SHN_HIRESERVE 0xffff

//for 32-bit 
typedef uint16_t Elf32_Half;   /* unsigned half int */
typedef uint32_t Elf32_Off;    /* unsigned offset */
typedef uint32_t Elf32_Addr;   /* unsigned address */
typedef uint32_t Elf32_Word;   /* unsigned word */
typedef int32_t  Elf32_Sword;  /* signed word */
//for 64-bit 
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Addr;
typedef uint32_t Elf64_Word;
typedef  int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef  int64_t Elf64_Sxword;

#define ELF_NIDENT 16
typedef struct {
    uint8_t    e_ident[ELF_NIDENT];/* ELF identification */
    Elf32_Half e_type;             /* Object file type */
    Elf32_Half e_machine;          /* Machine type */
    Elf32_Word e_version;          /* Object file version */
    Elf32_Addr e_entry;            /* Entry point address */
    Elf32_Off  e_phoff;            /* Program header offset */
    Elf32_Off  e_shoff;            /* Section header offset */
    Elf32_Word e_flags;            /* Processor-specific flags */
    Elf32_Half e_ehsize;           /* ELF header size */
    Elf32_Half e_phentsize;        /* Size of program header entry */
    Elf32_Half e_phnum;            /* Number of program header entries */
    Elf32_Half e_shentsize;        /* Size of section header entry */
    Elf32_Half e_shnum;            /* Number of section header entries */
    Elf32_Half e_shstrndx;         /* Section name string table index */
} Elf64_Ehdr;

/* Program Header */
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

/* for 32-bit */
/* ELF Header */
//from llvm - https://llvm.org/doxygen/BinaryFormat_2ELF_8h_source.html
typedef struct Elf32_Ehdr {
  unsigned char e_ident[EI_NIDENT]; // ELF Identification bytes
  Elf32_Half e_type;                // Type of file (see ET_* below)
  Elf32_Half e_machine;   // Required architecture for this file (see EM_*)
  Elf32_Word e_version;   // Must be equal to 1
  Elf32_Addr e_entry;     // Address to jump to in order to start program
  Elf32_Off  e_phoff;     // Program header table's file offset, in bytes
  Elf32_Off  e_shoff;     // Section header table's file offset, in bytes
  Elf32_Word e_flags;     // Processor-specific flags
  Elf32_Half e_ehsize;    // Size of ELF header, in bytes
  Elf32_Half e_phentsize; // Size of an entry in the program header table
  Elf32_Half e_phnum;     // Number of entries in the program header table
  Elf32_Half e_shentsize; // Size of an entry in the section header table
  Elf32_Half e_shnum;     // Number of entries in the section header table
  Elf32_Half e_shstrndx;  // Sect hdr table index of sect name string table
} Elf32_Ehdr;

/* Program Header */
typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;

/* Section Header */
typedef struct Elf32_Shdr
{
  Elf32_Word    sh_name;         /* Section name (string tbl index) */
  Elf32_Word    sh_type;         /* Section type */
  Elf32_Word    sh_flags;        /* Section flags */
  Elf32_Addr    sh_addr;         /* Section virtual addr at execution */
  Elf32_Off     sh_offset;       /* Section file offset */
  Elf32_Word    sh_size;         /* Section size in bytes */
  Elf32_Word    sh_link;         /* Link to another section */
  Elf32_Word    sh_info;         /* Additional section information */
  Elf32_Word    sh_addralign;    /* Section alignment */
  Elf32_Word    sh_entsize;      /* Entry size if section holds table */
} Elf32_Shdr;

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define STB_LOCAL   0
#define STB_GLOBAL  1
#define STB_WEAK    2
#define STB_HIPROC 13
#define STB_LOPROC 15

// symbol table
typedef struct {
    Elf32_Word     st_name;
    Elf32_Addr     st_value;
    Elf32_Word     st_size;
    unsigned char  st_info;
    unsigned char  st_other;
    Elf32_Half     st_shndx;
} Elf32_Sym;

typedef struct {
    Elf64_Word     st_name;
    unsigned char  st_info;
    unsigned char  st_other;
    Elf64_Half     st_shndx;
    Elf64_Addr     st_value;
    Elf64_Xword    st_size;
} Elf64_Sym;

#endif //DEFS_H
