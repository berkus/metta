#pragma once

#include "types.h"
#include "leb128.h"
#include "dwarf_parser.h"
#include "dwarf_abbrev.h"
#include <map>

/* Compilation Unit Header */
/* Resides in: .debug_info */
/* Referenced from: .debug_aranges */
/* References: .debug_abbrev */
class cuh_t
{
public:
    uint32_t unit_length;
    uint16_t version;
    uint32_t debug_abbrev_offset;
    uint8_t  address_size;

    void decode(address_t from, size_t& offset);
};

class form_reader_t;

/* Debug Information Entry */
/* Resides in: .debug_info after cuh */
class die_t
{
public:
    uleb128_t tag;
    typedef std::map<uleb128_t, form_reader_t*> attr_map;
    attr_map node_attributes;
    dwarf_parser_t& parser;
    //temp perversion
    address_t low_pc, high_pc;

    die_t(dwarf_parser_t& p) : parser(p) {}

    void decode(address_t from, size_t& offset);

    bool is_last()
    {
        return tag == 0;
    }
};

class dwarf_debug_info_t
{
public:
    address_t start;
    size_t    size;
    dwarf_debug_abbrev_t& abbrevs;

    dwarf_debug_info_t(address_t st, size_t sz, dwarf_debug_abbrev_t& ab)
        : start(st)
        , size(sz)
        , abbrevs(ab)
    {
    }

    cuh_t get_cuh(size_t& offset)
    {
        cuh_t cuh;
        cuh.decode(start, offset);
        return cuh;
    }

    abbrev_declaration_t* find_abbrev(uint32_t abbreviation_code)
    {
        return abbrevs.find_abbrev(abbreviation_code);
    }
};