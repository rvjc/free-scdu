// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include parse.h separately - use core.h instead!"
#endif

const Size PARSE_MAX = 20;
const Size PEM_MAX   = 80;

enum Pen
{
    PE_OK = 0,
    PE_NULPTR,
    PE_NUL,
    PE_FMT,
    PE_RANGE,
    PE_WILDBAD,
    PE_SELBAD,
    PE_PICKBAD,
    PE_PICKDUP,
    PE_PICKMORE,
    PE_PICKLESS,
    PE_CHARMORE,
    PE_CHARLESS,
    PE_LIST,
    PE_LISTLESS,
    PE_COUNT
};

struct PerDef
{
    Pen         num;
    const char* sym;
    const char* msg;
    const char* params;
};

extern const Pen&   pen;
extern const char*  const pem;
extern const PerDef perDefs[PE_COUNT];

extern void perClear();

extern void parse(const char* s, QtyNum qtynum, bool& val);
extern void parse(const char* s, QtyNum qtynum, double& val);
extern void parse(const char* s, QtyNum qtynum, Uint64& val);
extern void parse(const char* s, QtyNum qtynum, Int64& val);
extern void parse(const char* s, QtyNum qtynum, Uint32& val);
extern void parse(const char* s, QtyNum qtynum, Int32& val);

extern void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, bool val[], Size& count);
extern void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, double val[], Size& count);
extern void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Uint64 val[], Size& count);
extern void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Int64 val[], Size& count);
extern void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Uint32 val[], Size& count);
extern void parseList(const char* s, QtyNum qtynum, const char* dlm, Size max, Int32 val[], Size& count);

extern void parseFlag(const char* s, const Var* var, Flag& val);
extern void parseText(const char* s, const Var* var, Text& val);
extern void parseInum(const char* s, const Var* var, Inum& val);
extern void parseFnum(const char* s, const Var* var, Fnum& val);
extern void parsePick(const char* s, const Var* var, Pick& val);

// EOF