// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include type.h separately - use core.h instead!"
#endif

typedef bool    Flag;
typedef Str     Text;
typedef Int64   Inum;
typedef double  Fnum;

class Pick
{
public:
    Pick();
    bool isNull() const;
    int get(char chr) const;
    int set(bool val, const char* str);
    union
    {
        Uint64 all;
        struct
        {
            // portability note:
            // GCC/G++ always orders bit fields from LSB to MSB
            // regardless of target endianness

            bool a:1, b:1, c:1, d:1, e:1, f:1, g:1, h:1, i:1, j:1, k:1, l:1, m:1;
            bool n:1, o:1, p:1, q:1, r:1, s:1, t:1, u:1, v:1, w:1, x:1, y:1, z:1;
            bool A:1, B:1, C:1, D:1, E:1, F:1, G:1, H:1, I:1, J:1, K:1, L:1, M:1;
            bool N:1, O:1, P:1, Q:1, R:1, S:1, T:1, U:1, V:1, W:1, X:1, Y:1, Z:1;
        };
    };
};

enum TypNum
{
    TYP_NONE = -1,
    TYP_FLAG = 0,
    TYP_TEXT,
    TYP_INUM,
    TYP_FNUM,
    TYP_PICK,
    TYP_COUNT
};

struct TypDef
{
    const TypNum    typnum;
    const char*     name;       // descriptive option type name
    const char*     desc;       // brief help description
};

const Size PICK_CHARS_MAX = 52;

extern const TypDef typDefs[TYP_COUNT];

// EOF