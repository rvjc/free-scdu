// Copyright 2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "../core/core.h"

#include "show.h"

static void acks();
static void authors();
static void contribs();
static void copyright();
static void terms();
static void version();

void show()
{
    ASSERT(cmd.params.count == 1);

    const Str& p = cmd.params[0];

    if      ( p == "acks"       ) acks();
    else if ( p == "authors"    ) authors();
    else if ( p == "contribs"   ) contribs();
    else if ( p == "copyright"  ) copyright();
    else if ( p == "terms"      ) terms();
    else if ( p == "version"    ) version();

    else xer(XE_CMDPRM, p.cb());
}

static void acks()
{
    if ( !cmd.options.rawReporting )
    {
        outR("SCDU Acknowledgements");
        outR("---------------------");
        outR();
    }

    for (Size i = 0; i < SCDU_ACKS_SIZE; i++)
    {
        outR(SCDU_ACKS[i]);
    }
}

static void authors()
{
    if ( !cmd.options.rawReporting )
    {
        outR("SCDU Authors");
        outR("------------");
        outR();
    }

    for (Size i = 0; i < SCDU_AUTHORS_SIZE; i++)
    {
        outR(SCDU_AUTHORS[i]);
    }
}

static void contribs()
{
    if ( !cmd.options.rawReporting )
    {
        outR("SCDU Contributors");
        outR("-----------------");
        outR();
    }

    for (Size i = 0; i < SCDU_CONTRIBS_SIZE; i++)
    {
        outR(SCDU_CONTRIBS[i]);
    }
}

static void copyright()
{
    outR(scduCopyright);

    if ( cmd.options.rawReporting )
    {
        outR(scduCopyright);
    }
    else
    {
        outR(scduNotice);
    }
}

static void terms()
{
    if ( !cmd.options.rawReporting )
    {
        outR("SCDU Terms of Use");
        outR("-----------------");
        outR();
    }

    outR(scduCopyright);
    outR();

    for (Size i = 0; i < SCDU_TERMS_SIZE; i++)
    {
        outR(SCDU_TERMS[i]);
    }
}

static void version()
{
    if ( cmd.options.rawReporting )
    {
        outR(SCDU_VERSION);
    }
    else
    {
        oufR("scdu version %s %s.%s.%s %s %s", SCDU_VERSION, SCDU_OS, SCDU_ARCH, SCDU_MODE, scduDate, SCDU_HOST);
    }
}

// EOF