// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include "core.h"

const QtyDef qtyDefs[] =
{
    { QN_FLAG_E,     "enabled",      "disabled",     "",     QF_FLG,  QR_NONE },
    { QN_FLAG_O,     "on",           "off",          "",     QF_FLG,  QR_NONE },
    { QN_FLAG_T,     "true",         "false",        "",     QF_FLG,  QR_NONE },
    { QN_FLAG_Y,     "yes",          "no",           "",     QF_FLG,  QR_NONE },

    { QN_TEXT,       "",             "",             "",     QF_TEX,  QR_NONE },
    { QN_TEXT_A,     "",             "",             "",     QF_TEA,  QR_NONE },
    { QN_TEXT_U,     "",             "",             "",     QF_TEU,  QR_NONE },

    { QN_PATH,       "",             "",             "",     QF_PAX,  QR_NONE },
    { QN_PATH_A,     "",             "",             "",     QF_PAA,  QR_NONE },
    { QN_PATH_U,     "",             "",             "",     QF_PAU,  QR_NONE },

    { QN_URL,        "",             "",             "",     QF_URL,  QR_NONE },
    { QN_PSW,        "",             "",             "",     QF_PSW,  QR_NONE },
    { QN_PCK,        "",             "",             "",     QF_PCK,  QR_NONE },

    { QN_DEC,        "",             "",             "",     QF_DEC,  QR_NONE },
    { QN_HEX,        "",             "",             "",     QF_HEX,  QR_NONE },
    { QN_HMS,        "",             "",             "",     QF_HMS,  QR_NONE },
    { QN_P5V,        "",             "",             "",     QF_P5V,  QR_NONE },

    { QN_BITS,       "bit",          "bits",         "b",    QF_UPX,  QR_ANY  },
    { QN_BITS_B,     "bit",          "bits",         "b",    QF_UPB,  QR_SEC  },
    { QN_BITS_D,     "bit",          "bits",         "b",    QF_UPD,  QR_MIN  },

    { QN_BYTES,      "byte",         "bytes",        "B",    QF_UPX,  QR_ANY  },
    { QN_BYTES_B,    "byte",         "bytes",        "B",    QF_UPB,  QR_SEC  },
    { QN_BYTES_D,    "byte",         "bytes",        "B",    QF_UPD,  QR_MIN  },

    { QN_BLOCKS,     "block",        "blocks",       "",     QF_UPX,  QR_ANY  },
    { QN_BLOCKS_B,   "block",        "blocks",       "",     QF_UPB,  QR_SEC  },
    { QN_BLOCKS_D,   "block",        "blocks",       "",     QF_UPD,  QR_MIN  },

    { QN_PAGES,      "page",         "pages",        "",     QF_UPX,  QR_ANY  },
    { QN_PAGES_B,    "page",         "pages",        "",     QF_UPB,  QR_SEC  },
    { QN_PAGES_D,    "page",         "pages",        "",     QF_UPD,  QR_MIN  },

    { QN_CHUNKS,     "chunk",        "chunks",       "",     QF_UPX,  QR_ANY  },
    { QN_CHUNKS_B,   "chunk",        "chunks",       "",     QF_UPB,  QR_SEC  },
    { QN_CHUNKS_D,   "chunk",        "chunks",       "",     QF_UPD,  QR_MIN  },

    { QN_SECS,       "second",       "seconds",      "s",    QF_DEC,  QR_NONE },
    { QN_SECS_5V,    "second",       "seconds",      "s",    QF_D5V,  QR_NONE },

    { QN_MSECS,      "millisecond",  "milliseconds", "ms",   QF_DEC,  QR_NONE },
    { QN_USPB,       "usec/byte",    "usecs/byte",   "",     QF_DEC,  QR_NONE },

    { QN_CHARS,      "character",    "characters",   "",     QF_DEC,  QR_ANY  },

    { QN_FILES,      "file",         "files",        "",     QF_DCS,  QR_ANY  },
    { QN_DIRS,       "directory",    "directories",  "",     QF_DCS,  QR_ANY  },
    { QN_MATCHES,    "match",        "matches",      "",     QF_DCS,  QR_ANY  },
    { QN_CONFLICTS,  "conflict",     "conflicts",    "",     QF_DCS,  QR_ANY  },
    { QN_ERRORS,     "error",        "errors",       "",     QF_DCS,  QR_ANY  }
};

// EOF