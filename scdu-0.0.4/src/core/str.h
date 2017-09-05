// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include str.h separately - use core.h instead!"
#endif

// optimally pack string in 32 bytes regardless of target
// this means the internal string buffer size is:
// 24 bytes (23 chars) in a typical 32-bit target.
// 20 bytes (19 chars) in a typical 64-bit target.

const Size STRBUF_SIZE = 32 - 2 * sizeof(Uint16) - sizeof(char*);

class Str
{
public:
    Str();
    ~Str();
    Str(const Str& str);
    Str(const char* s);
    Str(const char c);
    Str& operator=(const Str& rhs);
    Str& operator=(const char* rhs);
    Str& operator=(const char rhs);
    Str& operator+=(const Str& rhs);
    Str& operator+=(const char* rhs);
    Str& operator+=(const char rhs);
    bool operator==(const Str& rhs) const;
    bool operator==(const char* rhs) const;
    bool operator==(const char rhs) const;
    Size len() const;
    const char* cb() const;
    const char* ce() const;
    int cmp(const Str& str) const;
    int cmp(const char* s) const;

    friend Str operator+(const Str& lhs, const Str& rhs);
    friend Str operator+(const Str& lhs, const char* rhs);
    friend Str operator+(const char* lhs, const Str& rhs);
    friend Str operator+(const Str& lhs, const char rhs);
    friend Str operator+(const char lhs, const Str& rhs);

private:
    char    mBuf[STRBUF_SIZE];          // (see STRBUF_SIZE definition!!!)
    Uint16  mLen;                       // 2 bytes
    Uint16  mCap;                       // 2 bytes
    char*   mC;                         // target dependent
};

extern Str operator+(const Str& lhs, const Str& rhs);
extern Str operator+(const Str& lhs, const char* rhs);
extern Str operator+(const char* lhs, const Str& rhs);
extern Str operator+(const Str& lhs, const char rhs);
extern Str operator+(const char lhs, const Str& rhs);

extern bool strlenz(const char* s);
extern int strleni(const char* s);

extern char* strncpyz(char* dst, const char* src, Size n);
extern char* strncatz(char* dst, const char* src, Size n);
extern int snprintfz(char* s, Size n, const char* fmt, ... ) __attribute__((format(printf, 3, 4)));
extern int vsnprintfz(char* s, Size n, const char* fmt, va_list args);

extern bool strIsDec(char* s);
extern void strToLower(char* s);
extern void strToUpper(char* s);
extern void strnToLower(char* s, Size max);
extern void strnToUpper(char* s, Size max);

extern const char* findDupChar(const char* s);
extern const char* findOddChar(const char* s, const char* range);

extern int fitz(char* dst, const char* src, Size w, int p);
extern char* trimWhite(char* s);

// EOF