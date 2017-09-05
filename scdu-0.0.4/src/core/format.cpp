// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>
#include <math.h>

#include "core.h"

static int formatReg(char* s, Size max, FmtSpec spec, Int64 val);
static int formatReg(char* s, Size max, FmtSpec spec, double val);

static int formatTEA(char* s, Size max, FmtSpec spec, const char* val);
static int formatTEU(char* s, Size max, FmtSpec spec, const char* val);
static int formatPAA(char* s, Size max, FmtSpec spec, const char* val);
static int formatPAU(char* s, Size max, FmtSpec spec, const char* val);
static int formatURL(char* s, Size max, FmtSpec spec, const char* val);
static int formatPSW(char* s, Size max, FmtSpec spec, const char* val);
static int formatHEX(char* s, Size max, FmtSpec spec, Int64 val);
static int formatHMS(char* s, Size max, FmtSpec spec, Int64 val);
static int formatP5V(char* s, Size max, FmtSpec spec, double val);
static int formatDEC(char* s, Size max, FmtSpec spec, Int64 val);
static int formatDCS(char* s, Size max, FmtSpec spec, Int64 val);
static int formatD12(char* s, Size max, FmtSpec spec, double val);
static int formatD5V(char* s, Size max, FmtSpec spec, double val);
static int formatE33(char* s, Size max, FmtSpec spec, double val);
static int formatUPD(char* s, Size max, FmtSpec spec, double val);
static int formatUPB(char* s, Size max, FmtSpec spec, double val);

int format(char* s, Size max, FmtSpec spec, QtyNum qtynum, bool val)
{
    const char*     fmt;
    const char*     ts;
    const char*     fs;
    const char*     xs;
    int             tn, fn, xn;
    int             w;

    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);
    const QtyDef& def = qtyDefs[qtynum];

    ASSERT(def.fmt == QF_FLG);

    if ( spec & FSM_FRIENDLY )
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_PR:     fmt = "%-*s";
            default:        fmt = "%+*s";
        }

        ts = def.one;
        fs = def.alt;

        tn = strleni(def.one);
        fn = strleni(def.alt);

        xs = val ? ts : fs;
        xn = spec & FSM_PADDING ? tn > fn ? tn : fn : strleni(xs);

        if ( int(max) < xn ) return -1;

        w = snprintfz(s, max, fmt, xn, xs);
        ASSERT(w > 0);
        return w;
    }

    if ( max < 1 ) return -1;
    *s++ = val ? '1' : '0';
    *s = 0;
    return 1;
}

int format(char *s, Size max, FmtSpec spec, QtyNum qtynum, const char* val)
{
    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    const QtyDef&   def = qtyDefs[qtynum];
    QtyFmt          f = def.fmt;

    if ( f == QF_TEX ) f = cmd.options.asciiText ? QF_TEA : QF_TEU;
    if ( f == QF_PAX ) f = cmd.options.asciiPath ? QF_PAA : QF_PAU;

    switch ( f )
    {
        case QF_TEA: return formatTEA(s, max, spec, val);
        case QF_TEU: return formatTEU(s, max, spec, val);
        case QF_PAA: return formatPAA(s, max, spec, val);
        case QF_PAU: return formatPAU(s, max, spec, val);
        case QF_URL: return formatURL(s, max, spec, val);
        case QF_PSW: return formatPSW(s, max, spec, val);

        default: ASSERT(false); return -1;
    }
}

int format(char *s, Size max, FmtSpec spec, QtyNum qtynum, Int64 val)
{
    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    const QtyDef&   def = qtyDefs[qtynum];
    QtyFmt          f = def.fmt;
    int             wa, wu, wr;
    const char*     us;
    int             un;

    if ( f == QF_UPX ) f = cmd.options.binaryPrefix ? QF_UPB : QF_UPD;

    if ( spec & FSM_FRIENDLY )
    {
        if ( strlenz(def.one) )
        {
            ASSERT(strlenz(def.alt));
            spec = FmtSpec(spec & ~FSM_FRIENDLY);
        }
    }
    else
    {
        if ( strlenz(def.sym) )
        {
            spec = FmtSpec(spec | FSM_FRIENDLY);
        }
    }

    wa = (int) max;

    if ( spec & FSM_REG )
    {
        wu = formatReg(s, max, spec, val);
    }
    else
    {
        switch ( f )
        {
            case QF_DEC: wu = formatDEC(s, max, spec, val);             break;
            case QF_DCS: wu = formatDCS(s, max, spec, val);             break;
            case QF_D12: wu = formatD12(s, max, spec, (double) val);    break;
            case QF_D5V: wu = formatD5V(s, max, spec, (double) val);    break;
            case QF_P5V: wu = formatP5V(s, max, spec, (double) val);    break;
            case QF_HEX: wu = formatHEX(s, max, spec, val);             break;
            case QF_E33: wu = formatE33(s, max, spec, (double) val);    break;
            case QF_UPD: wu = formatUPD(s, max, spec, (double) val);    break;
            case QF_UPB: wu = formatUPB(s, max, spec, (double) val);    break;
            case QF_HMS: wu = formatHMS(s, max, spec, val);             break;
            default: ASSERT(false); return -1;
        }
    }

    if ( wu < 0 || spec & FSM_BARE ) return wu;
    wr = wa - wu;

    if ( spec & FSM_FRIENDLY )
    {
        us = absv(val) == 1 ? def.one: def.alt;
    }
    else
    {
        us = def.sym;
    }

    un = strleni(us);

    if ( un == 0 )      return wu;
    if ( un > wr - 1 )  return -1;

    if ( (f == QF_UPD || f == QF_UPB) && !(spec & FSM_REG) )
    {
        if ( spec & FSM_FRIENDLY )
        {
            strcpy(s + wu, "-");
            wu++;
            wr--;
        }
    }
    else
    {
        strcpy(s + wu, " ");
        wu++;
        wr--;
    }

    strcpy(s + wu, us);
    wu += un;

    return wu;
}

int format(char *s, Size max, FmtSpec spec, QtyNum qtynum, double val)
{
    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    const QtyDef&   def = qtyDefs[qtynum];
    QtyFmt          f = def.fmt;
    int             wa, wu, wr;
    const char*     us;
    int             un;

    if ( f == QF_UPX ) f = cmd.options.binaryPrefix ? QF_UPB : QF_UPD;

    if ( spec & FSM_FRIENDLY )
    {
        if ( strlenz(def.one) )
        {
            ASSERT(strlenz(def.alt));
            spec = FmtSpec(spec & ~FSM_FRIENDLY);
        }
    }
    else
    {
        if ( strlenz(def.sym) )
        {
            spec = FmtSpec(spec | FSM_FRIENDLY);
        }
    }

    wa = (int) max;

    if ( spec & FSM_REG )
    {
        wu = formatReg(s, max, spec, val);
    }
    else
    {
        switch ( f )
        {
            case QF_DEC: wu = formatDEC(s, max, spec, roundi(val));     break;
            case QF_DCS: wu = formatDCS(s, max, spec, roundi(val));     break;
            case QF_D12: wu = formatD12(s, max, spec, val);             break;
            case QF_D5V: wu = formatD5V(s, max, spec, val);             break;
            case QF_P5V: wu = formatP5V(s, max, spec, val);             break;
            case QF_HEX: wu = formatHEX(s, max, spec, roundi(val));     break;
            case QF_E33: wu = formatE33(s, max, spec, val);             break;
            case QF_UPD: wu = formatUPD(s, max, spec, val);             break;
            case QF_UPB: wu = formatUPB(s, max, spec, val);             break;
            case QF_HMS: wu = formatHMS(s, max, spec, roundi(val));     break;
            default: ASSERT(false); return -1;
        }
    }

    if ( wu < 0 || spec & FSM_BARE ) return wu;
    wr = wa - wu;

    if ( spec & FSM_FRIENDLY )
    {
        us = absv(val) == 1 ? def.one: def.alt;
    }
    else
    {
        us = def.sym;
    }

    un = strleni(us);

    if ( un == 0 ) return wu;
    if ( un > wr - 1 ) return -1;

    if ( un == 0 )      return wu;
    if ( un > wr - 1 )  return -1;

    if ( (f == QF_UPD || f == QF_UPB) && !(spec & FSM_REG) )
    {
        if ( spec & FSM_FRIENDLY )
        {
            strcpy(s + wu, "-");
            wu++;
            wr--;
        }
    }
    else
    {
        strcpy(s + wu, " ");
        wu++;
        wr--;
    }

    strcpy(s + wu, us);
    wu += un;

    return wu;
}

int formatRate(char* s, Size max, FmtSpec spec, QtyNum qtynum, double val)
{
    ASSERT(qtynum >= 0 && qtynum < QN_COUNT);

    const QtyDef&   def = qtyDefs[qtynum];
    QtyRate         qr  = def.rate;
    const char*     u   = 0;
    int             w   = 0;

    ASSERT(qr != QR_NONE);

    if ( qr == QR_ANY )
    {
        if      ( cmd.options.rateMetric.S ) qr = QR_SEC;
        else if ( cmd.options.rateMetric.M ) qr = QR_MIN;
        else if ( cmd.options.rateMetric.H ) qr = QR_HOUR;
    }

    switch ( qr )
    {
    case QR_SEC:
        u = "/s";
        break;

    case QR_MIN:
        val *= 60.0;
        u = "/m";
        break;

    case QR_HOUR:
        val *= 3600.0;
        u = "/h";
        break;

    default:
        ASSERT(false);
    }

    w = format(s, max - 2, spec, qtynum, val);

    if ( w < 0 ) return -1;

    strcpy(s + w, u);
    w += strleni(u);

    return w;
}

int formatFlag(char* s, Size max, FmtSpec spec, const Var* var, const Flag& flag)
{
    ASSERT(var->typnum == TYP_FLAG);
    return format(s, max, spec, var->qtynum, flag);
}

int formatText(char* s, Size max, FmtSpec spec, const Var* var, const Text& text)
{
    ASSERT(var->typnum == TYP_TEXT);
    return format(s, max, spec, var->qtynum, text.cb());
}

int formatInum(char* s, Size max, FmtSpec spec, const Var* var, const Inum& inum)
{
    ASSERT(var->typnum == TYP_INUM);
    return format(s, max, spec, var->qtynum, inum);
}

int formatFnum(char* s, Size max, FmtSpec spec, const Var* var, const Fnum& fnum)
{
    ASSERT(var->typnum == TYP_FNUM);
    return format(s, max, spec, var->qtynum, fnum);
}

int formatPick(char* s, Size max, FmtSpec spec, const Var* var, const Pick& pick)
{
    ASSERT(var->typnum == TYP_PICK);

    const char* sel = var->sel;
    Size        m   = strlen(sel);
    char        c;
    int         w, g;

    ASSERT(m > 0);

    w = 0;

    if ( pick.isNull() )
    {
        if ( spec & FSM_FRIENDLY )
        {
            w = 6;
            if ( int(max) < w ) return -1;
            memcpy(s, "<null>", 6);
        }
    }
    else
    {
        if ( max < m ) return -1;

        while ( (c = *sel++) != 0 )
        {
            g = pick.get(c);

            if      (g == 1)    s[w++] = c;
            else if (g != 0)    s[w++] = '?';
        }
    }

    s[w] = 0;

    return w;
}

static int formatReg(char* s, Size max, FmtSpec spec, Int64 val)
{
    const char* fmt;
    Int64 aval;

    aval = absv(val);

    if ( val < 0 )
    {
        fmt = "-%d";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:  fmt = "%d";  break;
            case FS_PL:    fmt = " %d"; break;
            case FS_PR:    fmt = "%d "; break;
            case FS_PZ:    fmt = "0%d"; break;
            case FS_PP:    fmt = "+%d"; break;
            default: ASSERT(false); return -1;
        }
    }

    return snprintfz(s, max, fmt, aval);
}

static int formatReg(char* s, Size max, FmtSpec spec, double val)
{
    const char* fmt;
    double aval;

    aval = absv(val);

    if ( val < 0 )
    {
        fmt = "-%f";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:  fmt = "%f";  break;
            case FS_PL:    fmt = " %f"; break;
            case FS_PR:    fmt = "%f "; break;
            case FS_PZ:    fmt = "0%f"; break;
            case FS_PP:    fmt = "+%f"; break;
            default: ASSERT(false); return -1;
        }
    }

    return snprintfz(s, max, fmt, aval);
}

static int formatTEA(char* s, Size max, FmtSpec spec, const char* val)
{
    Size w;

    if ( val == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    w = strlen(val);

    if ( w == 0 && (spec & FSM_FRIENDLY) )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<null>");
        return 6;
    }

    if ( max < w ) return -1;

    strcpy(s, val);
    return (int) w;
}

static int formatTEU(char* s, Size max, FmtSpec spec, const char* val)
{
   Size w;

    if ( val == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    w = strlen(val);

    if ( w == 0 && (spec & FSM_FRIENDLY) )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<null>");
        return 6;
    }

    if ( max < w ) return -1;

    strcpy(s, val);
    return (int) w;
}

static int formatPAA(char* s, Size max, FmtSpec spec, const char* val)
{
   Size w;

    if ( val == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    w = strlen(val);

    if ( w == 0 && (spec & FSM_FRIENDLY) )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<null>");
        return 6;
    }

    if ( max < w ) return -1;

    strcpy(s, val);
    return (int) w;
}

static int formatPAU(char* s, Size max, FmtSpec spec, const char* val)
{
   Size w;

    if ( val == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    w = strlen(val);

    if ( w == 0 && (spec & FSM_FRIENDLY) )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<null>");
        return 6;
    }

    if ( max < w ) return -1;

    strcpy(s, val);
    return (int) w;
}

static int formatURL(char* s, Size max, FmtSpec spec, const char* val)
{
   Size w;

    if ( val == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    w = strlen(val);

    if ( w == 0 && (spec & FSM_FRIENDLY) )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<null>");
        return 6;
    }

    if ( max < w ) return -1;

    strcpy(s, val);
    return (int) w;
}

static int formatPSW(char* s, Size max, FmtSpec spec, const char* val)
{
   Size w;

    if ( val == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    w = strlen(val);

    if ( w == 0 && (spec & FSM_FRIENDLY) )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<null>");
        return 6;
    }

    if ( max < w ) return -1;

    strcpy(s, val);
    return (int) w;
}

static int formatHEX(char* s, Size max, FmtSpec spec, Int64 val)
{
    Size        rqd;
    int         w;
    const char* fmt;

    rqd = 18;
    if ( max < rqd ) return -1;

    switch ( spec & FSM_PADDING )
    {
        case FS_AUTO: fmt = "0x" F64x();        break;
        case FS_PZ:   fmt = "0x" F64x(016);     break;

        default: ASSERT(false); return -1;
    }

    w = snprintfz(s, max, fmt, val);
    ASSERT(w > 0);
    return w;
}

static int formatHMS(char* s, Size max, FmtSpec spec, Int64 val)
{
    int             rqd, w;
    Int64           aval;
    int             hour, min, sec;
    const char*     fmt;

    rqd = spec & FSM_PADDING || val < 0 ? 9 : 8;
    if ( max < (Size) rqd ) return -1;

    if ( val <= -360000 || val >= 360000 )
    {
        if ( val < 0 )
        {
            fmt = "---:--:--";
        }
        else
        {
            switch ( spec & FSM_PADDING )
            {
                case FS_AUTO:   fmt = "++:++:++";         break;
                case FS_PL:     fmt = " ++:++:++";        break;
                case FS_PR:     fmt = "++:++:++ ";        break;
                case FS_PZ:
                case FS_PP:     fmt = "+++:++:++";        break;

                default: ASSERT(false); return -1;
            }
        }

        strcpy(s, fmt);
        return -1;
    }

    aval = absv(val);

    sec  = (int) aval % 60; aval /= 60;
    min  = (int) aval % 60; aval /= 60;
    hour = (int) aval;

    ASSERT(hour < 100);

    if ( val < 0 )
    {
        fmt = "-%02u:%02u:%02u";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:   fmt = "%02u:%02u:%02u";         break;
            case FS_PL:     fmt = " %02u:%02u:%02u";        break;
            case FS_PR:     fmt = "%02u:%02u:%02u ";        break;
            case FS_PZ:     fmt = "0%02u:%02u:%02u";        break;
            case FS_PP:     fmt = "+%02u:%02u:%02u";        break;

            default: ASSERT(false); return -1;
        }
    }

    w = snprintfz(s, max, fmt, hour, min, sec);
    ASSERT(w == rqd);
    return w;
}

static int formatP5V(char* s, Size max, FmtSpec spec, double val)
{
    Size        rqd;
    const char* fmt;
    double      aval;
    int         w;

    rqd = 6;
    if ( spec & FSM_PADDING || val < 0 ) rqd++;
    if ( max < rqd ) return -1;

    if ( val <= -9.99950 || val >= 9.99950 )
    {
        if ( val < 0 )
        {
            fmt = "------%";
        }
        else
        {
            switch ( spec & FSM_PADDING )
            {
                case FS_AUTO:   fmt =  "+++++%%"; break;
                case FS_PL:     fmt = " +++++%%"; break;
                case FS_PR:     fmt = "+++++%% "; break;
                case FS_PZ:
                case FS_PP:     fmt = "++++++%%"; break;

                default: ASSERT(false); return -1;
            }
        }

        strcpy(s, fmt);
        return -1;
    }

    val *= 100.0;
    aval = absv(val);

    if ( val < 0 )
    {
        if      ( aval >= 99.9950 ) fmt = "-%05.1f%%";
        else if ( aval >=  9.9950 ) fmt = "-%05.2f%%";
        else                        fmt = "-%05.3f%%";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:
                if      ( aval >= 99.9950 ) fmt = "%05.1f%%";
                else if ( aval >=  9.9950 ) fmt = "%05.2f%%";
                else                        fmt = "%05.3f%%";
                break;

            case FS_PL:
                if      ( aval >= 99.9950 ) fmt = " %05.1f%%";
                else if ( aval >=  9.9950 ) fmt = " %05.2f%%";
                else                        fmt = " %05.3f%%";
                break;

            case FS_PR:
                if      ( aval >= 99.9950 ) fmt = "%05.1f%% ";
                else if ( aval >=  9.9950 ) fmt = "%05.2f%% ";
                else                        fmt = "%05.3f%% ";
                break;

            case FS_PZ:
                if      ( aval >= 99.9950 ) fmt = "0%05.1f%%";
                else if ( aval >=  9.9950 ) fmt = "0%05.2f%%";
                else                        fmt = "0%05.3f%%";
                break;

            case FS_PP:
                if      ( aval >= 99.9950 ) fmt = "+%05.1f%%";
                else if ( aval >=  9.9950 ) fmt = "+%05.2f%%";
                else                        fmt = "+%05.3f%%";
                break;

            default:
                ASSERT(false);
                return -1;
        }
    }

    w = snprintfz(s, max, fmt, aval);

    ASSERT(w == int(rqd));
    return w;
}

static int formatDEC(char* s, Size max, FmtSpec spec, Int64 val)
{
    const char* fmt;
    Int64 aval;

    aval = absv(val);

    if ( val < 0 )
    {
        fmt = "-%d";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:  fmt = "%d";  break;
            case FS_PL:    fmt = " %d"; break;
            case FS_PR:    fmt = "%d "; break;
            case FS_PZ:    fmt = "0%d"; break;
            case FS_PP:    fmt = "+%d"; break;
            default: ASSERT(false); return -1;
        }
    }

    return snprintfz(s, max, fmt, aval);
}

static int formatDCS(char* s, Size max, FmtSpec spec, Int64 val)
{
    Size        rqd;
    const char* fmt;
    int         wa, wu, wr, w;
    Int64       aval;

    rqd = INT64_CSD_MAX - 1;
    if ( spec & FSM_PADDING || val < 0 ) rqd++;
    if ( max < rqd ) return -1;

    aval = absv(val);

    wa = (int) max;
    wu = 0;
    wr = wa;

    if ( spec & FSM_PADDING || val < 0 )
    {
        if ( val < 0)
        {
            fmt = "-";
        }
        else
        {
            switch ( spec & FSM_PADDING )
            {
                case FS_PL: fmt = " ";  break;
                case FS_PP: fmt = "+";  break;
                default:    fmt = "";   break;
            }
        }

        if ( !strlenz(fmt) )
        {
            strcpy(s + wu, fmt);
            wu++;
            wr--;
        }

        w = formatDCS(s + wu, wr, FS_AUTO, aval);
        ASSERT(w > 0);
        wu += w;
        wr -= w;

        if ( (spec & FSM_PADDING) == FS_PR )
        {
            fmt = " ";
            strcpy(s + wu, fmt);
            wu++;
            wr--;
        }

        return wu;
    }

    ASSERT((spec & FSM_PADDING) == FS_AUTO && val >= 0);

    if ( val < 1000 )
    {
        w = snprintfz(s + wu, wr, "%d", (int) val);
        ASSERT(w > 0);
        wu += w;
        wr -= w;
        return wu;
    }

    w = formatDCS(s + wu, wr, FS_AUTO, val / 1000);
    ASSERT(w > 0);
    wu += w;
    wr -= w;

    ASSERT(wr > 4);

    w = snprintfz(s + wu, wr, ",%03d", (int) (val % 1000));
    ASSERT(w == 4);
    wu += w;
    wr -= w;

    return wu;
}

static int formatD12(char* s, Size max, FmtSpec spec, double val)
{
    Size        rqd;
    const char* fmt;
    double      aval;
    int         w;

    rqd = 4;
    if ( spec & FSM_PADDING || val < 0 ) rqd++;
    if ( max < rqd ) return -1;

    if ( val <= -9.9950 || val >= 9.9950 )
    {
        if ( val < 0 )
        {
            fmt = "--.--";
        }
        else
        {
            switch ( spec & FSM_PADDING )
            {
                case FS_AUTO:   fmt =  "+.++"; break;
                case FS_PL:     fmt = " +.++"; break;
                case FS_PR:     fmt = "+.++ "; break;
                case FS_PZ:
                case FS_PP:     fmt = "++.++"; break;

                default: ASSERT(false); return -1;
            }
        }

        strcpy(s, fmt);
        return -1;
    }

    aval = absv(val);

    if ( val < 0 )
    {
        fmt =  "-%01.2f";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:   fmt =  "%01.2f"; break;
            case FS_PL:     fmt = " %01.2f"; break;
            case FS_PR:     fmt = "%01.2f "; break;
            case FS_PZ:     fmt = "0%01.2f"; break;
            case FS_PP:     fmt = "+%01.2f"; break;

            default: ASSERT(false); return -1;
        }
    }

    w = snprintfz(s, max, fmt, aval);

    ASSERT(w == (int) rqd);
    return w;
}

static int formatD5V(char* s, Size max, FmtSpec spec, double val)
{
    Size        rqd;
    const char* fmt;
    double      aval;
    int         w;

    rqd = 5;
    if ( spec & FSM_PADDING || val < 0 ) rqd++;
    if ( max < rqd ) return -1;

    if ( val <= -999.950 || val >= 999.950 )
    {
        if ( val < 0 )
        {
            fmt = "------";
        }
        else
        {
            switch ( spec & FSM_PADDING )
            {
                case FS_AUTO:   fmt =  "+++++"; break;
                case FS_PL:     fmt = " +++++"; break;
                case FS_PR:     fmt = "+++++ "; break;
                case FS_PZ:
                case FS_PP:     fmt = "++++++"; break;

                default: ASSERT(false); return -1;
            }
        }

        strcpy(s, fmt);
        return -1;
    }

    aval = absv(val);

    if ( val < 0 )
    {
        if      ( aval >= 99.9950 ) fmt = "-%05.1f";
        else if ( aval >=  9.9950 ) fmt = "-%05.2f";
        else                        fmt = "-%05.3f";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:
                if      ( aval >= 99.9950 ) fmt = "%05.1f";
                else if ( aval >=  9.9950 ) fmt = "%05.2f";
                else                        fmt = "%05.3f";
                break;

            case FS_PL:
                if      ( aval >= 99.9950 ) fmt = " %05.1f";
                else if ( aval >=  9.9950 ) fmt = " %05.2f";
                else                        fmt = " %05.3f";
                break;

            case FS_PR:
                if      ( aval >= 99.9950 ) fmt = "%05.1f ";
                else if ( aval >=  9.9950 ) fmt = "%05.2f ";
                else                        fmt = "%05.3f ";
                break;

            case FS_PZ:
                if      ( aval >= 99.9950 ) fmt = "0%05.1f";
                else if ( aval >=  9.9950 ) fmt = "0%05.2f";
                else                        fmt = "0%05.3f";
                break;

            case FS_PP:
                if      ( aval >= 99.9950 ) fmt = "%+05.1f";
                else if ( aval >=  9.9950 ) fmt = "%+05.2f";
                else                        fmt = "%+05.3f";
                break;

            default:
                ASSERT(false);
                return -1;
        }
    }

    w = snprintfz(s, max, fmt, aval);

    ASSERT(w == (int) rqd);
    return w;
}

static int formatE33(char* s, Size max, FmtSpec spec, double val)
{
    Size        rqd;
    int         e;
    double      aval, b;
    int         w;
    const char* fmt;

    rqd = 10;
    if ( spec & FSM_PADDING || val < 0 ) rqd++;
    if ( max < rqd ) return -1;

    aval = absv(val);

    e = (int) log10(aval);
    b = aval * pow(10, -1.0 * e);

    // corner case

    if (b > 9.999)
    {
        ASSERT(b < 10.001);
        b = 9.999;
    }

    if ( val < 0 )
    {
        fmt = "-%5.3fe%+04d";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:   fmt = "%5.3fe%+04d";  break;
            case FS_PL:     fmt = "% 5.3fe%+04d"; break;
            case FS_PR:     fmt = "%5.3fe%+04d "; break;
            case FS_PZ:     fmt = "%05.3fe%+04d"; break;
            case FS_PP:     fmt = "%+5.3fe%+04d"; break;

            default: ASSERT(false); return -1;
        }
    }

    w = snprintfz(s, max, fmt, b, e);
    ASSERT( w == (int)rqd);
    return w;
}

static int formatUPD(char* s, Size max, FmtSpec spec, double val)
{
    int         rqd, w;
    double      aval, q;
    int         m;
    const char* fmt;
    const char* u;

    aval = absv(val);

    m = 0;
    q = (double) aval;

    do
    {
        q = q / 1000.0;
        m++;

    } while ( q >= 1000.0 );

    if ( val < 0 )
    {
        if      ( q >= 100.0 ) fmt = "-%05.1f %s";
        else if ( q >= 10.0  ) fmt = "-%05.2f %s";
        else                   fmt = "-%05.3f %s";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:
                if      ( q >= 100.0 ) fmt = "%05.1f %s";
                else if ( q >= 10.0  ) fmt = "%05.2f %s";
                else                   fmt = "%05.3f %s";
                break;

            case FS_PL:
                if      ( q >= 100.0 ) fmt = " %05.1f %s";
                else if ( q >= 10.0  ) fmt = " %05.2f %s";
                else                   fmt = " %05.3f %s";
                break;

            case FS_PR:
                if      ( q >= 100.0 ) fmt = "%05.1f %s ";
                else if ( q >= 10.0  ) fmt = "%05.2f %s ";
                else                   fmt = "%05.3f %s ";
                break;

            case FS_PZ:
                if      ( q >= 100.0 ) fmt = "0%05.1f %s";
                else if ( q >= 10.0  ) fmt = "0%05.2f %s";
                else                   fmt = "0%05.3f %s";
                break;

            case FS_PP:
                if      ( q >= 100.0 ) fmt = "+%05.1f %s";
                else if ( q >= 10.0  ) fmt = "+%05.2f %s";
                else                   fmt = "+%05.3f %s";
                break;

            default:
                ASSERT(false);
                return -1;
        }
    }

    rqd = 6;

    if ( spec & FSM_PADDING || val < 0 ) rqd++;

    if ( spec & FSM_FRIENDLY )
    {
        rqd += 4;

        switch ( m )
        {
            case 1: u = "kilo"; break;
            case 2: u = "mega"; break;
            case 3: u = "giga"; break;
            case 4: u = "tera"; break;
            case 5: u = "peta"; break;
            case 6: u = "exa"; rqd--; break; // N.B.
            default: ASSERT(false); return -1;
        }
    }
    else
    {
        rqd += 1;

        switch ( m )
        {
            case 1: u = "k"; break;
            case 2: u = "M"; break;
            case 3: u = "G"; break;
            case 4: u = "T"; break;
            case 5: u = "P"; break;
            case 6: u = "E"; break;
            default: ASSERT(false); return -1;
        }
    }

    if ( max < (Size) rqd ) return -1;

    w = snprintf(s, max, fmt, q, u);
    ASSERT(w == rqd);

    return w;
}

static int formatUPB(char* s, Size max, FmtSpec spec, double val)
{
    int         rqd, w;
    double      aval, q;
    int         m;
    const char* fmt;
    const char* u;

    aval = absv(val);

    m = 0;
    q = (double) aval;

    do
    {
        q = q / 1024.0;
        m++;

    } while ( q >= 1000.0 );

    if ( val < 0 )
    {
        if      ( q >= 100.0 ) fmt = "-%05.1f %s";
        else if ( q >= 10.0  ) fmt = "-%05.2f %s";
        else                   fmt = "-%05.3f %s";
    }
    else
    {
        switch ( spec & FSM_PADDING )
        {
            case FS_AUTO:
                if      ( q >= 100.0 ) fmt = "%05.1f %s";
                else if ( q >= 10.0  ) fmt = "%05.2f %s";
                else                   fmt = "%05.3f %s";
                break;

            case FS_PL:
                if      ( q >= 100.0 ) fmt = " %05.1f %s";
                else if ( q >= 10.0  ) fmt = " %05.2f %s";
                else                   fmt = " %05.3f %s";
                break;

            case FS_PR:
                if      ( q >= 100.0 ) fmt = "%05.1f %s ";
                else if ( q >= 10.0  ) fmt = "%05.2f %s ";
                else                   fmt = "%05.3f %s ";
                break;

            case FS_PZ:
                if      ( q >= 100.0 ) fmt = "0%05.1f %s";
                else if ( q >= 10.0  ) fmt = "0%05.2f %s";
                else                   fmt = "0%05.3f %s";
                break;

            case FS_PP:
                if      ( q >= 100.0 ) fmt = "+%05.1f %s";
                else if ( q >= 10.0  ) fmt = "+%05.2f %s";
                else                   fmt = "+%05.3f %s";
                break;

            default:
                ASSERT(false);
                return -1;
        }
    }

    rqd = 6;

    if ( spec & FSM_PADDING || val < 0 ) rqd++;

    if ( spec & FSM_FRIENDLY )
    {
        rqd += 4;

        switch ( m )
        {
            case 1: u = "kibi"; break;
            case 2: u = "mebi"; break;
            case 3: u = "gibi"; break;
            case 4: u = "tebi"; break;
            case 5: u = "pebi"; break;
            case 6: u = "exbi"; break;
            default: ASSERT(false); return -1;
        }
    }
    else
    {
        rqd += 2;

        switch ( m )
        {
            case 1: u = "Ki"; break;
            case 2: u = "Mi"; break;
            case 3: u = "Gi"; break;
            case 4: u = "Ti"; break;
            case 5: u = "Pi"; break;
            case 6: u = "Ei"; break;
            default: ASSERT(false); return -1;
        }
    }

    if ( max < (Size) rqd ) return -1;

    w = snprintf(s, max, fmt, q, u);
    ASSERT(w == rqd);

    return w;
}

// EOF