// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include channels.h separately - use core.h instead!"
#endif

enum ChanNum
{
    CH_NONE = -1,
    CH_C = 0,
    CH_S,
    CH_A,
    CH_R,
    CH_P,
    CH_I,
    CH_W,
    CH_E,
    CH_V,
    CH_D,
    CH_T,
    CH_COUNT
};

struct ChanDef
{
    const ChanNum   num;
    const char*     sym;
    const char*     name;
    const char*     desc;
};

struct Channels
{
    void clrAll() { *this = Channels(); }
    void setAll() { C=1; S=1; A=1; R=1; P=1; I=1; W=1; E=1; V=1; D=1; }

    bool C : 1; // command
    bool S : 1; // summary
    bool A : 1; // action
    bool R : 1; // results
    bool P : 1; // progress
    bool I : 1; // info
    bool W : 1; // warning
    bool E : 1; // error
    bool V : 1; // verbose
    bool D : 1; // debug
};

enum ProgressStatus
{
    PS_INIT = 0,
    PS_NORMAL,
    PS_BYPASS,
    PS_FINAL
};

struct ProgressMetrics
{
    Int64 estimate;
    Int64 complete;
};

struct Progress
{
    ProgressStatus  status;
    QtyNum          unitQty;
    QtyNum          itemQty;
    QtyNum          hitsQty;
    Int64           hits;
    const char*     snip;

    struct
    {
        ProgressMetrics units;
        ProgressMetrics items;

    } overall;

    struct
    {
        ProgressMetrics units;

    } current;
};

extern const ChanDef chanDefs[CH_COUNT];

extern void openChannels();
extern void closeChannels();
extern bool channelsOpened();

extern void stdFlush();
extern void dgnFlush();
extern void logFlush();

// Use GCC format attribute to enable compile-time checking of oufX arguments

extern void outC();
extern void outC(const char* s);
extern void ouvC(const char* fmt, va_list args);
extern void oufC(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outS();
extern void outS(const char* s);
extern void ouvS(const char* fmt, va_list args);
extern void oufS(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outA();
extern void outA(const char* s);
extern void ouvA(const char* fmt, va_list args);
extern void oufA(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outR();
extern void outR(const char* s);
extern void ouvR(const char* fmt, va_list args);
extern void oufR(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outP();
extern void outP(Progress& p);

extern void outI();
extern void outI(const char* s);
extern void ouvI(const char* fmt, va_list args);
extern void oufI(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outW();
extern void outW(const char* s);
extern void ouvW(const char* fmt, va_list args);
extern void oufW(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outE();
extern void outE(const char* s);
extern void ouvE(const char* fmt, va_list args);
extern void oufE(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

extern void outV();
extern void outV(const char* s);
extern void ouvV(const char* fmt, va_list args);
extern void oufV(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

#if defined SCDU_MODE_DEBUG

    extern void outD_();
    extern void outD_(const char* s);
    extern void ouvD_(const char* fmt, va_list args);
    extern void oufD_(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

    extern void outT_();
    extern void outT_(const char* s);
    extern void ouvT_(const char* fmt, va_list args);
    extern void oufT_(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

    #define outD(...) (outD_(__VA_ARGS__))
    #define outT(...) (outT_(__VA_ARGS__))

#else

    #define outD(...) ((void) 0)

#endif

// EOF