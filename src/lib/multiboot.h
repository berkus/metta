//
// Copyright 2007 - 2009, Stanislav Karchebnyy <berkus+metta@madfire.net>
//
// Distributed under the Boost Software License, Version 1.0.
// (See file LICENSE_1_0.txt or a copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "types.h"
#include "elf.h"
#include "panic.h"

/*!
* \brief Defines an interface to the multiboot header.
* \ingroup Boot
*/
class multiboot_t
{
public:
    /*!
    * Header flags.
    */
    enum {
        FLAG_MEM     = 0x0001,
        FLAG_DEVICE  = 0x0002,
        FLAG_CMDLINE = 0x0004,
        FLAG_MODS    = 0x0008,
        FLAG_AOUT    = 0x0010,
        FLAG_ELF     = 0x0020,
        FLAG_MMAP    = 0x0040,
        FLAG_CONFIG  = 0x0080,
        FLAG_LOADER  = 0x0100,
        FLAG_APM     = 0x0200,
        FLAG_VBE     = 0x0400
    };

    class mmap_entry_t;
    struct modinfo_t;

    class mmap_t
    {
    public:
        mmap_entry_t* first_entry();
        mmap_entry_t* next_entry(mmap_entry_t* prev);
        uint32_t      size();
        void          dump();

        void          set_addr(address_t new_addr)   { addr = reinterpret_cast<mmap_entry_t*>(new_addr); } // Can't resist being evil ^v^
        void          set_length(size_t new_length)  { length = new_length; }

    private:
        uint32_t      length;
        mmap_entry_t* addr;
    } PACKED;

    class mmap_entry_t
    {
    public:
        uint64_t address() const { return base_addr; }
        uint64_t size() const    { return length; }
        bool     is_free() const { return type == 1; }

        void     set_entry_size(uint32_t new_size) { entry_size = new_size; }

    private:
        uint32_t entry_size;//!< size of the mmap entry
        uint64_t base_addr; //!< base address of memory region (physical)
        uint64_t length;    //!< size of memory region
        uint32_t type;      //!< type == 1 for free regions, anything else means occupied

        friend class multiboot_t::mmap_t;
    } PACKED;

    /*!
    * Boot information passed in by multiboot loader.
    */
    struct header_t
    {
        uint32_t flags; // enum above

        // memory here usually excludes occupied memory in mmapinfo
        uint32_t mem_lower; //!< kilobytes of lower memory
        uint32_t mem_upper; //!< kilobytes of upper memory

        uint32_t boot_device;

        char* cmdline;

        uint32_t   modules_count;
        modinfo_t* modules;

        /* ELF information */
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;

        mmap_t mmap;

        uint32_t drives_length;
        uint32_t drives_addr;

        uint32_t config_table;

        uint32_t boot_loader_name;

        uint32_t apm_table;

        uint32_t vbe_control_info;
        uint32_t vbe_mode_info;
        uint32_t vbe_mode;
        uint32_t vbe_interface_seg;
        uint32_t vbe_interface_off;
        uint32_t vbe_interface_len;
    } PACKED;

    struct modinfo_t
    {
        uint32_t mod_start;
        uint32_t mod_end;
        const char* str;
        uint32_t reserved;
    } PACKED;

    multiboot_t(header_t* h = NULL)
        : header(NULL)
        , strtab(NULL)
        , symtab(NULL)
    {
        set_header(h);
    }

    void set_header(header_t* h);

    inline uint32_t lower_mem()  const { return header->mem_lower; }
    inline uint32_t upper_mem()  const { return header->mem_upper; }
    inline uint32_t flags()      const { return header->flags; }

    inline bool flags_set(uint32_t flag_mask) const { return (flags() & flag_mask) == flag_mask; }

    inline modinfo_t* module(uint32_t i) const
    {
        ASSERT(sizeof(modinfo_t)==16);
        if (flags_set(FLAG_MODS)
            && header->modules_count
            && i < header->modules_count)
        {
            return &header->modules[i];
        }
        return 0;
    }

    inline uint32_t module_count() const
    {
        if (flags_set(FLAG_MODS))
            return header->modules_count;
        return 0;
    }

    /**
    * Return highest address occupied by loaded modules.
    **/
    inline address_t last_mod_end() const
    {
        if (!flags_set(FLAG_MODS))
            return 0;

        address_t top = 0;
        for (uint32_t k = 0; k < header->modules_count; k++)
        {
            modinfo_t* m = module(k);
            if (m && m->mod_end > top)
                top = m->mod_end;
        }
        return top;
    }

    inline uint32_t elf_num_headers()     const { return header->num; }
    inline uint32_t elf_header_size()     const { return header->size; }
    inline uint32_t elf_header_addr()     const { return header->addr; }
    inline uint32_t elf_strtab_index()    const { return header->shndx; }
    inline elf32::section_header* symtab_start() const
    {
        return symtab;
    }
    inline address_t symtab_end() const
    {
        return symtab ? (address_t)symtab->addr + symtab->size : 0;
    }
    inline elf32::section_header* strtab_start() const
    {
        return strtab;
    }
    inline address_t strtab_end() const
    {
        return strtab ? (address_t)strtab->addr + strtab->size : 0;
    }

    inline bool is_elf() const { return flags_set(FLAG_ELF); }
    inline bool has_mem_info() const { return flags_set(FLAG_MEM); }

    inline bool has_mmap_info() const { return flags_set(FLAG_MMAP); }
    mmap_t* memory_map() const;

    uint32_t  size();
    void      copy(address_t target);

private:
    header_t*              header;
    elf32::section_header* strtab;
    elf32::section_header* symtab;
};

// kate: indent-width 4; replace-tabs on;
// vim: set et sw=4 ts=4 sts=4 cino=(4 :
