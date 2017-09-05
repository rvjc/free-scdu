// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include quantity.h separately - use core.h instead!"
#endif

const Size QTY_FLG_MAX = 10;
const Size QTY_TEA_MAX = 80;
const Size QTY_TEU_MAX = 80;
const Size QTY_PAA_MAX = UPATH_MAX;
const Size QTY_PAU_MAX = UPATH_MAX;
const Size QTY_PSW_MAX = 32;
const Size QTY_HEX_MAX = 18;
const Size QTY_HMS_MAX = 9;
const Size QTY_P5V_MAX = 7;
const Size QTY_DEC_MAX = INT64_DEC_MAX;
const Size QTY_DCS_MAX = INT64_CSD_MAX;
const Size QTY_D12_MAX = 5;
const Size QTY_D5V_MAX = 6;
const Size QTY_E33_MAX = 11;
const Size QTY_UPD_MAX = 10;
const Size QTY_UPB_MAX = 11;

const Size QTY_NUM_MAX = QTY_DCS_MAX;
const Size QTY_TXT_MAX = UPATH_MAX;
const Size QTY_UNIT_MAX = 16;

const Uint64 QTY_K = U64(1000);
const Uint64 QTY_M = QTY_K * U64(1000);
const Uint64 QTY_G = QTY_M * U64(1000);
const Uint64 QTY_T = QTY_G * U64(1000);
const Uint64 QTY_P = QTY_T * U64(1000);
const Uint64 QTY_E = QTY_P * U64(1000);

const Uint64 QTY_KI = U64(1024);
const Uint64 QTY_MI = QTY_KI * U64(1024);
const Uint64 QTY_GI = QTY_MI * U64(1024);
const Uint64 QTY_TI = QTY_GI * U64(1024);
const Uint64 QTY_PI = QTY_TI * U64(1024);
const Uint64 QTY_EI = QTY_PI * U64(1024);

enum QtyNum
{
    QN_NONE = -1,
    QN_FLAG_E = 0,
    QN_FLAG_O,
    QN_FLAG_T,
    QN_FLAG_Y,
    QN_TEXT,
    QN_TEXT_A,
    QN_TEXT_U,
    QN_PATH,
    QN_PATH_A,
    QN_PATH_U,
    QN_URL,
    QN_PSW,
    QN_PCK,
    QN_DEC,
    QN_HEX,
    QN_HMS,
    QN_P5V,
    QN_BITS,
    QN_BITS_B,
    QN_BITS_D,
    QN_BYTES,
    QN_BYTES_B,
    QN_BYTES_D,
    QN_BLOCKS,
    QN_BLOCKS_B,
    QN_BLOCKS_D,
    QN_PAGES,
    QN_PAGES_B,
    QN_PAGES_D,
    QN_CHUNKS,
    QN_CHUNKS_B,
    QN_CHUNKS_D,
    QN_SECS,
    QN_SECS_5V,
    QN_MSECS,
    QN_USPB,
    QN_CHARS,
    QN_FILES,
    QN_DIRS,
    QN_MATCHES,
    QN_CONFLICTS,
    QN_ERRORS,
    QN_COUNT
};

enum QtyFmt
{
    QF_NONE = -1,
    QF_FLG = 0,     // flag: 1 = true; 0 = false
    QF_TEA,         // text: ASCII encoding
    QF_TEU,         // text: UTF-8 encoding
    QF_TEX,         // text: ASCII or UTF-8 subject to -at option
    QF_PAA,         // path: ASCII encoding
    QF_PAU,         // path: UTF-8 encoding
    QF_PAX,         // path: ASCII or UTF-8 subject to -ap option
    QF_URL,         // Uniform Resource Location
    QF_PSW,         // password (always ASCII)
    QF_PCK,         // pick (enumerated for consistency only)
    QF_HEX,         // hex: type width: 0x0d 0x0ffe 0x00781e34
    QF_HMS,         // hours/minutes/seconds: 00:13:45
    QF_P5V,         // percent: 5.V fit (V=1,2,3): +/- 0.000% to 999.9%
    QF_DEC,         // decimal: natural: 1234 -12.3 123.456789
    QF_DCS,         // decimal: comma-separated: 1,234,567 -1,234,567
    QF_D12,         // decimal: 1.2 fit: 0.12 -1.23 9.99
    QF_D5V,         // decimal: 5.V fit (V=1,2,3): +/- 0.000 to 999.9
    QF_E33,         // exponent form: x.3e+3 fit: +1.234e10 -1.234e-10
    QF_UPD,         // unit prefix: decimal: 1.234 k  1.234 M
    QF_UPB,         // unit prefix: binary:  1.234 Ki 1.234 Mi
    QF_UPX,         // unit prefix: decimal or binary subject to -bp option

};

enum QtyRate
{
    QR_NONE = -1,
    QR_ANY = 0, // any rate subject to -rm option
    QR_SEC,     // per second
    QR_MIN,     // per minute
    QR_HOUR     // per day
};

struct QtyDef
{
    QtyNum      num;
    const char* one;
    const char* alt;
    const char* sym;
    QtyFmt      fmt;
    QtyRate     rate;
};

extern const QtyDef qtyDefs[QN_COUNT];

// EOF