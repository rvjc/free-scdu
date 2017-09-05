// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "core.h"

Allocs allocs_m;

const Allocs& allocs = allocs_m;

Allocs::Allocs()
{
    cur = 0;
    max = 0;
}

// EOF