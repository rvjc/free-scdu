// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_H

    #define CORE_H

    #define CORE_INCLUDE

    #include <errno.h>
    #include <malloc.h>
    #include <stdio.h>
    #include <io.h>
    #include <fcntl.h>
    #include <time.h>

    #include "platform.h"
    #include "error.h"
    #include "mem.h"
    #include "math.h"
    #include "str.h"
    #include "type.h"
    #include "quantity.h"
    #include "var.h"
    #include "parse.h"
    #include "format.h"
    #include "value.h"
    #include "cmd.h"
    #include "channels.h"

    #undef CORE_INCLUDE

#endif // CORE_H

// EOF