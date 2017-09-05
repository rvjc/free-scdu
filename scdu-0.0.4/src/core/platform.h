// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include platform.h separately - use core.h instead!"
#endif

// only support modern GNU compilers for best portability

#if !defined __GNUC__
    #error "unsupported compiler family"
#endif

// build script should have taken care of this but it is worth double-checking

#if (__GNUC__ != 6) && (__GNUC_MINOR__ != 2) && (__GNUC_PATCHLEVEL__ != 0)
    #error "GCC version needs to be 6.2.0"
#endif

// build OS/ARCH/MODE checks

#if defined SCDU_OS_WINDOWS

    #if !defined __MINGW32__
        #error "unsupported windows compiler port"
    #endif

    #if !defined __WIN32__
        #error "unsupported windows compiler"
    #endif

    const char* const SCDU_OS = "windows";

    #if defined SCDU_ARCH_I386

        #if defined __x86_64__
            #error "windows compiler does not support i386 arch"
        #endif

        #define SCDU_M32

        const char* const SCDU_ARCH = "i386";

    #elif defined SCDU_ARCH_AMD64

        #if !defined __x86_64__
            #error "windows compiler does not support amd64 arch"
        #endif

        #define SCDU_M64

        const char* const SCDU_ARCH = "amd64";

    #else

        #error "missing or invalid -D SCDU_ARCH_* compile option (windows)"

    #endif

    #if defined SCDU_MODE_DEBUG

        #if defined NDEBUG
            #error "invalid -D NDEBUG (No Debug) compile option in windows DEBUG build"
        #endif

        const char* const SCDU_MODE = "debug";

    #elif defined SCDU_MODE_RELEASE

        #if !defined NDEBUG
            #error "missing -D NDEBUG (No Debug) compile option in windows RELEASE build"
        #endif

        const char* const SCDU_MODE = "release";

    #else

        #error "missing or invalid -D SCDU_MODE_* compile option (windows)"

    #endif

#elif defined SCDU_OS_LINUX

    #error "-D SCDU_OS_LINUX compile option supported but not yet implemented"

#elif defined SCDU_OS_DARWIN

    #error "-D SCDU_OS_DARWIN compile option supported but not yet implemented"

#else

    #error "missing or invalid -D SCDU_OS_* compile option"

#endif

#define CUR_FUNC __PRETTY_FUNCTION__
#define CUR_FILE __FILE__
#define CUR_LINE __LINE__

typedef __UINT8_TYPE__  Uint8;
typedef __UINT16_TYPE__ Uint16;
typedef __UINT32_TYPE__ Uint32;
typedef __UINT64_TYPE__ Uint64;

typedef __INT8_TYPE__   Int8;
typedef __INT16_TYPE__  Int16;
typedef __INT32_TYPE__  Int32;
typedef __INT64_TYPE__  Int64;

// portable 64-bit and 32-bit literal and formatter macros

#define U64(val) val ## ULL         // unsigned integer
#define I64(val) val ## LL          // signed integer

#define F64u(mod) "%" #mod "I64u"   // unsigned decimal
#define F64d(mod) "%" #mod "I64d"   // signed decimal
#define F64x(mod) "%" #mod "I64x"   // hex (lowercase)
#define F64X(mod) "%" #mod "I64X"   // HEX (UPPERCASE)

#define U32(val) val ## UL          // unsigned integer
#define I32(val) val ## L           // signed integer

#define F32u(mod) "%" #mod "I32u"   // unsigned decimal
#define F32d(mod) "%" #mod "I32d"   // signed decimal
#define F32x(mod) "%" #mod "I32x"   // hex (lowercase)
#define F32X(mod) "%" #mod "I32X"   // HEX (UPPERCASE)

// limits

const Uint8 UINT8_VAL_MAX       = 255;
const Uint8 UINT8_VAL_MIN       = 0;
const Uint8 UINT8_BIT_MAX       = 8;
const Uint8 UINT8_HEX_MAX       = 2;
const Uint8 UINT8_DEC_MAX       = 3;
const Uint8 UINT8_CSD_MAX       = 3;

const Uint16 UINT16_VAL_MAX     = 65535;
const Uint16 UINT16_VAL_MIN     = 0;
const Uint16 UINT16_BIT_MAX     = 16;
const Uint16 UINT16_HEX_MAX     = 4;
const Uint16 UINT16_DEC_MAX     = 5;
const Uint16 UINT16_CSD_MAX     = 6;

const Uint64 UINT32_VAL_MAX     = U32(4294967295);
const Uint64 UINT32_VAL_MIN     = 0;
const Uint64 UINT32_BIT_MAX     = 32;
const Uint64 UINT32_HEX_MAX     = 8;
const Uint64 UINT32_DEC_MAX     = 10;
const Uint64 UINT32_CSD_MAX     = 13;

const Uint64 UINT64_VAL_MAX     = U64(18446744073709551615);
const Uint64 UINT64_VAL_MIN     = 0;
const Uint64 UINT64_BIT_MAX     = 64;
const Uint64 UINT64_HEX_MAX     = 16;
const Uint64 UINT64_DEC_MAX     = 20;
const Uint64 UINT64_CSD_MAX     = 26;

const Int8 INT8_VAL_MAX         = 127;
const Int8 INT8_VAL_MIN         = -INT8_VAL_MAX - 1;
const Int8 INT8_BIT_MAX         = 8;
const Int8 INT8_HEX_MAX         = 2;
const Int8 INT8_DEC_MAX         = 4;
const Int8 INT8_CSD_MAX         = 4;

const Int16 INT16_VAL_MAX       = 32767;
const Int16 INT16_VAL_MIN       = -INT16_VAL_MAX - 1;
const Int16 INT16_BIT_MAX       = 16;
const Int16 INT16_HEX_MAX       = 4;
const Int16 INT16_DEC_MAX       = 6;
const Int16 INT16_CSD_MAX       = 7;

const Int32 INT32_VAL_MAX       = I32(2147483647);
const Int32 INT32_VAL_MIN       = -INT32_VAL_MAX - 1;
const Int32 INT32_BIT_MAX       = 32;
const Int32 INT32_HEX_MAX       = 8;
const Int32 INT32_DEC_MAX       = 11;
const Int32 INT32_CSD_MAX       = 14;

const Int64 INT64_VAL_MAX       = I64(9223372036854775807);
const Int64 INT64_VALMIN        = -INT64_VAL_MAX - 1;
const Int64 INT64_BIT_MAX       = 64;
const Int64 INT64_HEX_MAX       = 16;
const Int64 INT64_DEC_MAX       = 21;
const Int64 INT64_CSD_MAX       = 27;

typedef size_t  Size;

#if defined SCDU_M32

    const Size SIZE_VAL_MAX = UINT32_VAL_MAX;
    const Size SIZE_VAL_MIN = UINT32_VAL_MIN;
    const Size SIZE_BIT_MAX = UINT32_BIT_MAX;
    const Size SIZE_HEX_MAX = UINT32_HEX_MAX;
    const Size SIZE_DEC_MAX = UINT32_DEC_MAX;
    const Size SIZE_CSD_MAX = UINT32_CSD_MAX;

    #define SZ(val) val ## UL

    #define FSu(mod) "%" #mod "I32u"   // signed decimal
    #define FSd(mod) "%" #mod "I32d"   // unsigned decimal
    #define FSx(mod) "%" #mod "I32x"   // hex (lowercase)
    #define FSX(mod) "%" #mod "I32X"   // HEX (UPPERCASE)

#elif defined SCDU_M64

    const Size SIZE_VAL_MAX = UINT64_VAL_MAX;
    const Size SIZE_VAL_MIN = UINT64_VAL_MIN;
    const Size SIZE_BIT_MAX = UINT64_BIT_MAX;
    const Size SIZE_HEX_MAX = UINT64_HEX_MAX;
    const Size SIZE_DEC_MAX = UINT64_DEC_MAX;
    const Size SIZE_CSD_MAX = UINT64_CSD_MAX;

    #define SZ(val) val ## ULL

    #define FSu(mod) "%" #mod "I64u"   // signed decimal
    #define FSd(mod) "%" #mod "I64d"   // unsigned decimal
    #define FSx(mod) "%" #mod "I64x"   // hex (lowercase)
    #define FSX(mod) "%" #mod "I64X"   // HEX (UPPERCASE)

#else

   #error "target CPU data width not known"

#endif

const char* const NEWLINE_WIN = "\r\n";
const char* const NEWLINE_NIX = "\n";

#if defined SCDU_OS_WINDOWS
    const char* const NEWLINE_DEF = NEWLINE_WIN;
#else
    const char* const NEWLINE_DEF = NEWLINE_NIX;
#endif

#if !defined SCDU_VERSION
    #error "missing compiler option: -D SCDU_VERSION="
#endif

#if !defined SCDU_DATETIME
    #error "missing compiler option: -D SCDU_DATETIME="
#endif

const Size SCDU_DATE_SIZE = 10;
const Size SCDU_YEAR_SIZE = 4;
const Size SCDU_COPYRIGHT_MAX = 40;
const Size SCDU_NOTICE_MAX = 78;

extern const char* const scduDate;
extern const char* const scduYear;
extern const char* const scduCopyright;
extern const char* const scduNotice;

extern const char* const SCDU_ORIG_YEAR;
extern const char* const SCDU_HOLDER;
extern const char* const SCDU_LEGAL;
extern const char* const SCDU_BUGS;

extern const char* const SCDU_AUTHORS[];
extern const Size SCDU_AUTHORS_SIZE;

extern const char* const SCDU_CONTRIBS[];
extern const Size SCDU_CONTRIBS_SIZE;

extern const char* const SCDU_ACKS[];
extern const Size SCDU_ACKS_SIZE;

extern const char* const SCDU_TERMS[];
extern const Size SCDU_TERMS_SIZE;

typedef clock_t Clock;

class Timer
{
public:
    Timer();
    void reset();
    double read() const;

private:
    Clock mStartTime;
};

// use UPATH_MAX (Universal Path) to distinguish
// from OS-specific MAX_PATH or PATH_MAX

const Size UPATH_MAX = 2048;

typedef FILE File;

extern void initPlatform();
extern bool platformInitialised();
extern File* fileOpen(const char* path, const char* mode);
extern int fileClose(File* stream);
extern int fileFlush(File* stream);
extern Size fileRead(void* ptr, Size size, Size count, File* stream);
extern Size fileWrite(const void* ptr, Size size, Size count, File* stream);
extern int fileSeek( File* stream, Int64 offset, int origin);
extern Int64 fileTell (File* stream);
extern int fileDesc(File *stream);
extern Size fileBufCap(File *stream);
extern Size fileBufLen(File *stream);
extern const char* fileGetS(char* s, Size max, File *stream);
extern int setMode(int fd, int mode);
extern int setDir(const char* path);
extern const char* getDir();
extern bool isConsole(int fd);
extern Size mallocSize(void* ptr);
extern void milliSleep(Size milliseconds);
extern Uint64 strtoUint64(const char* str, char** endptr, int base);

// EOF