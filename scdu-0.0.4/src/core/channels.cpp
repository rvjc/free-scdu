// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>
#include <stdarg.h>

#include "core.h"

static const bool sharedConsole =   cmd.env.consoleStreams.s &&
                                    cmd.env.consoleStreams.d;

static const char* stdNewline = NEWLINE_DEF;
static const char* dgnNewline = NEWLINE_DEF;
static const char* logNewline = NEWLINE_DEF;

static File* logout = 0;
static char* lineP = 0;
static char* wipeP = 0;

static bool opened = false;

// pre-calculated booleans help boost performance

static bool stdFlushable = false;
static bool dgnFlushable = false;
static bool logFlushable = false;

static bool stdProgress = false;
static bool dgnProgress = false;
static bool logProgress = false;

static bool  stdC = false;
static bool  stdS = false;
static bool  stdA = false;
static bool  stdR = false;
static bool  stdP = false;
static bool  stdI = false;
static bool  stdW = false;
static bool  stdE = false;
static bool  stdV = false;
static bool  stdD = false;
static bool  stdT = false;

static bool  dgnC = false;
static bool  dgnS = false;
static bool  dgnA = false;
static bool  dgnR = false;
static bool  dgnP = false;
static bool  dgnI = false;
static bool  dgnW = false;
static bool  dgnE = false;
static bool  dgnV = false;
static bool  dgnD = false;
static bool  dgnT = false;

static bool  logC = false;
static bool  logS = false;
static bool  logA = false;
static bool  logR = false;
static bool  logP = false;
static bool  logI = false;
static bool  logW = false;
static bool  logE = false;
static bool  logV = false;
static bool  logD = false;
static bool  logT = false;

static bool  routeC = false;
static bool  routeS = false;
static bool  routeA = false;
static bool  routeR = false;
static bool  routeP = false;
static bool  routeI = false;
static bool  routeW = false;
static bool  routeE = false;
static bool  routeV = false;
static bool  routeD = false;
static bool  routeT = false;

static void stdPutv(const char* fmt, va_list args);
static void stdPutc(int c);
static void stdPuts(const char* s);

static void dgnPutv(const char* fmt, va_list args);
static void dgnPutc(int c);
static void dgnPuts(const char* s);

static void logPutv(const char* fmt, va_list args);
static void logPutc(int c);
static void logPuts(const char* s);

const ChanDef chanDefs[] =
{
    { CH_C, "C", "command", "for generating program preamble and postable"       },
    { CH_S, "S", "summary", "for summarising program execution stats"            },
    { CH_A, "A", "action",  "for indicating that the current action has started" },
    { CH_R, "R", "result",  "for generating action results"                      },
    { CH_P, "P", "progress","for tracking potentially long-running actions"      },
    { CH_I, "I", "info",    "for logging info"                                   },
    { CH_W, "W", "warning", "for logging warnings"                               },
    { CH_E, "E", "error",   "for logging errors"                                 },
    { CH_V, "V", "verbose", "for logging extra info that is occasionally useful" },
    { CH_D, "D", "debug",   "for logging extra info to assist debugging"         },
    { CH_T, "T", "test",    "for logging temporary info during testing" }
};

#if defined SCDU_MODE_DEBUG

    #define FDB() stdFlush()
    #define FDE() dgnFlush()

#else

    #define FDB() (sharedConsole ? stdFlush() : (void) 0)
    #define FDE() (sharedConsole ? dgnFlush() : (void) 0)

#endif

void openChannels()
{
    const char* path;
    const char* mode;

    const Pick& ns = cmd.options.newlineStd;
    const Pick& nd = cmd.options.newlineDgn;
    const Pick& nl = cmd.options.newlineLog;

    const Pick& rs = cmd.options.routeStd;
    const Pick& rd = cmd.options.routeDgn;
    const Pick& rl = cmd.options.routeLog;

    const Pick& lm = cmd.options.logMode;

    Size w;

    ASSERT(!opened);

    stdNewline = ns.D ? NEWLINE_DEF : ns.W ? NEWLINE_WIN : NEWLINE_NIX;
    dgnNewline = nd.D ? NEWLINE_DEF : nd.W ? NEWLINE_WIN : NEWLINE_NIX;
    logNewline = nl.D ? NEWLINE_DEF : nl.W ? NEWLINE_WIN : NEWLINE_NIX;

    if ( !lm.isNull() && !rl.isNull() )
    {
        path = cmd.options.logFile.cb();
        mode = lm.O ? "wb" : lm.A ? "ab" : 0;
        ASSERT(mode != 0);

        logout = fileOpen(path, mode);
        if (logout == 0)
        {
            xer(XE_STREAM, "log: cannot open", path);
        }
    }

    w = cmd.options.progressWidth - 1;

    memAlloc(&lineP, w + 2);
    lineP[0] = 0;

    memAlloc(&wipeP, w + 2);
    memset(wipeP, ' ', w);
    wipeP[w] = '\r';
    wipeP[w + 1] = 0;

    stdC = rs.C;
    stdS = rs.S;
    stdA = rs.A;
    stdR = rs.R;
    stdP = rs.P;
    stdI = rs.I;
    stdW = rs.W;
    stdE = rs.E;
    stdV = rs.V;
    stdD = rs.D;
    stdT = rs.T;

    dgnC = rd.C;
    dgnS = rd.S;
    dgnA = rd.A;
    dgnR = rd.R;
    dgnP = rd.P;
    dgnI = rd.I;
    dgnW = rd.W;
    dgnE = rd.E;
    dgnV = rd.V;
    dgnD = rd.D;
    dgnT = rd.T;

    if ( !lm.isNull() )
    {
        logC = rl.C;
        logS = rl.S;
        logA = rl.A;
        logR = rl.R;
        logP = rl.P;
        logI = rl.I;
        logW = rl.W;
        logE = rl.E;
        logV = rl.V;
        logD = rl.D;
        logT = rl.T;
    }

    routeC = stdC || dgnC || logC;
    routeS = stdS || dgnS || logS;
    routeA = stdA || dgnA || logA;
    routeR = stdR || dgnR || logR;
    routeP = stdP || dgnP || logP;
    routeI = stdI || dgnI || logI;
    routeW = stdW || dgnW || logW;
    routeE = stdE || dgnE || logE;
    routeV = stdV || dgnV || logV;
    routeD = stdD || dgnD || logD;
    routeT = stdT || dgnT || logT;

    opened = true;
}

void closeChannels()
{
    ASSERT(opened);

    stdFlush();
    dgnFlush();
    logFlush();

    stdFlushable = false;
    dgnFlushable = false;
    logFlushable = false;

    stdProgress = false;
    dgnProgress = false;
    logProgress = false;

    stdC = false;
    stdS = false;
    stdA = false;
    stdR = false;
    stdP = false;
    stdI = false;
    stdW = false;
    stdE = false;
    stdV = false;
    stdD = false;
    stdT = false;

    dgnC = false;
    dgnS = false;
    dgnA = false;
    dgnR = false;
    dgnP = false;
    dgnI = false;
    dgnW = false;
    dgnE = false;
    dgnV = false;
    dgnD = false;
    dgnT = false;

    logC = false;
    logS = false;
    logA = false;
    logR = false;
    logP = false;
    logI = false;
    logW = false;
    logE = false;
    logV = false;
    logD = false;
    logT = false;

    routeC = false;
    routeS = false;
    routeA = false;
    routeR = false;
    routeP = false;
    routeI = false;
    routeW = false;
    routeE = false;
    routeV = false;
    routeD = false;
    routeT = false;

    stdNewline = NEWLINE_DEF;
    dgnNewline = NEWLINE_DEF;
    logNewline = NEWLINE_DEF;

    if ( logout != 0 )
    {
        fileClose(logout);
        logout = 0;
    }

    memFree(&wipeP, cmd.options.progressWidth + 1);
    memFree(&lineP, cmd.options.progressWidth + 1);

    opened = false;
}

bool channelsOpened()
{
    return opened;
}

void stdFlush()
{
    Size catchup;
    Size buflen;

    if ( !stdFlushable ) return;

    buflen = fileBufLen(stdout);

    if ( buflen > 0 )
    {
        if ( fileFlush(stdout) < 0 )
        {
            xer(XE_STREAM, "std", "flush fail");
        }
    }

    if ( !cmd.options.fastStreams.s )
    {
        catchup = cmd.options.flushDelay
                + (buflen * cmd.options.flushFactor) / 1000;

        if ( catchup > cmd.options.flushLimit )
        {
            catchup = cmd.options.flushLimit;
        }

        milliSleep(catchup);
    }
}

void dgnFlush()
{
    Size catchup;
    Size buflen;

    if ( !dgnFlushable ) return;

    buflen = fileBufLen(stderr);

    if ( buflen > 0 )
    {
        if ( fileFlush(stderr) < 0 )
        {
            xer(XE_STREAM, "dgn", "flush fail");
        }
    }

    if ( !cmd.options.fastStreams.d )
    {
        catchup = cmd.options.flushDelay
                + (buflen * cmd.options.flushFactor) / 1000;

        if ( catchup > cmd.options.flushLimit )
        {
            catchup = cmd.options.flushLimit;
        }

        milliSleep(catchup);
    }
}

void logFlush()
{
    Size catchup;
    Size buflen;

    if ( !logFlushable ) return;

    buflen = fileBufLen(logout);

    if ( buflen > 0 )
    {
        if ( fileFlush(logout) < 0 )
        {
            xer(XE_STREAM, "log", "flush fail");
        }
    }

    if ( !cmd.options.fastStreams.l )
    {
        catchup = cmd.options.flushDelay
                + (buflen * cmd.options.flushFactor) / 1000;

        if ( catchup > cmd.options.flushLimit )
        {
            catchup = cmd.options.flushLimit;
        }

        milliSleep(catchup);
    }
}

void outC()
{
    if ( !routeC ) return;

    if ( stdC ) { stdPuts(stdNewline); }
    if ( dgnC ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logC ) { logPuts(logNewline); }
}

void outC(const char* s)
{
    if ( !routeC ) return;

    if ( stdC ) { stdPuts(s); stdPuts(stdNewline); }
    if ( dgnC ) { FDB(); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logC ) { logPuts(s); logPuts(logNewline); }
}

void ouvC(const char* fmt, va_list args)
{
    if ( !routeC ) return;

    if ( stdC ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnC ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logC ) { logPutv(fmt, args); logPuts(logNewline); }
}

void oufC(const char* fmt, ...)
{
    va_list args;

    if ( !routeC ) return;

    va_start (args, fmt);

    if ( stdC ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnC ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logC ) { logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outS()
{
    if ( !routeS ) return;

    if ( stdS ) { stdPuts(stdNewline); }
    if ( dgnS ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logS ) { logPuts(logNewline); }
}

void outS(const char* s)
{
    if ( !routeS ) return;

    if ( stdS ) { stdPuts(s); stdPuts(stdNewline); }
    if ( dgnS ) { FDB(); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logS ) { logPuts(s); logPuts(logNewline); }
}

void ouvS(const char* fmt, va_list args)
{
    if ( !routeS ) return;

    if ( stdS ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnS ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logS ) { logPutv(fmt, args); logPuts(logNewline); }
}

void oufS(const char* fmt, ...)
{
    va_list args;

    if ( !routeS ) return;

    va_start (args, fmt);

    if ( stdS ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnS ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logS ) { logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outA()
{
    if ( !routeA ) return;

    if ( stdA ) { stdPuts(stdNewline); }
    if ( dgnA ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logA ) { logPuts(logNewline); }
}

void outA(const char* s)
{
    if ( !routeA ) return;

    if ( stdA ) { stdPuts(s); stdPuts(stdNewline); }
    if ( dgnA ) { FDB(); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logA ) { logPuts(s); logPuts(logNewline); }
}

void ouvA(const char* fmt, va_list args)
{
    if ( !routeA ) return;

    if ( stdA ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnA ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logA ) { logPutv(fmt, args); logPuts(logNewline); }
}

void oufA(const char* fmt, ...)
{
    va_list args;

    if ( !routeA ) return;

    va_start (args, fmt);

    if ( stdA ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnA ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logA ) { logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outR()
{
    if ( !routeR ) return;

    if ( stdR ) { stdPuts(stdNewline); }
    if ( dgnR ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logR ) { logPuts(logNewline); }
}

void outR(const char* s)
{
    if ( !routeR ) return;

    if ( stdR ) { stdPuts(s); stdPuts(stdNewline); }
    if ( dgnR ) { FDB(); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logR ) { logPuts(s); logPuts(logNewline); }
}

void ouvR(const char* fmt, va_list args)
{
    if ( !routeR ) return;

    if ( stdR ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnR ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logR ) { logPutv(fmt, args); logPuts(logNewline); }
}

void oufR(const char* fmt, ...)
{
    va_list args;

    if ( !routeR ) return;

    va_start (args, fmt);

    if ( stdR ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnR ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logR ) { logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outP()
{
    if ( !routeP ) return;
    if ( !stdProgress && !dgnProgress && !logProgress ) return;

    if ( cmd.options.progressFeed.I )
    {
        if ( stdP && stdProgress ) { stdProgress = false; stdPuts(stdNewline); }
        if ( dgnP && dgnProgress ) { dgnProgress = false; FDB(); dgnPuts(dgnNewline); FDE(); }
        if ( logP && logProgress ) { logProgress = false; logPuts(logNewline); }
    }
    else
    {
        if ( stdP && stdProgress ) { stdProgress = false; stdPuts(wipeP); }
        if ( dgnP && dgnProgress ) { dgnProgress = false; FDB(); dgnPuts(wipeP); FDE(); }
        if ( logP && logProgress ) { logProgress = false; logPuts(wipeP); }
    }
}

void outP(Progress& p)
{
    static Timer    timer;
    static double   prv_time = 0;
    static Int64    prv_units = 0;

    QtyNum          uqn, iqn, hqn;
    double          cur_time;
    double          rem_time;
    double          dif_time;
    Int64           cur_units;
    Int64           rem_units;
    Int64           dif_units;
    double          cur_rate;
    double          avg_rate;

    Pick            pf;                 // configured progress feed option
    bool            feed;               // feed current line rather than overwrite
    int             wa, wu, wr;         // width available, used, remaining
    Int64           ue, uc, ie, ic;     // units/items estimate/complete
    double          pc;                 // percent complete
    char            s[FMT_NUM_MAX + 1]; // general purpose numeric string
    int             w, f;               // general purpose string width vars

    if ( !routeP ) return;

    if ( p.status == PS_INIT )
    {
        timer.reset();

        prv_time = 0.0;
        prv_units = 0;

        cur_time = 0.0;
        dif_time = 0.0;

        return;
    }
    else
    {
        cur_time = timer.read();
        dif_time = cur_time - prv_time;

        // display rate limit only applies to PS_NORMAL updates

        if ( p.status == PS_NORMAL && dif_time < cmd.options.progressRate )
        {
            return;
        }
    }

    *s = 0;

    wa = (int) cmd.options.progressWidth - 1;
    wu = 0;
    wr = wa;

    pf = cmd.options.progressFeed;

    if ( p.status == PS_FINAL && !pf.F )
    {
        feed = false;

        memset(lineP, ' ', wa);
        lineP[wa] = 0;

        goto done;
    }

    if ( ( p.status == PS_FINAL && pf.F ) ||
         ( p.status != PS_FINAL && pf.U ) )
    {
        feed = true;
    }
    else
    {
        feed = false;
    }

    uqn = p.unitQty;

    if ( uqn != QN_NONE )
    {
        const QtyDef& uqd = qtyDefs[uqn];

        cur_units = p.overall.units.complete;
        dif_units = cur_units - prv_units;

        cur_rate  = dif_time == 0.0 ? 0.0 : (double) dif_units / dif_time;
        avg_rate  = cur_time == 0.0 ? 0.0 : (double) cur_units / cur_time;

        rem_units = p.overall.units.estimate - cur_units;
        rem_time  = avg_rate == 0.0 ? 0.0 : (double) rem_units / avg_rate;

        prv_time = cur_time;
        prv_units = cur_units;

        ue = p.overall.units.estimate;
        uc = p.overall.units.complete;

        if (uc > ue) ue = uc;
        pc = ( ue == 0 ) ? -1.0 : (double) uc / (double) ue;

        /////////////// OVERALL PERCENT  ///////////////////

        if ( cmd.options.progressStats.P && pc >= 0.0 )
        {
            w = format(s, FMT_NUM_MAX, FS_AUTO, QN_P5V, pc);
            ASSERT(w > 0); if (w > wr) goto done;

            w = snprintfz(lineP + wu, wr, "%s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;
        }

        ///////////////////  TIME  /////////////////////////

        if ( cmd.options.progressStats.T )
        {
            w = format(s, FMT_NUM_MAX, FS_AUTO, QN_HMS, cur_time);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " : %s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;

            w = format(s, FMT_NUM_MAX, FS_AUTO, QN_HMS, rem_time);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " + %s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;
        }

        ///////////////////  UNITS  ////////////////////////

        if ( cmd.options.progressStats.U )
        {
            f = format(s, FMT_NUM_MAX, FS_AUTO, uqn, ue);
            ASSERT(f > 0); if (f > wr - 3) goto done;

            w = format(s, FMT_NUM_MAX, FS_AUTO, uqn, uc);
            ASSERT(w > 0 && w <= f);

            w = snprintfz(lineP + wu, wr, " : %*s", f, s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;

            w = format(s, FMT_NUM_MAX, FS_AUTO, uqn, ue);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " / %s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;
        }

        ///////////////////  RATE  /////////////////////////

        if ( cmd.options.progressStats.R && uqd.rate != QR_NONE )
        {
            w = formatRate(s, FMT_NUM_MAX, FS_AUTO, uqn, avg_rate);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " : %s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;

            w = formatRate(s, FMT_NUM_MAX, FS_AUTO, uqn, cur_rate);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " (%s)", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;
        }
    }

    iqn = p.itemQty;

    if ( iqn != QN_NONE )
    {
        ie = p.overall.items.estimate;
        ic = p.overall.items.complete;

        if (ic > ie) ie = ic;

        ue = p.current.units.estimate;
        uc = p.current.units.complete;

        if (uc > ue) ue = uc;
        pc = ( ue == 0 ) ? -1.0 : (double) uc / (double) ue;

        ///////////////////  ITEMS  ////////////////////////

        if ( cmd.options.progressStats.I )
        {
            f = format(s, FMT_NUM_MAX, FS_B, iqn, ie);
            ASSERT(f > 0); if (f > wr - 3) goto done;

            w = format(s, FMT_NUM_MAX, FS_B, iqn, ic);
            ASSERT(w > 0 && w <= f);

            w = snprintfz(lineP + wu, wr, " : %*s", f, s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;

            w = format(s, FMT_NUM_MAX, FS_AUTO, iqn, ie);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " / %s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;
        }

        /////////////  CURRENT PERCENT  ////////////////////

        if ( cmd.options.progressStats.P && pc >= 0.0 )
        {
            w = format(s, FMT_NUM_MAX, FS_AUTO, QN_P5V, pc);
            ASSERT(w > 0); if (w > wr - 3) goto done;

            w = snprintfz(lineP + wu, wr, " : %s", s);
            ASSERT(w > 0 && w <= wr);

            wu += w;
            wr -= w;
        }
    }

    ////////////////////  HITS  ///////////////////////////

    hqn = p.hitsQty;

    if ( cmd.options.progressStats.H && hqn != QN_NONE )
    {
        // eventual hits are unknown, so we need to minimise
        // horizontal shifts without wasting too much space
        // best approach: assume initial max hits of 9999
        // this leaves three extra spaces initially and a
        // single character shift at 10K; 100K; 1M, etc

        f = format(s, FMT_NUM_MAX, FS_AUTO, hqn, I64(9999));
        ASSERT(f > 0); if (f > wr - 3) goto done;

        w = format(s, FMT_NUM_MAX, FS_AUTO, hqn, p.hits);
        ASSERT(w > 0); if (w > wr - 3) goto done;

        f = maxv(f, w);

        w = snprintfz(lineP + wu, wr, " : %*s", f, s);
        ASSERT(w > 0 && w <= wr);

        wu += w;
        wr -= w;
    }

    /////////////////////  SNIP  //////////////////////////

    if ( cmd.options.progressStats.S && wr >= 5 && p.snip != 0 && p.snip[0] != 0 )
    {
        strncpyz(lineP + wu, " : ", wr);

        wu += 3;
        wr -= 3;

        w = fitz(lineP + wu, p.snip, wr, 0);

        ASSERT(w > 0 && w <= wr);

        wu += w;
        wr -= w;
    }

    /////////////////// END OF DATA ////////////////////////

done:

    ASSERT_ALWAYS(wr >= 0);

    // pad out rest of line if no progress feed
    // ensuring that previous data is over-written

    if ( wr > 0 && !feed )
    {
        memset(lineP + wu, ' ', wr);
        lineP[wu] = 0;

        wu += wr;
        wr = 0;

        ASSERT(wu == wa);
    }

    stdProgress = false;
    dgnProgress = false;
    logProgress = false;

    if ( feed )
    {
        if ( stdP ) { stdPuts(lineP); stdPuts(stdNewline); }
        if ( dgnP ) { FDB(); dgnPuts(lineP); dgnPuts(dgnNewline); FDE(); }
        if ( logP ) { logPuts(lineP); logPuts(logNewline); }
    }
    else
    {
        if ( stdP ) { stdPuts(lineP); stdPutc('\r'); }
        if ( dgnP ) { FDB(); dgnPuts(lineP); dgnPutc('\r'); FDE(); }
        if ( logP ) { logPuts(lineP); logPutc('\r'); }
    }

    if ( !feed && p.status != PS_FINAL )
    {
        stdProgress = stdP;
        dgnProgress = dgnP;
        logProgress = logP;
    }
}

void outI()
{
    if ( !routeI ) return;

    if ( stdI ) { stdPuts(stdNewline); }
    if ( dgnI ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logI ) { logPuts(logNewline); }
}

void outI(const char* s)
{
    if ( !routeI ) return;

    if ( stdI ) { stdPuts(s); stdPuts(stdNewline); }
    if ( dgnI ) { FDB(); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logI ) { logPuts(s); logPuts(logNewline); }
}

void ouvI(const char* fmt, va_list args)
{
    if ( !routeI ) return;

    if ( stdI ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnI ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logI ) { logPutv(fmt, args); logPuts(logNewline); }
}

void oufI(const char* fmt, ...)
{
    va_list args;

    if ( !routeI ) return;

    va_start (args, fmt);

    if ( stdI ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnI ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logI ) { logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outW()
{
    if ( !routeW ) return;

    if ( stdW ) { stdPuts(stdNewline); }
    if ( dgnW ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logW ) { logPuts(logNewline); }
}

void outW(const char* s)
{
    if ( !routeW ) return;

    if ( stdW ) { stdPuts("WARNING: "); stdPuts(s); stdPuts(stdNewline); }
    if ( dgnW ) { FDB(); stdPuts("WARNING: "); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logW ) { stdPuts("WARNING: "); logPuts(s); logPuts(logNewline); }
}

void ouvW(const char* fmt, va_list args)
{
    if ( !routeW ) return;

    if ( stdW ) { stdPuts("WARNING: "); stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnW ) { FDB(); stdPuts("WARNING: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logW ) { stdPuts("WARNING: "); logPutv(fmt, args); logPuts(logNewline); }
}

void oufW(const char* fmt, ...)
{
    va_list args;

    if ( !routeW ) return;

    va_start (args, fmt);

    if ( stdW ) { stdPuts("WARNING: "); stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnW ) { FDB(); stdPuts("WARNING: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logW ) { stdPuts("WARNING: "); logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outE()
{
    if ( !routeE ) return;

    if ( stdE ) { stdPuts(stdNewline); }
    if ( dgnE ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logE ) { logPuts(logNewline); }
}

void outE(const char* s)
{
    if ( !routeE ) return;

    if ( stdE ) { stdPuts("ERROR: "); stdPuts(s); stdPuts(stdNewline); }
    if ( dgnE ) { FDB(); stdPuts("ERROR: "); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logE ) { stdPuts("ERROR: "); logPuts(s); logPuts(logNewline); }
}

void ouvE(const char* fmt, va_list args)
{
    if ( !routeE ) return;

    if ( stdE ) { stdPuts("ERROR: "); stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnE ) { FDB(); stdPuts("ERROR: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logE ) { stdPuts("ERROR: "); logPutv(fmt, args); logPuts(logNewline); }
}

void oufE(const char* fmt, ...)
{
    va_list args;

    if ( !routeE ) return;

    va_start (args, fmt);

    if ( stdE ) { stdPuts("ERROR: "); stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnE ) { FDB(); stdPuts("ERROR: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logE ) { stdPuts("ERROR: "); logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

void outV()
{
    if ( !routeV ) return;

    if ( stdV ) { stdPuts(stdNewline); }
    if ( dgnV ) { FDB(); dgnPuts(dgnNewline); FDE(); }
    if ( logV ) { logPuts(logNewline); }
}

void outV(const char* s)
{
    if ( !routeV ) return;

    if ( stdV ) { stdPuts(s); stdPuts(stdNewline); }
    if ( dgnV ) { FDB(); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
    if ( logV ) { logPuts(s); logPuts(logNewline); }
}

void ouvV(const char* fmt, va_list args)
{
    if ( !routeV ) return;

    if ( stdV ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnV ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logV ) { logPutv(fmt, args); logPuts(logNewline); }
}

void oufV(const char* fmt, ...)
{
    va_list args;

    if ( !routeV ) return;

    va_start (args, fmt);

    if ( stdV ) { stdPutv(fmt, args); stdPuts(stdNewline); }
    if ( dgnV ) { FDB(); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
    if ( logV ) { logPutv(fmt, args); logPuts(logNewline); }

    va_end (args);
}

#if defined SCDU_MODE_DEBUG

    void outD_()
    {
        if ( !routeD ) return;

        if ( stdD ) { stdPuts(stdNewline); }
        if ( dgnD ) { FDB(); dgnPuts(dgnNewline); FDE(); }
        if ( logD ) { logPuts(logNewline); }
    }

    void outD_(const char* s)
    {
        if ( !routeD ) return;

        if ( stdD ) { stdPuts("DEBUG: "); stdPuts(s); stdPuts(stdNewline); }
        if ( dgnD ) { FDB(); stdPuts("DEBUG: "); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
        if ( logD ) { stdPuts("DEBUG: "); logPuts(s); logPuts(logNewline); }
    }

    void ouvD_(const char* fmt, va_list args)
    {
        if ( !routeD ) return;

        if ( stdD ) { stdPuts("DEBUG: "); stdPutv(fmt, args); stdPuts(stdNewline); }
        if ( dgnD ) { FDB(); stdPuts("DEBUG: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
        if ( logD ) { stdPuts("DEBUG: "); logPutv(fmt, args); logPuts(logNewline); }
    }

    void oufD_(const char* fmt, ...)
    {
        va_list args;

        if ( !routeD ) return;

        va_start (args, fmt);

        if ( stdD ) { stdPuts("DEBUG: "); stdPutv(fmt, args); stdPuts(stdNewline); }
        if ( dgnD ) { FDB(); stdPuts("DEBUG: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
        if ( logD ) { stdPuts("DEBUG: "); logPutv(fmt, args); logPuts(logNewline); }

        va_end (args);
    }

    void outT_()
    {
        if ( !routeT ) return;

        if ( stdT ) { stdPuts(stdNewline); }
        if ( dgnT ) { FDB(); dgnPuts(dgnNewline); FDE(); }
        if ( logT ) { logPuts(logNewline); }
    }

    void outT_(const char* s)
    {
        if ( !routeT ) return;

        if ( stdT ) { stdPuts("TEST: "); stdPuts(s); stdPuts(stdNewline); }
        if ( dgnT ) { FDB(); stdPuts("TEST: "); dgnPuts(s); dgnPuts(dgnNewline); FDE(); }
        if ( logT ) { stdPuts("TEST: "); logPuts(s); logPuts(logNewline); }
    }

    void ouvT_(const char* fmt, va_list args)
    {
        if ( !routeT ) return;

        if ( stdT ) { stdPuts("TEST: "); stdPutv(fmt, args); stdPuts(stdNewline); }
        if ( dgnT ) { FDB(); stdPuts("TEST: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
        if ( logT ) { stdPuts("TEST: "); logPutv(fmt, args); logPuts(logNewline); }
    }

    void oufT_(const char* fmt, ...)
    {
        va_list args;

        if ( !routeT ) return;

        va_start (args, fmt);

        if ( stdT ) { stdPuts("TEST: "); stdPutv(fmt, args); stdPuts(stdNewline); }
        if ( dgnT ) { FDB(); stdPuts("TEST: "); dgnPutv(fmt, args); dgnPuts(dgnNewline); FDE(); }
        if ( logT ) { stdPuts("TEST: "); logPutv(fmt, args); logPuts(logNewline); }

        va_end (args);
    }

#endif

static void stdPutv(const char* fmt, va_list args)
{
    if ( stdProgress ) outP();

    stdFlushable = true;

    if ( vfprintf(stdout, fmt, args) < 0 )
    {
        xer(XE_STREAM, "std", "print fail");
    }
}

static void stdPuts(const char* s)
{
    if ( stdProgress ) outP();

    stdFlushable = true;

    if ( fputs(s, stdout) < 0 )
    {
        xer(XE_STREAM, "std", "puts fail");
    }
}

static void stdPutc(int c)
{
    if ( stdProgress ) outP();

    stdFlushable = true;

    if ( fputc(c, stdout) < 0 )
    {
        xer(XE_STREAM, "std", "putc fail");
    }
}

static void dgnPutv(const char* fmt, va_list args)
{
    if ( dgnProgress ) outP();

    dgnFlushable = true;

    if ( vfprintf(stderr, fmt, args) < 0 )
    {
        xer(XE_STREAM, "dgn", "print fail");
    }
}

static void dgnPuts(const char* s)
{
    if ( dgnProgress ) outP();

    dgnFlushable = true;

    if ( fputs(s, stderr) < 0 )
    {
        xer(XE_STREAM, "dgn", "puts fail");
    }
}

static void dgnPutc(const int c)
{
    if ( dgnProgress ) outP();

    dgnFlushable = true;

    if ( fputc(c, stderr) < 0 )
    {
        xer(XE_STREAM, "dgn", "putc fail");
    }
}

static void logPutv(const char* fmt, va_list args)
{
    if ( logProgress ) outP();

    logFlushable = true;

    if ( vfprintf(logout, fmt, args) < 0 )
    {
        xer(XE_STREAM, "log", "print fail");
    }
}

static void logPuts(const char* s)
{
    if ( logProgress ) outP();

    logFlushable = true;

    if ( fputs(s, logout) < 0 )
    {
        xer(XE_STREAM, "log", "puts fail");
    }
}

static void logPutc(int c)
{
    if ( logProgress ) outP();

    logFlushable = true;

    if ( fputc(c, logout) < 0 )
    {
        xer(XE_STREAM, "log", "putc fail");
    }
}

// EOF