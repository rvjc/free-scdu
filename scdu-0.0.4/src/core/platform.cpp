// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "core.h"

static bool initialised = false;

static char scduDateMutable[SCDU_DATE_SIZE + 1] = "";
const char* const scduDate = scduDateMutable;

static char scduYearMutable[SCDU_YEAR_SIZE + 1] = "";
const char* const scduYear = scduYearMutable;

static char scduCopyrightMutable[SCDU_COPYRIGHT_MAX + 1] = "";
const char* const scduCopyright = scduCopyrightMutable;

static char scduNoticeMutable[SCDU_NOTICE_MAX + 1] = "";
const char* const scduNotice = scduNoticeMutable;

const char* const SCDU_ORIG_YEAR = "2015";
const char* const SCDU_HOLDER = "RVJ Callanan";
const char* const SCDU_LEGAL = "Licensed under GNU GPLv3";
const char* const SCDU_BUGS = "GITHUB";

const char* const SCDU_AUTHORS[] =
{
    "RVJ Callanan"
};

const Size SCDU_AUTHORS_SIZE = sizeof(SCDU_AUTHORS)/sizeof(char*);

const char* const SCDU_CONTRIBS[] =
{
    "Shane Callanan, Carlow Institute of Technology",
    "Dennis Callanan, Maynooth University",
    "David Callanan, Gaelcholaiste Ceatharlach"
};

const Size SCDU_CONTRIBS_SIZE = sizeof(SCDU_CONTRIBS)/sizeof(char*);

const char* const SCDU_ACKS[] =
{
    "Free Software Foundation",
    "GNU Project",
    "Pandoc (John McFarlane)",
    "Skidoo",
};

const Size SCDU_ACKS_SIZE = sizeof(SCDU_ACKS)/sizeof(char*);

const char* const SCDU_TERMS[] =
{
    "SCDU is free software: you can redistribute it and/or modify",
    "it under the terms of the GNU General Public License as published by",
    "the Free Software Foundation, either version 3 of the License, or",
    "(at your option) any later version.",
    "",
    "SCDU is distributed in the hope that it will be useful,",
    "but WITHOUT ANY WARRANTY; without even the implied warranty of",
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
    "GNU General Public License for more details.",
    "",
    "You should have received a copy of the GNU General Public License",
    "along with SCDU.  If not, see <http://www.gnu.org/licenses/>."
};

const Size SCDU_TERMS_SIZE = sizeof(SCDU_TERMS)/sizeof(char*);

Timer::Timer()
{
    mStartTime = clock();
}

void Timer::reset()
{
    mStartTime = clock();
}

double Timer::read() const
{
    return ( clock() - mStartTime ) / (double) CLOCKS_PER_SEC;
}

void initPlatform()
{
    ASSERT(!initialised);

    // set standard I/O to binary mode instead of default text mode
    // this is faster, more consistent and more flexible
    // however, program is now responsible for text encodings

    errno = 0;

    if ( setMode(STDIN_FILENO,  O_BINARY) < 0 ||
         setMode(STDOUT_FILENO, O_BINARY) < 0 ||
         setMode(STDERR_FILENO, O_BINARY) < 0 )
    {
        xer(XE_STREAM, strerror(errno));
    }

    // SCDU_DATETIME derivatives require simple truncation

    strncpyz(scduDateMutable, SCDU_DATETIME, SCDU_DATE_SIZE);
    strncpyz(scduYearMutable, SCDU_DATETIME, SCDU_YEAR_SIZE);

    // construct copyright string

    snprintfz(  scduCopyrightMutable,
                SCDU_COPYRIGHT_MAX,
                "Copyright (C) %s-%s %s",
                SCDU_ORIG_YEAR,
                scduYear,
                SCDU_HOLDER);

    // construct notice string

    snprintfz(  scduNoticeMutable,
                SCDU_NOTICE_MAX,
                "SCDU %s %s. %s.",
                SCDU_VERSION,
                scduCopyright,
                SCDU_LEGAL);

    initialised = true;
}

bool platformInitialised()
{
    return initialised;
}

File* fileOpen(const char* path, const char* mode)
{
    return fopen64(path, mode);
}

int fileClose(File* stream)
{
    return fclose(stream);
}

int fileFlush(File* stream)
{
    return fflush(stream);
}

Size fileRead(void* ptr, Size size, Size count, File* stream)
{
    return fread(ptr, size, count, stream);
}

Size fileWrite(const void* ptr, Size size, Size count, File* stream)
{
    return fwrite(ptr, size, count, stream);
}

int fileSeek(File* stream, Int64 offset, int origin)
{
    return fseeko64(stream, offset, origin);
}

Int64 fileTell (File* stream)
{
    return ftello64(stream);
}

int fileDesc(File *stream)
{
    return fileno(stream);
}

Size fileBufCap(File *stream)
{
    return (Size) stream->_bufsiz;
}

Size fileBufLen(File *stream)
{
    return (Size) (stream->_ptr - stream->_base);
}

const char* fileGetS(char* line, Size max, File *stream)
{
    return fgets(line, (int) max, stream);
}

#if defined SCDU_OS_WINDOWS

    #include <windows.h>
    #include <direct.h>

    int setMode(int fd, int mode)
    {
        return _setmode (fd, mode);
    }

    int setDir(const char* path)
    {
        return _chdir(path);
    }

    const char* getDir()
    {
        static char dir[UPATH_MAX + 1];

        return _getcwd(dir, UPATH_MAX);
    }

    bool isConsole(int fd)
    {
        return _isatty(fd);
    }

    void milliSleep(Size milliseconds)
    {
        Sleep((unsigned int) milliseconds);
    }

    Size mallocSize(void* ptr)
    {
        return _msize(ptr);
    }

#else

    #include <unistd.h>

    int setMode(int fd, int mode)
    {
        return setmode (fd, mode);
    }

    int setDir(char* path)
    {
        return chdir(path);
    }

    const char* getDir()
    {
        static char dir[UPATH_MAX + 1];

        return getcwd(dir, UPATH_MAX);
    }

    bool isConsole(int fd)
    {
        return isatty(fd);
    }

    void milliSleep(Size milliseconds)
    {
        usleep((useconds_t) (milliseconds * 1000));
    }

    Size mallocSize(void* ptr)
    {
        return malloc_usable_size(ptr);
    }

#endif

Uint64 strtoUint64(const char* str, char** endptr, int base)
{
    return strtoll(str, endptr, base);
}

// EOF