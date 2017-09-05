// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <stdarg.h>
#include <stdlib.h>

#include "core.h"

static Xen xenMutable = XE_OK;
const  Xen& xen = xenMutable;

// keep exit code backward compatibility!!!

const XerDef xerDefs[] =
{
    { XE_OK,        "XE_OK",        "success",                      ""          },
    { XE_UNKNOWN,   "XE_UNKNOWN",   "unknown error",                ""          },
    { XE_ABTERM,    "XE_ABTERM",    "abnormal termination",         ""          },
    { XE_UNKSIG,    "XE_UNKSIG",    "unknown signal",               "%d"        },
    { XE_USRINT,    "XE_USRINT",    "user interrupt",               ""          },
    { XE_MEMOUT,    "XE_MEMOUT",    "out of memory",                ""          },
    { XE_STREAM,    "XE_STREAM",    "stream error",                 "%s: %s"    },
    { XE_CMD,       "XE_CMD",       "invalid command",              "%s"        },
    { XE_CMDPRM,    "XE_CMDPRM",    "invalid command parameter",    "%s"        }
};

static void printerr(const char* fmt, ...);

void xer(Xen e, ...)
{
    // prevent reentrant call

    static bool called = false;
    ASSERT_ALWAYS( !called );
    called = true;

    char    msg[XEM_MAX+1];
    char    fmt[XEM_MAX+1];
    XerDef  def;
    va_list args;

    msg[0] = 0;
    fmt[0] = 0;

    va_start (args, e);

    if (e <= 0 || e >= XE_COUNT)
    {
        e = XE_UNKNOWN;
    }

    def = xerDefs[e];
    ASSERT(def.num == e);

    if ( !strlenz(def.params) )
    {
        strncpyz(fmt, def.msg, XEM_MAX);
        strncatz(fmt, ": ", XEM_MAX);
        strncatz(fmt, def.params, XEM_MAX);
        vsnprintfz(msg, XEM_MAX, fmt, args);
    }
    else
    {
        strncpyz(msg, def.msg, XEM_MAX);
    }

    va_end (args);

    printerr("SCDU ERROR: %s", msg);

    xenMutable = e;
    exit(exitCode(e));
}

void panic(const char* func, const char* file, int line, const char* cause)
{
    const char* newline;

    if ( platformInitialised() )
    {
        // stderr stream is in binary mode
        // always use default newline for panics

        newline = NEWLINE_DEF;
    }
    else
    {
        // stderr stream is still in text mode
        // let platform generate appropriate newline

        newline = "\n";
    }

    printerr(   "SCDU PANIC%s"
                "Function : %s%s"
                "File     : %s%s"
                "Line     : %u%s"
                "Cause    : %s",
                newline,
                func, newline,
                file, newline,
                line, newline,
                cause);
    abort();
}

int exitCode(int e)
{
    return e == 0 ? 0 : EXIT_BASE + e;
}

static void printerr(const char* fmt, ...)
{
    const char* newline;

    if ( platformInitialised() )
    {
        // stderr stream is in binary mode
        // use configured newline for normal errors

        const Pick& nd = cmd.options.newlineDgn;
        newline = nd.D ? NEWLINE_DEF : nd.W ? NEWLINE_WIN : NEWLINE_NIX;
    }
    else
    {
        // stderr stream is still in text mode
        // let platform generate appropriate newline

        newline = "\n";
    }

    va_list args;

    va_start (args, fmt);

    outP();

    fprintf(stderr, newline);
    fileFlush(stdout);

    milliSleep(100);

    vfprintf(stderr, fmt, args);
    fprintf(stderr, newline);

    fprintf(stderr, newline);
    fileFlush(stderr);

    va_end (args);
}

// EOF