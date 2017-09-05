// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include math.h separately - use core.h instead!"
#endif

template <typename T>
T absv(T v)
{
    return v < 0 ? -v : +v;
}

template <typename T>
T maxv(T a, T b)
{
    return a > b ? a : b;
}

template <typename T>
T pwr(T b, T p)
{
    T tmp;

    if (p == 0) return 1;
    if (p == 1) return b;

    tmp = pwr(b, p / 2);

    if (p % 2 == 0)
    {
        return tmp * tmp;
    }
    else
    {
        return b * tmp * tmp;
    }
}

// TO DO: make this more efficient
// using shift multiplication trick (10 = 8 + 2)

template <typename T>
T pwr10(T p)
{
    T tmp;

    if (p == 0) return 1;
    if (p == 1) return 10;

    tmp = pwr( (T) 10, p / 2);

    if (p % 2 == 0)
    {
        return tmp * tmp;
    }
    else
    {
        return (T) 10 * tmp * tmp;
    }
}

extern Int64 roundi(double val);

// EOF