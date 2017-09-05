// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <signal.h>
#include <stdlib.h>

#include "../core/core.h"

#include "copy.h"
#include "help.h"
#include "info.h"
#include "show.h"
#include "view.h"

static bool success = false;

static void exitHandler();
static void signalHandler(int sig);
static void preamble();
static void postamble();
static void recap();
static void summarise();

static Timer timer;

int main(int argc, char* argv[])
{
    atexit(exitHandler);
    signal(SIGINT, signalHandler);

    initPlatform();

    cmd.init(argc, argv);
    if ( cen ) xer(XE_CMD, cem);

    openChannels();
    preamble();

    if ( cmd.recap )
    {
        recap();
    }
    else
    {
        switch ( cmd.action.num )
        {
            case ACT_COPY: copy(); break;
            case ACT_HELP: help(); break;
            case ACT_INFO: info(); break;
            case ACT_SHOW: show(); break;
            case ACT_VIEW: view(); break;

            default: ASSERT(false);
        }
    }

    success = true;
    return XE_OK;
}

static void exitHandler()
{
    if ( success )
    {
        ASSERT_ALWAYS( xen == XE_OK);
        ASSERT_ALWAYS( channelsOpened() );

        summarise();
        postamble();
    }
    else
    {
        // Abnormal termination is detected by our registered exit handler when
        // the program has NOT completed normally yet no error is apparent as a
        // result of a prior xer() call. In these scenarios, our exit handler
        // will have been invoked by some lower level library exit() to deal
        // with the non-standard termination event as best it can). For some
        // reason (possibly POSIX compatibility), this bypasses our registered
        // signal handler used for Ctrl C detection, etc. One common example is
        // when the user hits Ctrl Break while the program is running in a
        // Windows terminal. This alternative user interrupt mechanism works
        // even when the shell process intercepts Ctrl C signals. Detecting
        // Ctrl Break (and other non-standard events) explicitly requires
        // more complex OS-specific code which we want to avoid for now.
        // But even if we decide to do this in the future, leave this abnormal
        // termination in place as a "handler of last resort".

        if ( xen == XE_OK )
        {
            xer(XE_ABTERM);
        }

        // if an error is active at this point, no further action is required
        // (apart from allowing the program to terminate) since xer() will have
        // output an appropriate error message and exit code.
    }

    if ( channelsOpened() )
    {
        closeChannels();
    }
}

static void signalHandler(int sig)
{
    switch(sig)
    {
        case SIGINT:
            xer(XE_USRINT);

        default:
            xer(XE_UNKSIG, sig);
    }
}

static void preamble()
{
    outC(scduNotice);
    outC();
}

static void postamble()
{
    outC();
    outC("SCDU completed OK");
}

static void recap()
{
    Value   val;
    char    s[FMT_MAX + 1];
    int     w, nw;

    outC("Recap only (action will not execute!)");
    outC();

    outR("ACTION");
    outR(cmd.action.name.cb());
    outR();

    outR("PARAMETERS");
    if ( cmd.params.count == 0 )
    {
        outR("<none>");
    }
    else
    {
        for ( Size i = 0; i < cmd.params.count; i++ )
        {
            outR(cmd.params[i].cb());
        }
    }
    outR();

    outR("OPTIONS");
    for ( int i = 0; i < OPT_COUNT; i++ )
    {
        OptNum          num = OptNum(i);
        const OptDef&   def = optDefs[num];

        cmd.options.get(num, val);
        w = val.format(s, FMT_MAX, FS_FR);

        ASSERT_ALWAYS(w >= 0);

        nw = int(OPT_NAME_MAX);

        oufR("%-*s  : %s", nw, def.name, s);
    }
    outR();

    outR("ENVIRONMENT");
    for ( int i = 0; i < ENV_COUNT; i++ )
    {
        EnvNum          num = EnvNum(i);
        const EnvDef&   def = envDefs[num];

        cmd.env.get(num, val);
        w = val.format(s, FMT_MAX, FS_FR);

        ASSERT_ALWAYS(w >= 0);

        nw = int(ENV_NAME_MAX);

        oufR("%-*s  : %s", nw, def.name, s);
    }
}

static void summarise()
{
    FmtSpec     spec;
    const char* fmt;
    char        s[FMT_MAX + 1];
    int         w;

    const Pick& ss = cmd.options.summaryStats;
    const bool  rr = cmd.options.rawReporting;

    if ( ss.isNull() ) return;

    if ( !rr ) outS("SUMMARY");

    if ( ss.A )
    {
        spec = rr ? FS_BR : FS_FR;
        fmt  = rr ? "%s"  : "core allocs (cur) : %s";
        w = format(s, FMT_MAX, spec, QN_BYTES, Int64(allocs.cur));
        ASSERT_ALWAYS(w >= 0);
        oufS(fmt, s);

        spec = rr ? FS_BR : FS_FR;
        fmt  = rr ? "%s"  : "core allocs (max) : %s";
        w = format(s, FMT_MAX, spec, QN_BYTES, Int64(allocs.max));
        ASSERT_ALWAYS(w >= 0);
        oufS(fmt, s);
    }

    if ( ss.D )
    {
        spec = rr ? FS_BR : FS_F;
        fmt  = rr ? "%s"  : "duration          : %s";
        w = format(s, FMT_MAX, spec, QN_HMS, timer.read());
        ASSERT(w >= 0);
        oufS(fmt, s);
    }

    outS();
}

// EOF