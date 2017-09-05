// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include format.h separately - use core.h instead!"
#endif

const Size FMT_NUM_MAX = QTY_NUM_MAX + QTY_UNIT_MAX + 6;
const Size FMT_TXT_MAX = QTY_TXT_MAX;
const Size FMT_MAX = FMT_TXT_MAX;
const Size FMT_RATE_MAX = FMT_NUM_MAX + 2;

enum FmtSpecMask
{
    FSM_BARE     = 0x1000,
    FSM_FRIENDLY = 0x2000,
    FSM_REG      = 0x4000,
    FSM_PADDING  = 0x00ff,
    FSM_LEFT     = 0x0080,
    FSM_RIGHT    = 0x0040,
    FSM_SPACE    = 0x0000,
    FSM_ZERO     = 0x0001,
    FSM_PLUS     = 0x0002
};

enum FmtSpec
{
    FS_AUTO = 0,

    FS_B     = FSM_BARE,
    FS_F     = FSM_FRIENDLY,
    FS_R     = FSM_REG,
    FS_BF    = FS_B | FS_F,
    FS_BR    = FS_B | FS_R,
    FS_FR    = FS_F | FS_R,
    FS_BFR   = FS_B | FS_F | FS_R,

    FS_PL    = FSM_LEFT,
    FS_PR    = FSM_RIGHT,
    FS_PZ    = FSM_LEFT | FSM_ZERO,
    FS_PP    = FSM_LEFT | FSM_PLUS,

    FS_PL_B  = FS_PL | FS_B,
    FS_PR_B  = FS_PR | FS_B,
    FS_PZ_B  = FS_PZ | FS_B,
    FS_PP_B  = FS_PP | FS_B,

    FS_PL_F  = FS_PL | FS_F,
    FS_PR_F  = FS_PR | FS_F,
    FS_PZ_F  = FS_PZ | FS_F,
    FS_PP_F  = FS_PP | FS_F,

    FS_PL_R  = FS_PL | FS_R,
    FS_PR_R  = FS_PR | FS_R,
    FS_PZ_R  = FS_PZ | FS_R,
    FS_PP_R  = FS_PP | FS_R,

    FS_PL_BF = FS_PL | FS_BF,
    FS_PR_BF = FS_PR | FS_BF,
    FS_PZ_BF = FS_PZ | FS_BF,
    FS_PP_BF = FS_PP | FS_BF,

    FS_PL_BR = FS_PL | FS_BR,
    FS_PR_BR = FS_PR | FS_BR,
    FS_PZ_BR = FS_PZ | FS_BR,
    FS_PP_BR = FS_PP | FS_BR,

    FS_PL_FR = FS_PL | FS_FR,
    FS_PR_FR = FS_PR | FS_FR,
    FS_PZ_FR = FS_PZ | FS_FR,
    FS_PP_FR = FS_PP | FS_FR,

    FS_PL_BFR = FS_PL | FS_BFR,
    FS_PR_BFR = FS_PR | FS_BFR,
    FS_PZ_BFR = FS_PZ | FS_BFR,
    FS_PP_BFR = FS_PP | FS_BFR
};

extern int format(char* s, Size max, FmtSpec spec, QtyNum qtynum, bool val);
extern int format(char* s, Size max, FmtSpec spec, QtyNum qtynum, const char* val);
extern int format(char* s, Size max, FmtSpec spec, QtyNum qtynum, Int64 val);
extern int format(char* s, Size max, FmtSpec spec, QtyNum qtynum, double val);

extern int formatRate(char* s, Size max, FmtSpec spec, QtyNum qtynum, double val);

extern int formatFlag(char* s, Size max, FmtSpec spec, const Var* var, const Flag& flag);
extern int formatText(char* s, Size max, FmtSpec spec, const Var* var, const Text& text);
extern int formatInum(char* s, Size max, FmtSpec spec, const Var* var, const Inum& inum);
extern int formatFnum(char* s, Size max, FmtSpec spec, const Var* var, const Fnum& fnum);
extern int formatPick(char* s, Size max, FmtSpec spec, const Var* var, const Pick& pick);

// EOF