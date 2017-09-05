// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "core.h"

Int64 roundi(double val)
{
    return (Int64) (val > 0 ? val + 0.5 : val - 0.5);
}

// EOF