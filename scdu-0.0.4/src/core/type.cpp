// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "core.h"

const TypDef typDefs[] =
{
    {   TYP_FLAG,     "flag",
        "1 = enabled; 0 = disabled"                                     },

    {   TYP_TEXT,     "text",
        "text string of arbitrary length"                               },

    {   TYP_INUM,     "inum",
        "integer number of arbitrary size"                              },

    {   TYP_FNUM,     "fnum",
        "floating-point number of arbitrary size and precision"         },

    {   TYP_PICK,     "pick",
        "selections using wildcards and/or letter combinations"         }
};

Pick::Pick()
{
    all = 0;
}

bool Pick::isNull() const
{
    return (all == 0);
}

int Pick::get(char chr) const
{
    // warning: avoid single char local var names
    // they will silently conflict with class members!!!

    int    bit;
    Uint64 mask;

    if ( chr >= 'a' && chr <= 'z' )
    {
        bit = chr - 'a';
    }
    else if ( chr >= 'A' && chr <= 'Z' )
    {
        bit = 26 + chr - 'A';
    }
    else
    {
        return -1;
    }

    mask = U64(1) << bit;

    return all & mask ? 1 : 0;
}

int Pick::set(bool val, const char* str)
{
    // warning: avoid single char local var names
    // they will silently conflict with class members!!!

    int    chr;
    int    cnt;
    int    bit;
    Uint64 mask;

    mask = 0;

    for ( cnt = 0; (chr = *str) != 0; str++, cnt++ )
    {
        if ( chr >= 'a' && chr <= 'z' )
        {
            bit = chr - 'a';
        }
        else if ( chr >= 'A' && chr <= 'Z' )
        {
            bit = 26 + chr - 'A';
        }
        else
        {
            return -1;
        }

        mask |= ( U64(1) << bit);
    }

    all = val ? all | mask : all & ~mask;
    return cnt;
}

// EOF