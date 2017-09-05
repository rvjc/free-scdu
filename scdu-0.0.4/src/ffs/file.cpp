// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>
#include <stdarg.h>

#include "../core/core.h"

#include "file.h"

static void fer(Fen e, ...);

static Fen  fenMutable = FE_OK;
const Fen&  fen = fenMutable;

static char femMutable[FEM_MAX + 1] = "";
const char* const fem = femMutable;

const FerDef ferDefs[] =
{
    { FE_OK,        "FE_OK",        "",                 ""      },
    { FE_OPEN,      "FE_OPEN",      "cannot open",      "%s"    },
    { FE_SEEK,      "FE_SEEK",      "seek failure",     "%s"    },
    { FE_READ,      "FE_READ",      "read failure",     "%s"    },
    { FE_WRITE,     "FE_WRITE",     "write failure",    "%s"    }
};

FileBuffer::FileBuffer()
{
    mBase = 0;
    mTop = 0;
    mStart = 0;
    mEnd = 0;
    mLastCount = 0;
    mFile = 0;
    mPath = "";
}

FileBuffer::~FileBuffer()
{
    ASSERT(mBase == 0);
    ASSERT(mTop == 0);
    ASSERT(mStart == 0);
    ASSERT(mEnd == 0);
    ASSERT(mLastCount == 0);
    ASSERT(mFile == 0);
    ASSERT(mPath.len() == 0);
}

Size FileBuffer::cap() const
{
    return mTop - mBase;
}

Size FileBuffer::len() const
{
    return mEnd - mStart;
}

Int64 FileBuffer::lastCount() const
{
    return mLastCount;
}

const Str& FileBuffer::path() const
{
    return mPath;
}

bool FileBuffer::isReserved() const
{
    return (mBase != 0);
}

bool FileBuffer::isOpen() const
{
    return (mFile != 0);
}

void FileBuffer::reserve(Size chunks)
{
    Size n;

    ASSERT(mBase == 0);
    ASSERT(mTop == 0);
    ASSERT(mStart == 0);
    ASSERT(mEnd == 0);
    ASSERT(mLastCount == 0);
    ASSERT(mFile == 0);
    ASSERT(mPath.len() == 0);

    n = chunks*cmd.env.chunkSize;
    memAlloc(&mBase, n);
    mTop = mBase + n;
}

void FileBuffer::release()
{
    ASSERT(mBase != 0);
    ASSERT(mTop != 0);
    ASSERT(mStart == 0);
    ASSERT(mEnd == 0);
    ASSERT(mLastCount == 0);
    ASSERT(mFile == 0);
    ASSERT(mPath.len() == 0);

    memFree(&mBase, mTop - mBase);
    mTop = 0;
}

FileReader::FileReader()
{
    mSize = 0;
}

FileReader::~FileReader()
{
    ASSERT(mSize == 0);
}

void FileReader::open(const char* path)
{
    File* f;
    Int64 s;

    ferClear();

    ASSERT(isReserved());
    ASSERT(!isOpen());

    f = fileOpen(path, "rb");
    if ( f == 0 )
    {
        fer(FE_OPEN, path);
        return;
    }

    if ( fileSeek(f, 0, SEEK_END) < 0 )
    {
        fer(FE_SEEK, path);
        fileClose(f);
        return;
    }

    s = fileTell(f);

    if ( s < 0 )
    {
        fer(FE_SEEK, path);
        fileClose(f);
        return;
    }

    if ( fileSeek(f, 0, SEEK_SET) < 0 )
    {
        fer(FE_SEEK, path);
        fileClose(f);
        return;
    }

    mPath = path;
    mFile = f;
    mSize = s;
    mStart = mTop;
    mEnd = mTop;

    ASSERT(mLastCount == 0);

    ASSERT(!fen);
}

void FileReader::close()
{
    ASSERT(isOpen());

    fileClose(mFile);

    mStart = 0;
    mEnd = 0;
    mLastCount = 0;
    mFile = 0;
    mPath = "";
    mSize = 0;
}

Int64 FileReader::size() const
{
    return mSize;
}

Int64 FileReader::pos() const
{
    return mLastCount + mStart - mEnd;
}

void FileReader::fill()
{
    Size rcount, rcap;

    ferClear();

    ASSERT(isOpen());
    ASSERT(mStart == mEnd);
    ASSERT(mLastCount != mSize);

    rcap = mTop - mBase;
    rcount = fileRead(mBase, 1, rcap, mFile);
    if ( rcount < rcap )
    {
        if ( rcount == 0 || (rcount > 0 && !feof(mFile)) )
        {
            fer(FE_READ, mPath);
            return;
        }
    }

    mStart = mBase;
    mEnd = mBase + rcount;
    mLastCount += rcount;

    ASSERT(!fen);
}

Uint8 FileReader::get()
{
    if (mStart == mEnd)
    {
        fill();
        if ( fen ) return 0;
    }

    return *mStart++;
}

FileWriter::FileWriter()
{
    ;   // for possible future use
}

FileWriter::~FileWriter()
{
    ;   // for possible future use
}

void FileWriter::open(const char* path)
{
    File* f;

    ferClear();

    ASSERT(isReserved());
    ASSERT(!isOpen());

    f = fileOpen(path, "wb");
    if (f == 0)
    {
        fer(FE_OPEN, path);
        return;
    }

    mPath = path;
    mFile = f;

    ASSERT(mLastCount == 0);

    mStart = mBase;
    mEnd = mBase;

    ASSERT(!fen);
}

void FileWriter::close()
{
    ASSERT(isOpen());

    if ( mEnd > mStart )
    {
        flush();
        if ( fen ) return;
    }

    fileClose(mFile);

    mStart = 0;
    mEnd = 0;
    mLastCount = 0;
    mFile = 0;
    mPath = "";

    ASSERT(!fen);
}

Int64 FileWriter::size() const
{
    return mLastCount + mEnd - mStart;
}

Int64 FileWriter::pos() const
{
    return mLastCount + mEnd - mStart;
}

void FileWriter::flush()
{
    Size wcount;

    ferClear();

    ASSERT(isOpen());

    if (mEnd == mStart) return;

    ASSERT(mStart == mBase);

    wcount = fileWrite(mStart, 1, len(), mFile);
    if ( wcount < len() )
    {
        fer(FE_WRITE, mPath);
        return;
    }

    mEnd = mStart;
    mLastCount += wcount;

    ASSERT(!fen);
}

void FileWriter::flushToDisk()
{
    flush();
    if ( fen ) return;

    if ( fileFlush(mFile) < 0 )
    {
        fer(FE_WRITE, mPath);
        return;
    }

    ASSERT(fen == FE_OK);
}

void FileWriter::put(Uint8 data)
{
    if (mEnd == mTop)
    {
        flush();
        if ( fen ) return;
    }

    *mEnd++ = data;

    ASSERT(!fen);
}

void FileWriter::put(FileReader& r, Int64 count)
{
    Int64 rleft;
    Size rlen;

    // writer and reader must have identical buffer capacities

    ASSERT(r.cap() == cap());

    rleft = r.size() - r.pos();

    if ( count < 0 )
    {
        if ( rleft == 0 ) return;
    }
    else
    {
        ASSERT(count <= rleft);
        rleft = count;
    }

    flush();

    if ( fen ) return;

    while ( rleft > 0 )
    {
        rlen = r.len();
        if ( (Int64) rlen > rleft ) rlen = (Size) rleft;

        memcpy(mStart, r.mStart, rlen);
        r.mStart += rlen;
        mEnd += rlen;

        flush();

        if ( fen ) return;

        rleft -= rlen;
        if ( rleft > 0 ) r.fill();
    }

    ASSERT(!fen);
}

void ferClear()
{
    fenMutable = FE_OK;
    femMutable[0] = 0;
}

static void fer(Fen e, ...)
{
    char    fmt[FEM_MAX+1];
    FerDef  def;
    va_list args;

    fmt[0] = 0;

    va_start (args, e);

    ASSERT(e > 0 && e < FE_COUNT);

    def = ferDefs[e];
    ASSERT(def.num == e);

    if ( !strlenz(def.params) )
    {
        strncpyz(fmt, def.msg, FEM_MAX);
        strncatz(fmt, ": ", FEM_MAX);
        strncatz(fmt, def.params, FEM_MAX);
        vsnprintfz(femMutable, FEM_MAX, fmt, args);
    }
    else
    {
        strncpyz(femMutable, def.msg, FEM_MAX);
    }

    va_end (args);
    fenMutable = e;
}

// EOF