// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "core.h"

Str::Str()
{
    mLen = 0;
    mCap = STRBUF_SIZE - 1;
    mBuf[0] = 0;
    mC = mBuf;
}

Str::~Str()
{
    if ( mC != mBuf )
    {
        memFree(&mC, mCap + 1);
    }
}

Str::Str(const Str& str)
{
    mLen = (Uint16) str.mLen;

    if ( mLen < STRBUF_SIZE)
    {
        mCap = STRBUF_SIZE - 1;
        mC = mBuf;
    }
    else
    {
        mCap = mLen;
        mC = 0;
        memAlloc(&mC, mCap + 1);
    }

    strcpy(mC, str.mC);
}

Str::Str(const char c)
{
    mLen = c == 0 ? 0 : 1;
    mCap = STRBUF_SIZE - 1;
    mC = mBuf;

    if ( mLen == 0)
    {
        mC[0] = 0;
    }
    else
    {
        mC[0] = c;
        mC[1] = 0;
    }
}

Str::Str(const char* s)
{
    mLen = (Uint16) strlen(s);

    if ( mLen < STRBUF_SIZE)
    {
        mCap = STRBUF_SIZE - 1;
        mC = mBuf;
    }
    else
    {
        mCap = mLen;
        mC = 0;
        memAlloc(&mC, mCap + 1);
    }

    strcpy(mC, s);
}

Str& Str::operator=(const Str& rhs)
{
    mLen = rhs.mLen;

    if ( mC == mBuf )
    {
        if ( mLen >= STRBUF_SIZE )
        {
            mCap = mLen;
            mC = 0;
            memAlloc(&mC, mCap + 1);
        }
    }
    else
    {
        if ( mLen < STRBUF_SIZE )
        {
            memFree(&mC, mCap + 1);
            mCap = STRBUF_SIZE - 1;
            mC = mBuf;
        }
        else
        {
            memRealloc(&mC, mLen + 1, mCap + 1);
            mCap = mLen;
        }
    }

    strcpy(mC, rhs.mC);
    return *this;
}

Str& Str::operator=(const char* rhs)
{
    mLen = (Uint16) strlen(rhs);

    if ( mC == mBuf )
    {
        if ( mLen >= STRBUF_SIZE )
        {
            mCap = mLen;
            mC = 0;
            memAlloc(&mC, mCap + 1);
        }
    }
    else
    {
        if ( mLen < STRBUF_SIZE )
        {
            memFree(&mC, mCap + 1);
            mCap = STRBUF_SIZE - 1;
            mC = mBuf;
        }
        else
        {
            memRealloc(&mC, mLen + 1, mCap + 1);
            mCap = mLen;
        }
    }

    strcpy(mC, rhs);
    return *this;
}

Str& Str::operator=(const char rhs)
{
    mLen = rhs == 0 ? 0 : 1;

    if ( mC != mBuf )
    {
        memFree(&mC, mCap + 1);
    }
    else
    {
        mCap = STRBUF_SIZE - 1;
        mC = mBuf;
    }

    if ( mLen == 0)
    {
        mC[0] = 0;
    }
    else
    {
        mC[0] = rhs;
        mC[1] = 0;
    }

    return *this;
}

Str& Str::operator+=(const Str& rhs)
{
    Uint16 old_len, rhs_len;

    rhs_len = rhs.mLen;

    if ( rhs_len == 0 )
    {
        return *this;
    }

    old_len = mLen;
    mLen = (Uint16) (old_len + rhs_len);

    if ( mC == mBuf )
    {
        if ( mLen >= STRBUF_SIZE )
        {
            mCap = mLen;
            mC = 0;
            memAlloc(&mC, mCap + 1);
            strcpy(mC, mBuf);
        }
    }
    else
    {
        memRealloc(&mC, mLen + 1, mCap + 1);
        mCap = mLen;
    }

    strcpy(mC + old_len, rhs.mC);
    return *this;
}

Str& Str::operator+=(const char* rhs)
{
    Uint16 old_len, rhs_len;

    rhs_len = (Uint16) strlen(rhs);

    if ( rhs_len == 0 )
    {
        return *this;
    }

    old_len = mLen;
    mLen = (Uint16) (old_len + rhs_len);

    if ( mC == mBuf )
    {
        if ( mLen >= STRBUF_SIZE )
        {
            mCap = mLen;
            mC = 0;
            memAlloc(&mC, mCap + 1);
            strcpy(mC, mBuf);
        }
    }
    else
    {
        memRealloc(&mC, mLen + 1, mCap + 1);
        mCap = mLen;
    }

    strcpy(mC + old_len, rhs);
    return *this;
}

Str& Str::operator+=(const char rhs)
{
    if ( rhs == 0 )
    {
        return *this;
    }

    mLen = (Uint16) (mLen + 1);

    if ( mC == mBuf )
    {
        if ( mLen >= STRBUF_SIZE )
        {
            mCap = mLen;
            mC = 0;
            memAlloc(&mC, mCap + 1);
            strcpy(mC, mBuf);
        }
    }
    else
    {
        memRealloc(&mC, mLen + 1, mCap + 1);
        mCap = mLen;
    }

    mC[mLen-1] = rhs;
    mC[mLen] = 0;

    return *this;
}

bool Str::operator==(const Str& rhs) const
{
    return ( strcmp(mC, rhs.mC) == 0 );
}

bool Str::operator==(const char* rhs) const
{
    return ( strcmp(mC, rhs) == 0 );
}

bool Str::operator==(const char rhs) const
{
    return ( mC[0] == rhs && mC[1] == 0 );
}

Size Str::len() const
{
    return mLen;
}

const char* Str::cb() const
{
    return mC;
}

const char* Str::ce() const
{
    return mC + mLen;
}

int Str::cmp(const Str& str) const
{
    return strcmp(mC, str.mC);
}

int Str::cmp(const char* s) const
{
    return strcmp(mC, s);
}

Str operator+(const Str& lhs, const Str& rhs)
{
    Str str;

    Uint16 lhs_len = lhs.mLen;
    Uint16 rhs_len = rhs.mLen;

    str.mLen = (Uint16) (lhs_len + rhs_len);

    if ( str.mLen >= STRBUF_SIZE)
    {
        str.mCap = str.mLen;
        str.mC = 0;
        memAlloc(&str.mC, str.mCap + 1);
    }

    strcpy(str.mC, lhs.mC);
    strcpy(str.mC + lhs_len, rhs.mC);

    return str;
}

Str operator+(const Str& lhs, const char* rhs)
{
    Str str;

    Uint16 lhs_len = lhs.mLen;
    Uint16 rhs_len = (Uint16) strlen(rhs);

    str.mLen = (Uint16) (lhs_len + rhs_len);

    if ( str.mLen >= STRBUF_SIZE)
    {
        str.mCap = str.mLen;
        str.mC = 0;
        memAlloc(&str.mC, str.mCap + 1);
    }

    strcpy(str.mC, lhs.mC);
    strcpy(str.mC + lhs_len, rhs);

    return str;
}

Str operator+(const char* lhs, const Str& rhs)
{
    Str str;

    Uint16 lhs_len = (Uint16) strlen(lhs);
    Uint16 rhs_len = rhs.mLen;

    str.mLen = (Uint16) (lhs_len + rhs_len);

    if ( str.mLen >= STRBUF_SIZE)
    {
        str.mCap = str.mLen;
        str.mC = 0;
        memAlloc(&str.mC, str.mCap + 1);
    }

    strcpy(str.mC, lhs);
    strcpy(str.mC + lhs_len, rhs.mC);

    return str;
}

Str operator+(const Str& lhs, const char rhs)
{
    Str str;

    if ( rhs == 0 )
    {
        return str;
    }

    str.mLen = (Uint16) (lhs.mLen + 1);

    if ( str.mLen >= STRBUF_SIZE)
    {
        str.mCap = str.mLen;
        str.mC = 0;
        memAlloc(&str.mC, str.mCap + 1);
    }

    strcpy(str.mC, lhs.mC);
    str.mC[str.mLen-1] = rhs;
    str.mC[str.mLen] = 0;

    return str;
}

Str operator+(const char lhs, const Str& rhs)
{
    Str str;

    if ( lhs == 0 )
    {
        return str;
    }

    str.mLen = (Uint16) (rhs.mLen + 1);

    if ( str.mLen >= STRBUF_SIZE)
    {
        str.mCap = str.mLen;
        str.mC = 0;
        memAlloc(&str.mC, str.mCap + 1);
    }

    *str.mC = lhs;
    strcpy(str.mC + 1, rhs.mC);

    return str;
}

// the following is faster than strlen(s) == 0

bool strlenz(const char* s)
{
    return *s == 0;
}

// strlen() normally returns Size type
// int version is handy is certain situations

int strleni(const char* s)
{
    int n = 0;
    while ( *s++ != 0 ) n++;
    return n;
}

char* strncpyz(char* dst, const char* src, Size n)
{
    char* c = dst;
    Size s = strlen(src);

    if ( n > s )
    {
        n = s;
    }

    while ( n-- > 0 )
    {
        *c++ = *src++;
    }

   *c = 0;
   return dst;
}

char* strncatz(char* dst, const char* src, Size n)
{
    char* c = dst;
    Size d = strlen(dst);

    if ( n < d )
    {
        c += n;
    }
    else
    {
        c += d;
        n -= d;

        while ( n-- > 0 )
        {
            *c++ = *src++;
        }
    }

   *c = 0;
   return dst;
}

int snprintfz(char* s, Size n, const char* fmt, ... )
{
    int r;
    va_list args;

    va_start (args, fmt);

    r = vsnprintf(s, n, fmt, args);

    if ( r < 0 || r > (int) n)
    {
        s[0] = 0;
    }
    else
    {
        s[r] = 0;
    }

    va_end (args);

    return r;
}

int vsnprintfz (char* s, Size n, const char* fmt, va_list args)
{
    int r = vsnprintf(s, n, fmt, args);

    if ( r < 0 || r > (int) n)
    {
        s[0] = 0;
    }
    else
    {
        s[r] = 0;
    }

    return r;
}

bool strIsDec(char* s)
{
    char c;

    if ( *s == 0 ) return false;

    while ( (c = *s++) != 0 )
    {
        if ( c < '0' || c > '9' ) return false;
    }

    return true;
}

void strToLower(char* s)
{
    while ( char c = *s != 0 )
    {
        *s++ = (char) tolower(c);
    }
}

void strToUpper(char* s)
{
    while ( char c = *s != 0 )
    {
        *s++ = (char) toupper(c);
    }
}

void strnToLower(char* s, Size max)
{
    Size i;
    Size len = strlen(s);

    if ( len > max ) len = max;

    for (i = 0; i < len; i++)
    {
        s[i] = (char) tolower(s[i]);
    }
}

void strnToUpper(char* s, Size max)
{
    Size i;
    Size len = strlen(s);

    if ( len > max ) len = max;

    for (i = 0; i < len; i++)
    {
        s[i] = (char) toupper(s[i]);
    }
}

const char* findDupChar(const char* s)
{
    Size n;
    const char* a;
    const char* b;
    const char* e;

    n = strlen(s);
    e = s + n;

    for (a = s; a < e; a++)
    {
        for (b = a + 1; b < e; b++)
        {
            if ( *a == *b)
            {
                return a;
            }
        }
    }

    return 0;
}

const char* findOddChar(const char* s, const char* range)
{
    const char* r;
    char        c, d;

    while ( (c = *s) != 0 )
    {
        r = range;

        do { d = *r++; } while ( d != 0 && c != d);

        if ( d == 0 )
        {
            return s;
        }

        s++;
    }

    return 0;
}

int fitz(char* dst, const char* src, Size w, int p)
{
    Size s, m, l, r;

    if ( dst == 0 || src == 0 ) return -1;

    s = strlen(src);

    if (s == w)
    {
        memcpy(dst, src, s);
        dst[w] = 0;
        return (int) w;
    }

    if ( s < w)
    {
        memcpy(dst, src, s);
        if ( p == 0 )
        {
            dst[s] = 0;
            return (int) s;
        }
        else
        {
            memset(dst + s, p, w - s);
            dst[w] = 0;
            return (int) w;
        }
    }

    // s > w

    if ( w <= 2 )
    {
        memset(dst, '.', w);
        dst[w] = 0;
        return (int) w;
    }

    m = 2;
    l = (w - m)/2;
    r = w - l - m;

    memcpy(dst, src, l);
    memset(dst + l, '.', m);
    memcpy(dst + l + m, src + s - r, r);
    dst[w] = 0;

    return (int) w;
}

char* trimWhite(char* s)
{
    char *e;

    while ( isspace(*s) )
    {
        s++;
    }

    if ( *s == 0 )
    {
        return s;
    }

    e = s + strlen(s) - 1;

    while ( e > s && isspace(*e) )
    {
        e--;
    }

    *(e + 1) = 0;

    return s;
}

// EOF