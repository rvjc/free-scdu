// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
#error "do not include error.h separately - use core.h instead!"
#endif

const Size XEM_MAX = 80;
const int  EXIT_BASE = 64;

#define PANIC(cause) panic(CUR_FUNC, CUR_FILE, CUR_LINE, cause)

#if defined SCDU_MODE_DEBUG
    #define ASSERT(e) ((e) ? (void) 0 : PANIC("ASSERT(" #e ")"))
#else
    #define ASSERT(e) ((void) 0)
#endif

#define ASSERT_ALWAYS(e) ((e) ? (void) 0 : PANIC("ASSERT_ALWAYS(" #e ")"))

enum Xen
{
    XE_OK = 0,
    XE_UNKNOWN,
    XE_ABTERM,
    XE_UNKSIG,
    XE_USRINT,
    XE_MEMOUT,
    XE_STREAM,
    XE_CMD,
    XE_CMDPRM,
    XE_COUNT
};

struct XerDef
{
    Xen         num;
    const char* sym;
    const char* msg;
    const char* params;
};

extern const Xen& xen;
extern const XerDef xerDefs[XE_COUNT];

extern void xer(Xen e, ...);
extern void panic(const char* func, const char* file, int line, const char* msg);
extern int exitCode(int e);

// EOF