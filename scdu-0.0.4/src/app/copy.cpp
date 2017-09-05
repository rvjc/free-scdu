// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "../core/core.h"
#include "../ffs/file.h"

#include "copy.h"

static FileReader reader;
static FileWriter writer;

static void copyFile(const char* src, const char* dst);

void copy()
{
    reader.reserve(cmd.options.bufferSize);
    writer.reserve(cmd.options.bufferSize);

    ASSERT(cmd.params.count == 2);

    const char*  src = cmd.params[0].cb();
    const char*  dst = cmd.params[1].cb();

    outA("copying");

    copyFile(src, dst);  // currently only file copies supported

    outR("OK");
    outR();

    reader.release();
    writer.release();
}

static void copyFile(const char* src, const char* dst)
{
    Int64 s;

    reader.open(src);
    writer.open(dst);

    s = reader.size();

    oufI("from: %s", src);
    oufI("to:   %s", dst);
    oufI("size: " F64u() " bytes", s);

    writer.put(reader);

    reader.close();
    writer.close();
}

// EOF