// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
#error "do not include var.h separately - use core.h instead!"
#endif

const Size VAR_MAX_SELS = 8;
const Size VAR_SEL_MAX = 80;

struct Var
{
    const TypNum    typnum;
    const QtyNum    qtynum;
    const char*     min;
    const char*     max;
    const char*     sel;
};

// EOF