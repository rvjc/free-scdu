// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <stdlib.h>

#include "../core/core.h"
#include "../alg/match.h"

#include "view.h"

static Progress progress;

void view()
{
    ASSERT_ALWAYS( isMatchCS("*match*", "match") );

    const Size MAX_SNIP = 60;
    char snip[MAX_SNIP+1];

    outA("viewing");

    const Int64 cue = 512;
    const Int64 cus = 512;

    const Int64 oie = 1234;
    const Int64 oue = oie * cue;

    progress.unitQty = QN_BYTES;
    progress.itemQty = QN_FILES;
    progress.hitsQty = QN_MATCHES;

    progress.hits = 0;
    progress.snip = 0;

    progress.overall.items.estimate = oie;
    progress.overall.items.complete = 0;
    progress.overall.units.estimate = oue;
    progress.overall.units.complete = 0;

    progress.current.units.estimate = cue;
    progress.current.units.complete = 0;

    progress.status = PS_INIT;

    for ( Int64 i = 1; i <= oie; i++ )
    {
        snprintfz(snip, MAX_SNIP, "This is a long file name with an index number " F64d(), i);

        progress.snip = snip;
        progress.current.units.complete = 0;

        for ( Int64 c = 0; c < cue; c += cus )
        {
            progress.current.units.complete += cus;
            progress.overall.units.complete += progress.current.units.complete;

            progress.hits += ( rand() % 100 );

            if ( c + cus >= cue )
            {
                progress.overall.items.complete += 1;
            }

            outP(progress);

            progress.status = PS_NORMAL;

            milliSleep(rand() % 10);
        }
    }

    progress.status = PS_FINAL;
    outP(progress);

    outR("OK");
    outR();
}

// EOF