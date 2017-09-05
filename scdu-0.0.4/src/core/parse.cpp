// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "core.h"

static void per(Pen e, ...);

static Pen  penMutable = PE_OK;
const Pen&  pen = penMutable;

static char pemMutable[PEM_MAX + 1] = "";
const char* const pem = pemMutable;

const PerDef perDefs[] =
{
    { PE_OK,        "PE_OK",        "",                         ""                  },
    { PE_NULPTR,    "PE_NULPTR",    "null pointer",             ""                  },
    { PE_NUL,       "PE_NUL",       "null",                     ""                  },
    { PE_FMT,       "PE_FMT",       "invalid format",           ""                  },
    { PE_RANGE,     "PE_RANGE",     "out-of-range",             ""                  },
    { PE_WILDBAD,   "PE_WILDBAD",   "invalid wildcard",         ""                  },
    { PE_SELBAD,    "PE_SELBAD",    "invalid selection",        ""                  },
    { PE_PICKBAD,   "PE_PICKBAD",   "invalid selection",        "%c"                },
    { PE_PICKDUP,   "PE_PICKDUP",   "duplicate selection",      "%c"                },
    { PE_PICKMORE,  "PE_PICKMORE",  "too few selections",       ""                  },
    { PE_PICKLESS,  "PE_PICKLESS",  "too many selections",      ""                  },
    { PE_CHARMORE,  "PE_CHARMORE",  "too few characters",       ""                  },
    { PE_CHARLESS,  "PE_CHARLESS",  "too many characters",      ""                  },
    { PE_LIST,      "PE_LIST",      "list",                     "item %d: %s: %s"   },
    { PE_LISTLESS,  "PE_LISTLESS",  "too many items",           ""                  }
};

void perClear()
{
    penMutable = PE_OK;
    pemMutable[0] = 0;
}

void parse(const char* s, QtyNum qtynum, bool& val)
{
    int     c;
    Int64   v;

    perClear();

    if ( s == 0     )   { per(PE_NULPTR);   return; }
    if ( strlenz(s) )   { per(PE_NUL);      return; }

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);
    const QtyDef& def = qtyDefs[qtynum];

    ASSERT(def.fmt == QF_FLG);
    ASSERT(!strlenz(def.one) && !strlenz(def.alt));

    // single character values are parsed faster
    // standard symbols (0 and 1) get priority

    if ( strlen(s) == 1 )
    {
        c = *s;

        if ( c == '1' ) { val = true;  return; }
        if ( c == '0' ) { val = false; return; }

        if ( c == *def.one ) { val = true;  return; }
        if ( c == *def.alt ) { val = false; return; }
    }
    else
    {
        if ( strcmp(def.one, s) == 0 ) { val = true;  return; }
        if ( strcmp(def.alt, s) == 0 ) { val = false; return; }
    }

    // for completeness, provide more info about error

    parse(s, QN_DEC, v);
    if ( !pen ) per(PE_RANGE); else per(PE_FMT);
}

void parse(const char* s, QtyNum qtynum, double& val)
{
    Size    i, n;
    int     c;
    double  v;
    bool    inNew;  // in new section of string (mantissa or exponent)
    bool    inExp;  // in exponent section of string
    bool    inFrc;  // in fraction part of current section
    bool    inDig;  // in digit span within integer or fractional part

    perClear();

    ASSERT_ALWAYS(qtynum >= 0 && qtynum < QN_COUNT); // future use

    if ( s == 0 )               { per(PE_NULPTR);   return; }
    if ( (n = strlen(s)) == 0 ) { per(PE_NUL);      return; }

    errno = 0;
    inNew = true;
    inExp = false;
    inFrc = false;
    inDig = false;

    for ( i = 0; i < n; i++ )
    {
        c = s[i];

        if ( inNew )
        {
            if ( c >= '0' && c <= '9' )
            {
                inDig = true;
            }
            else
            {
                if ( c != '+' && c != '-' ) { per(PE_FMT); return; }
            }

            inNew = false;
        }
        else // !isNew
        {
            if ( c == '.' )
            {
                if ( inExp || inFrc || !inDig ) { per(PE_FMT); return; }

                inFrc = true;
                inDig = false;
            }
            else if ( c == 'e' || c == 'E' )
            {
                if ( inExp || !inDig ) { per(PE_FMT); return; }

                inNew = true;
                inExp = true;
                inFrc = false;
                inDig = false;
            }
            else if ( c >= '0' && c <= '9' )
            {
                inDig = true;
            }
            else
            {
                per(PE_FMT); return;
            }
        }
    }

    v = strtod(s, 0);

    switch ( errno )
    {
        case 0:         break;
        case ERANGE:    per(PE_RANGE);    return;
        default:        per(PE_FMT);      return;
    }

    val = v;
    ASSERT(!pen);
}

void parse(const char* s, QtyNum qtynum, Uint64& val)
{
    Size    i, n;
    char    t[PARSE_MAX+1];
    int     c;
    char*   e;
    Uint64  v;
    Uint64  mul = 1;

    perClear();

    ASSERT_ALWAYS(qtynum >= 0 && qtynum < QN_COUNT); // future use

    if ( s == 0 )               { per(PE_NULPTR);   return; }
    if ( (n = strlen(s)) == 0 ) { per(PE_NUL);      return; }

    strncpyz(t, s, PARSE_MAX);

    errno = 0;

    if ( n > 2 && t[1] == 'x' )
    {
        if ( t[0] != '0' ) { per(PE_FMT); return; }

        for (i = 2; i < n; i++)
        {
            c = t[i];

            if ( c < '0' || (c > '9' && (c < 'a' || c > 'f')) )
            {
                per(PE_FMT); return;
            }
        }

        v = strtoUint64(t + 2, 0, 16);
        goto chk_err;
    }

    // decimal

    for (i = 0; i < n; i++)
    {
        c = t[i];

        if ( c < '0' || c > '9' )
        {
            if ( c == 'e' )
            {
                if ( i < 1 ) { per(PE_FMT); return; }

                e = t + i + 1;
                if ( !strIsDec(e) ) { per(PE_FMT); return; }

                v = strtoUint64(e, 0, 10);
                if ( errno  ) goto chk_err;
                if ( v > 19 ) { per(PE_RANGE); return; }

                mul = pwr10(v);
            }
            else if ( i == n - 2 && t[i+1] == 'i' )
            {
                switch (c)
                {
                    case 'K': mul = QTY_KI; break;
                    case 'M': mul = QTY_MI; break;
                    case 'G': mul = QTY_GI; break;
                    case 'T': mul = QTY_TI; break;
                    case 'P': mul = QTY_PI; break;
                    case 'E': mul = QTY_EI; break;
                    default:  per(PE_FMT); return;
                }
            }
            else if ( i == n - 1 )
            {
                switch (c)
                {
                    case 'k': mul = QTY_K; break;
                    case 'M': mul = QTY_M; break;
                    case 'G': mul = QTY_G; break;
                    case 'T': mul = QTY_T; break;
                    case 'P': mul = QTY_P; break;
                    case 'E': mul = QTY_E; break;
                    default:  per(PE_FMT); return;
                }
            }
            else
            {
                per(PE_FMT); return;
            }

            t[i] = 0;
            break;
        }
    }

    v = strtoUint64(t, 0, 10);

chk_err:
    switch ( errno )
    {
        case 0:
            if ( v > UINT64_VAL_MAX / mul ) { per(PE_RANGE); return; }
            break;

        case ERANGE:
            per(PE_RANGE); return;

        default:
            per(PE_FMT); return;
    }

    val = mul * v;
    ASSERT(!pen);
}

void parse(const char* s, QtyNum qtynum, Int64& val)
{
    char    c;
    bool    neg;
    Uint64  v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    c = *s;

    if ( c == '-' )
    {
        neg = true;
        s++;
    }
    else
    {
        neg = false;
        if ( c == '+' ) s++;
    }

    parse(s, qtynum, v);

    if ( pen ) return;

    if ( neg )
    {
        if ( v > (Uint64) INT64_VAL_MAX + 1 )   { per(PE_RANGE); return; }
        val = - (Int64) v;
    }
    else
    {
        if ( v > (Uint64) INT64_VAL_MAX )       { per(PE_RANGE); return; }
        val = + (Int64) v;
    }

    ASSERT(!pen);
}

void parse(const char* s, QtyNum qtynum, Uint32& val)
{
    Uint64 v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    parse(s, qtynum, v);

    if ( pen ) return;
    if ( v > UINT32_VAL_MAX ) { per(PE_RANGE); return; }

    val = Uint32(v);
    ASSERT(!pen);
}

void parse(const char* s, QtyNum qtynum, Int32& val)
{
    Int64 v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    parse(s, qtynum, v);

    if ( pen ) return;
    if ( v < INT32_VAL_MIN || v > INT32_VAL_MAX ) { per(PE_RANGE); return; }

    val = Int32(v);
    ASSERT(!pen);
}

void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, bool val[], Size& count)
{
    Size    i;
    char    a[PARSE_MAX+1];
    char*   v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    count = 0;

    if ( s == 0     ) { per(PE_NULPTR); return; }
    if ( strlenz(s) ) { per(PE_NUL);    return; }

    strncpyz(a, s, PARSE_MAX);

    v = strtok (a, dlm);

    for ( i = 0; i < max && v != 0; i++ )
    {
        parse(v, qtynum, val[i]);

        if ( pen )
        {
            per(PE_LIST, i + 1, v, pem);
            count = i;
            return;
        }

        v = strtok (0, dlm);
    }

    ASSERT(!pen);

    count = i;
    if ( i > max ) per(PE_LISTLESS);
}

void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, double val[], Size& count)
{
    Size    i;
    char    a[PARSE_MAX+1];
    char*   v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    count = 0;

    if ( s == 0     ) { per(PE_NULPTR); return; }
    if ( strlenz(s) ) { per(PE_NUL);    return; }

    strncpyz(a, s, PARSE_MAX);

    v = strtok (a, dlm);

    for ( i = 0; i < max && v != 0; i++ )
    {
        parse(v, qtynum, val[i]);

        if ( pen )
        {
            per(PE_LIST, i + 1, v, pem);
            count = i;
            return;
        }

        v = strtok (0, dlm);
    }

    ASSERT(!pen);

    count = i;
    if ( i > max ) per(PE_LISTLESS);
}

void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Uint64 val[], Size& count)
{
    Size    i;
    char    a[PARSE_MAX+1];
    char*   v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    count = 0;

    if ( s == 0     ) { per(PE_NULPTR); return; }
    if ( strlenz(s) ) { per(PE_NUL);    return; }

    strncpyz(a, s, PARSE_MAX);

    v = strtok (a, dlm);

    for ( i = 0; i < max && v != 0; i++ )
    {
        parse(v, qtynum, val[i]);

        if ( pen )
        {
            per(PE_LIST, i + 1, v, pem);
            count = i;
            return;
        }

        v = strtok (0, dlm);
    }

    ASSERT(!pen);

    count = i;
    if ( i > max ) per(PE_LISTLESS);
}

void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Int64 val[], Size& count)
{
    Size    i;
    char    a[PARSE_MAX+1];
    char*   v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    count = 0;

    if ( s == 0     ) { per(PE_NULPTR); return; }
    if ( strlenz(s) ) { per(PE_NUL);    return; }

    strncpyz(a, s, PARSE_MAX);

    v = strtok (a, dlm);

    for ( i = 0; i < max && v != 0; i++ )
    {
        parse(v, qtynum, val[i]);

        if ( pen )
        {
            per(PE_LIST, i + 1, v, pem);
            count = i;
            return;
        }

        v = strtok (0, dlm);
    }

    ASSERT(!pen);

    count = i;
    if ( i > max ) per(PE_LISTLESS);
}

void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Uint32 val[], Size& count)
{
    Size    i;
    char    a[PARSE_MAX+1];
    char*   v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    count = 0;

    if ( s == 0     ) { per(PE_NULPTR); return; }
    if ( strlenz(s) ) { per(PE_NUL);    return; }

    strncpyz(a, s, PARSE_MAX);

    v = strtok (a, dlm);

    for ( i = 0; i < max && v != 0; i++ )
    {
        parse(v, qtynum, val[i]);

        if ( pen )
        {
            per(PE_LIST, i + 1, v, pem);
            count = i;
            return;
        }

        v = strtok (0, dlm);
    }

    ASSERT(!pen);

    count = i;
    if ( i > max ) per(PE_LISTLESS);
}

void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Int32 val[], Size& count)
{
    Size    i;
    char    a[PARSE_MAX+1];
    char*   v;

    perClear();

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    count = 0;

    if ( s == 0     ) { per(PE_NULPTR); return; }
    if ( strlenz(s) ) { per(PE_NUL);    return; }

    strncpyz(a, s, PARSE_MAX);

    v = strtok (a, dlm);

    for ( i = 0; i < max && v != 0; i++ )
    {
        parse(v, qtynum, val[i]);

        if ( pen )
        {
            per(PE_LIST, i + 1, v, pem);
            count = i;
            return;
        }

        v = strtok (0, dlm);
    }

    ASSERT(!pen);

    count = i;
    if ( i > max ) per(PE_LISTLESS);
}

void parseFlag(const char* s, const Var* var, Flag& val)
{
    Flag tmpval;

    perClear();

    ASSERT_ALWAYS(var->typnum == TYP_FLAG);
    ASSERT(var->qtynum >= 0 && var->qtynum < QN_COUNT);

    ASSERT(strlenz(var->min));
    ASSERT(strlenz(var->max));
    ASSERT(strlenz(var->sel));

    parse(s, var->qtynum, tmpval);

    if ( pen ) return;
    val = tmpval;
}

void parseText(const char* s, const Var* var, Text& val)
{
    Size n;
    Size minval = 0;
    Size maxval = 0;

    perClear();

    ASSERT(var->typnum == TYP_TEXT);
    ASSERT(var->qtynum >= 0 && var->qtynum < QN_COUNT);

    n = strlen(s);

    if ( !strlenz(var->min) )
    {
        parse(var->min, QN_DEC, minval);
        ASSERT_ALWAYS(!pen);

        if ( n < minval ) { per(PE_CHARMORE); return; }
    }

    if ( !strlenz(var->max) )
    {
        parse(var->max, QN_DEC, maxval);
        ASSERT_ALWAYS(!pen);

        if ( n > maxval ) { per(PE_CHARLESS); return; }
    }

    val = s;
    ASSERT(!pen);
}

void parseInum(const char* s, const Var* var, Inum& val)
{
    Inum tmpval;
    Inum selvals[VAR_MAX_SELS];
    Size selCount;
    Inum minval;
    Inum maxval;

    perClear();

    ASSERT(var->typnum == TYP_INUM);
    ASSERT(var->qtynum >= 0 && var->qtynum < QN_COUNT);

    parse(s, var->qtynum, tmpval);

    if ( pen ) return;

    if ( !strlenz(var->sel) )
    {
        parseList(var->sel, var->qtynum, "|", VAR_MAX_SELS, selvals, selCount);
        ASSERT(!pen);

        for (Size i = 0; i < selCount; i++ )
        {
            if ( tmpval == selvals[i] )
            {
                val = tmpval;
                ASSERT(!pen);
                return;
            }
        }

        if ( strlenz(var->min) && strlenz(var->max) )
        {
            per(PE_SELBAD); return;
        }
    }

    if ( !strlenz(var->min) )
    {
        parse(var->min, var->qtynum, minval);
        ASSERT(!pen);

        if ( tmpval < minval ) { per(PE_RANGE); return; }
    }

    if ( !strlenz(var->max) )
    {
        parse(var->max, var->qtynum, maxval);
        ASSERT(!pen);

        if ( tmpval > maxval ) { per(PE_RANGE); return; }
    }

    val = tmpval;
    ASSERT(!pen);
}

void parseFnum(const char* s, const Var* var, Fnum& val)
{
    Fnum tmpval;
    Fnum selvals[VAR_MAX_SELS];
    Size selCount;
    Fnum minval;
    Fnum maxval;

    perClear();

    ASSERT(var->typnum == TYP_FNUM);
    ASSERT(var->qtynum >= 0 && var->qtynum < QN_COUNT);

    parse(s, var->qtynum, tmpval);

    if ( pen ) return;

    if ( !strlenz(var->sel) )
    {
        parseList(var->sel, var->qtynum, "|", VAR_MAX_SELS, selvals, selCount);
        ASSERT(!pen);

        for (Size i = 0; i < selCount; i++ )
        {
            if ( tmpval == selvals[i] )
            {
                val = tmpval;
                ASSERT(!pen);
                return;
            }
        }

        if ( strlenz(var->min) && strlenz(var->max) )
        {
            per(PE_SELBAD); return;
        }
    }

    if ( !strlenz(var->min) )
    {
        parse(var->min, var->qtynum, minval);
        ASSERT(!pen);

        if ( tmpval < minval ) { per(PE_RANGE); return; }
    }

    if ( !strlenz(var->max) )
    {
        parse(var->max, var->qtynum, maxval);
        ASSERT(!pen);

        if ( tmpval > maxval ) { per(PE_RANGE); return; }
    }

    val = tmpval;
    ASSERT(!pen);
}

void parsePick(const char* s, const Var* var, Pick& val)
{
    Size        n, t;
    const char* c;
    const char* x;
    bool        v;
    Pick        tmpval;
    Size        minval = 0;
    Size        maxval = 0;

    perClear();

    ASSERT(var->typnum == TYP_PICK);
    ASSERT(var->qtynum >= 0 && var->qtynum < QN_COUNT);

    n = strlen(s);
    t = 0;

    if ( n > 0 )
    {
        if ( s[0] != '*' )
        {
            ASSERT(tmpval.all == 0);
            v = 1;
            c = s;
            t = n;
        }
        else
        {
            tmpval.set(1, var->sel);
            v = 0;

            if ( n == 1)
            {
                c = s + 1;
                t = strlen(var->sel);
            }
            else
            {
                if ( s[1] != '-' || n < 3 ) { per(PE_WILDBAD); return; }

                c = s + 2;
                t = strlen(var->sel) - n + 2;
            }
        }

        if ( !strlenz(c) )
        {
            x = findOddChar(c, var->sel);
            if ( x != 0) { per(PE_PICKBAD, *x); return; }

            x = findDupChar(c);
            if ( x != 0) { per(PE_PICKDUP, *x); return; }

            tmpval.set(v, c);
        }
    }

    if ( !strlenz(var->min) )
    {
        parse(var->min, QN_DEC, minval);
        ASSERT_ALWAYS(!pen);

        if ( t < minval ) { per(PE_PICKMORE); return; }
    }

    if ( !strlenz(var->max) )
    {
        parse(var->max, QN_DEC, maxval);
        ASSERT_ALWAYS(!pen);

        if ( t > maxval ) { per(PE_PICKLESS); return; }
    }

    val = tmpval;
    ASSERT(!pen);
}

static void per(Pen e, ...)
{
    char    fmt[PEM_MAX+1];
    char    msg[PEM_MAX+1];
    PerDef  def;
    va_list args;

    // intermediate msg buffer allows
    // recursive error parameters
    // e.g. per(PE_LIST, ..., pem)

    fmt[0] = 0;

    va_start (args, e);

    ASSERT(e > 0 && e < PE_COUNT);

    def = perDefs[e];
    ASSERT(def.num == e);

    if ( !strlenz(def.params) )
    {
        strncpyz(fmt, def.msg, PEM_MAX);
        strncatz(fmt, ": ", PEM_MAX);
        strncatz(fmt, def.params, PEM_MAX);
        vsnprintfz(msg, PEM_MAX, fmt, args);

        strncpyz(pemMutable, msg, PEM_MAX);
    }
    else
    {
        strncpyz(pemMutable, def.msg, PEM_MAX);
    }

    va_end (args);
    penMutable = e;
}

// EOF