// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>
#include <stdarg.h>

#include "core.h"

static OptNum optParse(const char* arg, Value& val);
static void cer(Cen e, ...);

static Cen  cenMutable = CE_OK;
const Cen&  cen = cenMutable;

static char cemMutable[CEM_MAX + 1] = "";
const char* const cem = cemMutable;

const CerDef cerDefs[] =
{
    { CE_OK,        "CE_OK",        "",                                 ""          },
    { CE_ACTEXP,    "CE_ACTEXP",    "action expected",                  "%s"        },
    { CE_ACTUNK,    "CE_ACTUNK",    "unknown action",                   "%s"        },
    { CE_OPTEXPS,   "CE_OPTEXPS",   "short option expected",            "%s"        },
    { CE_OPTEXPL,   "CE_OPTEXPL",   "long option expected",             "%s"        },
    { CE_OPTEXPM,   "CE_OPTEXPM",   "multi-option expected",            "%s"        },
    { CE_OPTUNKS,   "CE_OPTUNKS",   "unknown short option",             "%s"        },
    { CE_OPTUNKL,   "CE_OPTUNKL",   "unknown long option",              "%s"        },
    { CE_OPTUNKM,   "CE_OPTUNKM",   "unknown multi-option",             "%s(%c)"    },
    { CE_OPTRQD,    "CE_OPTRQD",    "option requires value",            "%s"        },
    { CE_OPTVAL,    "CE_OPTVAL",    "option value",                     "%s : %s"   },
    { CE_PRMMORE,   "CE_PRMMORE",   "too few parameters",               ""          },
    { CE_PRMLESS,   "CE_PRMLESS",   "too many parameters",              ""          },
    { CE_PRMOPT,    "CE_PRMOPT",    "option found in parameter list",   "%s"        },
    { CE_CFGOPEN,   "CE_CFGOPEN",   "cannot open config file",          "%s"        },
    { CE_CFGFILE,   "CE_CFGFILE",   "config file",                      "%s:%d: %s" },
    { CE_CFGOPT,    "CE_CFGOPT",    "option disallowed in config file", "%s:%d: %s" },
    { CE_CFGENT,    "CE_CFGENT",    "invalid config file entry",        "%s:%d: %s" },
    { CE_SETWD,     "CE_SETWD",     "unable to set work directory",     "%s"        }
};

const Cmd cmd;

const EnvDef envDefs[] =
{
    {   ENV_CS,     "chunk-size",       { TYP_INUM, QN_BYTES,   "", "", ""      }   },
    {   ENV_CST,    "console-streams",  { TYP_PICK, QN_PCK,     "", "", "sd"    }   },
    {   ENV_WD,     "work-directory",   { TYP_TEXT, QN_PATH,    "", "", ""      }   }
};

const ActDef actDefs[] =
{
    {   ACT_COPY, "copy", 2, 2, "<source> <destination>",
        "copies source files or directories to destination",
        "-cf=scdu.cfg -bs=100 myfile.dat mycopy.dat"                },

    {   ACT_HELP, "help", 0, 1, "[ <action> ]",
        "provides general or specific help",
        "show"                                                      },

    {   ACT_INFO, "info", 1, 1, "<source>",
        "provides useful information about the source",
        "-rs=R -rd= 192.168.1.1"                                    },

    {   ACT_SHOW, "show", 1, 1, "acks | authors | contribs | copyright | terms | version",
        "outputs program information",
        "version"                                                   },

    {   ACT_VIEW, "view", 1, 1, "<source>",
        "displays source in human-readable format",
        "myfile.dat"                                                }
};

const OptDef optDefs[] =
{
    {   OPT_A,  "a", "all", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "general-purpose flag e.g. process all files"               },

    {   OPT_F,  "f", "force", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "general purpose flag e.g. force over-writing"              },

    {   OPT_P,  "p", "preserve", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "general purpose flag e.g. preserve attributes"             },

    {   OPT_R,  "r", "recurse", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "general purpose flag e.g. include sub-directories"         },

    {   OPT_AP, "ap", "ascii-path", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "for generic paths, use ascii instead of utf-8 encoding"    },

    {   OPT_AT, "at", "ascii-text", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "for generic text, use ascii instead of utf-8 encoding"     },

    {   OPT_BP, "bp", "binary-prefix", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "for generic quantities, use Ki(1024) instead of k(1000)"   },

    {   OPT_BS, "bs", "buffer-size", "1Ki",
        { TYP_INUM, QN_CHUNKS, "1", "10Mi", "" },
        "file buffer size, memory permitting (see -cs option)"      },

    {   OPT_CF, "cf", "config-file", "",
        { TYP_TEXT, QN_PATH, "", "", "" },
        "alternative means of specifying command-line options"      },

    {   OPT_CS, "cs", "chunk-size", "0",
        { TYP_INUM, QN_BYTES, "512", "10Mi", "0" },
        "LCM of page sizes of accessible file systems (0 = auto)"   },

    {   OPT_FD, "fd", "flush-delay", "50",
        { TYP_INUM, QN_MSECS, "1", "100", "" },
        "minimum catch-up time for slow stream flush"               },

    {   OPT_FF, "ff", "flush-factor", "10",
        { TYP_INUM, QN_USPB, "1", "1000", "" },
        "additional catch-up time for slow stream flush"            },

    {   OPT_FL, "fl", "flush-limit", "1000",
        { TYP_INUM, QN_MSECS, "100", "10000", "" },
        "maximum catch-up time for slow stream flush"               },

    {   OPT_FST, "fst", "fast-streams", "",
        { TYP_PICK, QN_PCK, "", "", "sdl" },
        "streams which do not require catch-up time after flush"    },

    {   OPT_LF, "lf", "log-file", "scdu.log",
        { TYP_TEXT, QN_PATH, "1", "", "" },
        "destination of logged channels (see -rl and -lm options)"  },

    {   OPT_LM, "lm", "log-mode", "",
        { TYP_PICK, QN_PCK, "", "1", "AO" },
        "<null> = disabled; A = append; O = overwrite;"             },

    {   OPT_NS, "ns", "newline-std", "D",
        { TYP_PICK, QN_PCK, "1", "1", "DWN" },
        "Default; Windows(CRLF); Nix(LF)"                           },

    {   OPT_ND, "nd", "newline-dgn", "D",
        { TYP_PICK, QN_PCK, "1", "1", "DWN" },
        "Default; Windows(CRLF); Nix(LF)"                           },

    {   OPT_NL, "nl", "newline-log", "D",
        { TYP_PICK, QN_PCK, "1", "1", "DWN" },
        "Default; Windows(CRLF); Nix(LF)"                           },

    {   OPT_PF, "pf", "progress-feed", "IF",
        { TYP_PICK, QN_PCK, "", "", "IUF" },
        "<null> = never; Interrupts; Updates; Final"                },

    {   OPT_PR, "pr", "progress-rate", "0.25",
        { TYP_FNUM, QN_SECS_5V, "0.1", "300.0", "" },
        "minimum time between progress updates"                     },

    {   OPT_PS, "ps", "progress-stats", "PHIS",
        { TYP_PICK, QN_PCK, "", "", "PTURHIS" },
        "Percent; Time; Units; Rate; Hits; Item; Snip"              },

    {   OPT_PW, "pw", "progress-width", "80",
        { TYP_INUM, QN_CHARS, "40", "240", "" },
        "truncation width of progress updates"                      },

    {   OPT_RS, "rs", "route-std", "R",
        { TYP_PICK, QN_PCK, "", "", "CSARPIWEVDT" },
        "channels routed to standard output (stdout)"               },

    {   OPT_RD, "rd", "route-dgn", "*-R",
        { TYP_PICK, QN_PCK, "", "", "CSARPIWEVDT" },
        "channels routed to diagnostic output (stderr)"             },

    {   OPT_RL, "rl", "route-log", "*-R",
        { TYP_PICK, QN_PCK, "", "", "CSARPIWEVDT" },
        "channels routed to log output (see -lf and -lm options)"   },

    {   OPT_RM, "rm", "rate-metric", "M",
        { TYP_PICK, QN_PCK, "1", "1", "SMH" },
        "generic rate output: per Second; Minute; Hour"             },

    {   OPT_RR, "rr", "raw-reporting", "",
        { TYP_FLAG, QN_FLAG_E, "", "", "" },
        "show bare data in results and summary output"              },

    {   OPT_SS, "ss", "summary-stats", "",
        { TYP_PICK, QN_PCK, "", "", "AD" },
        "Allocs; Duration;"                                         },

    {   OPT_WD, "wd", "work-directory", "",
        { TYP_TEXT, QN_PATH, "", "", "" },
        "alternative working directory to calling process"          }
};

Env::Env()
{
    chunkSize  = 0;
    consoleStreams.all = 0;
    workDirectory = "";
}

void Env::get(EnvNum envnum, Value& val) const
{
    const Var* var;

    ASSERT(envnum >= 0 && envnum < ENV_COUNT);

    val.setNull();
    var = &envDefs[envnum].var;

    switch (envnum)
    {
        case ENV_CS:    val.setInum( (Inum)     chunkSize,      var);   break;
        case ENV_CST:   val.setPick(            consoleStreams, var);   break;
        case ENV_WD:    val.setText(            workDirectory,  var);   break;

        default: ASSERT(false);
    }
}

Action::Action()
{
    num = ACT_NONE;
}

void Action::set(ActNum n)
{
    ASSERT(n >= 0 && n < ACT_COUNT);

    num = n;
    name = actDefs[n].name;
}

Options::Options()
{
    OptNum  num;
    Value   val;

    for (int i = 0; i < OPT_COUNT; i++)
    {
        const OptDef& def = optDefs[i];

        num = OptNum(i);

        ASSERT(def.num == num);
        ASSERT(strlen(def.sym) <= OPT_SYM_MAX);
        ASSERT(strlen(def.name) <= OPT_NAME_MAX);

        if ( def.var.typnum == TYP_FLAG )
        {
            ASSERT(strlenz(def.dval));
            ASSERT(strlenz(def.var.min));
            ASSERT(strlenz(def.var.max));
            ASSERT(strlenz(def.var.sel));

            val.setFlag(false, &def.var);
        }
        else
        {
            val.parse(def.dval, &def.var);
            if ( pen ) PANIC(def.name);
        }

        set(num, val);
    }
}

void Options::set(OptNum optnum, const Value& val)
{
    ASSERT(optnum >= 0 && optnum < OPT_COUNT);

    switch (optnum)
    {
        case OPT_A:     all             =           val.flag();     break;
        case OPT_F:     force           =           val.flag();     break;
        case OPT_P:     preserve        =           val.flag();     break;
        case OPT_R:     recurse         =           val.flag();     break;
        case OPT_AP:    asciiPath       =           val.flag();     break;
        case OPT_AT:    asciiText       =           val.flag();     break;
        case OPT_BP:    binaryPrefix    =           val.flag();     break;
        case OPT_BS:    bufferSize      = (Size)    val.inum();     break;
        case OPT_CF:    configFile      =           val.text();     break;
        case OPT_CS:    chunkSize       = (Size)    val.inum();     break;
        case OPT_FD:    flushDelay      = (Size)    val.inum();     break;
        case OPT_FF:    flushFactor     = (Size)    val.inum();     break;
        case OPT_FL:    flushLimit      = (Size)    val.inum();     break;
        case OPT_FST:   fastStreams     =           val.pick();     break;
        case OPT_LF:    logFile         =           val.text();     break;
        case OPT_LM:    logMode         =           val.pick();     break;
        case OPT_NS:    newlineStd      =           val.pick();     break;
        case OPT_ND:    newlineDgn      =           val.pick();     break;
        case OPT_NL:    newlineLog      =           val.pick();     break;
        case OPT_PF:    progressFeed    =           val.pick();     break;
        case OPT_PR:    progressRate    =           val.fnum();     break;
        case OPT_PS:    progressStats   =           val.pick();     break;
        case OPT_PW:    progressWidth   = (Size)    val.inum();     break;
        case OPT_RS:    routeStd        =           val.pick();     break;
        case OPT_RD:    routeDgn        =           val.pick();     break;
        case OPT_RL:    routeLog        =           val.pick();     break;
        case OPT_RM:    rateMetric      =           val.pick();     break;
        case OPT_RR:    rawReporting    =           val.flag();     break;
        case OPT_SS:    summaryStats    =           val.pick();     break;
        case OPT_WD:    workDirectory   =           val.text();     break;

        default: ASSERT(false);
    }
}

void Options::get(OptNum optnum, Value& val) const
{
    const Var* var;

    ASSERT(optnum >= 0 && optnum < OPT_COUNT);

    val.setNull();
    var = &optDefs[optnum].var;

    switch (optnum)
    {
        case OPT_A:     val.setFlag(            all,            var);   break;
        case OPT_F:     val.setFlag(            force,          var);   break;
        case OPT_P:     val.setFlag(            preserve,       var);   break;
        case OPT_R:     val.setFlag(            recurse,        var);   break;
        case OPT_AP:    val.setFlag(            asciiPath,      var);   break;
        case OPT_AT:    val.setFlag(            asciiText,      var);   break;
        case OPT_BP:    val.setFlag(            binaryPrefix,   var);   break;
        case OPT_BS:    val.setInum( (Inum)     bufferSize,     var);   break;
        case OPT_CF:    val.setText(            configFile,     var);   break;
        case OPT_CS:    val.setInum( (Inum)     chunkSize,      var);   break;
        case OPT_FD:    val.setInum( (Inum)     flushDelay,     var);   break;
        case OPT_FF:    val.setInum( (Inum)     flushFactor,    var);   break;
        case OPT_FL:    val.setInum( (Inum)     flushLimit,     var);   break;
        case OPT_FST:   val.setPick(            fastStreams,    var);   break;
        case OPT_LF:    val.setText(            logFile,        var);   break;
        case OPT_LM:    val.setPick(            logMode,        var);   break;
        case OPT_NS:    val.setPick(            newlineStd,     var);   break;
        case OPT_ND:    val.setPick(            newlineDgn,     var);   break;
        case OPT_NL:    val.setPick(            newlineLog,     var);   break;
        case OPT_PF:    val.setPick(            progressFeed,   var);   break;
        case OPT_PR:    val.setFnum(            progressRate,   var);   break;
        case OPT_PS:    val.setPick(            progressStats,  var);   break;
        case OPT_PW:    val.setInum( (Inum)     progressWidth,  var);   break;
        case OPT_RS:    val.setPick(            routeStd,       var);   break;
        case OPT_RD:    val.setPick(            routeDgn,       var);   break;
        case OPT_RL:    val.setPick(            routeLog,       var);   break;
        case OPT_RM:    val.setPick(            rateMetric,     var);   break;
        case OPT_RR:    val.setFlag(            rawReporting,   var);   break;
        case OPT_SS:    val.setPick(            summaryStats,   var);   break;
        case OPT_WD:    val.setText(            workDirectory,  var);   break;

        default: ASSERT(false);
    }
}

Params::Params()
:   count(mCount)
{
    mCount = 0;
}

const Str& Params::operator[](const Size i) const
{
    ASSERT(i < mCount);

    return mItems[i];
}

void Params::add(const char* param)
{
    mItems[mCount] = param;
    mCount++;
}

Cmd::Cmd()
:   initialised(mInitialised),
    recap(mRecap),
    env(mEnv),
    action(mAction),
    options(mOptions),
    params(mParams)
{
    mInitialised = false;
}

void Cmd::init(int argc, char* argv[]) const
{
    int             i;
    const char*     arg;
    char            c;
    ActNum          actnum;
    OptNum          optnum;
    Value           val;

    if ( mInitialised ) PANIC("cmd object already initialised");

    cerClear();

    // note: program path is always the first argument

    ASSERT(argc >=1);
    i = 1;

    if ( argc > i )
    {
        arg = argv[i];
        ASSERT(!strlenz(arg));

        if ( strcmp(arg, "?") == 0 )
        {
            mRecap = true;
            i++;
        }
        else
        {
            mRecap = false;
        }
    }

    if ( i == argc )
    {
        actnum = ACT_HELP; // default
    }
    else
    {
        arg = argv[i++];
        actnum = actFind(arg);
        if ( actnum == ACT_NONE ) { cer(CE_ACTUNK, arg); return; }
    }

    mAction.set(actnum);

    for ( i = i; i < argc; i++ )
    {
        arg = argv[i];
        c = arg[0];

        if ( c != '-' && c != '+' ) break;

        optnum = optParse(arg, val);

        while ( true )
        {
            if ( cen ) return;
            if ( optnum == OPT_NONE ) break;

            mOptions.set(optnum, val);

            // some options are relevant during initialisation

            switch ( optnum )
            {
                case OPT_WD: setEnvWorkDirectory(true);     break;
                case OPT_CF: config();                      break;
                default: break;
            }

            if ( cen ) return;

            optnum = optParse(0, val);
        }
    }

    while ( i < argc )
    {
        arg = argv[i++];

        if ( arg[0] == '-' || arg[0] == '+' ) { cer(CE_PRMOPT, arg); return; }
        if ( mParams.count == PARAMS_MAX )    { cer(CE_PRMLESS);     return; }

        mParams.add(arg);
    }

    const ActDef& actdef = actDefs[actnum];

    if ( mParams.count < actdef.pmin ) { cer(CE_PRMMORE); return; }
    if ( mParams.count > actdef.pmax ) { cer(CE_PRMLESS); return; }

    setEnv();

    mInitialised = true;

    ASSERT(!cen);
}

void Cmd::config() const
{
    const char* path;
    File*       file;
    char        entry[CFG_ENTRY_MAX + 1];
    int         line;
    char*       arg;
    Size        n;
    char        c;
    OptNum      optnum;
    ActNum      actnum;
    Value       val;

    ASSERT(!cen);

    path = mOptions.configFile.cb();

    ASSERT(!mInitialised);
    ASSERT(!strlenz(path));

    file = fileOpen(path, "r");
    if ( file == 0 )
    {
        cer(CE_CFGOPEN, path);
        return;
    }

    actnum = ACT_NONE; // global section

    line = 0;

    while ( fileGetS(entry, CFG_ENTRY_MAX, file) != 0 )
    {
        line++;

        arg = trimWhite(entry);
        n = strlen(arg);

        if ( n == 0 ) continue;

        c = arg[0];

        switch ( c )
        {
            case '-':
            case '+':

                // ignore option if not global or specific to current action

                if ( actnum != ACT_NONE && actnum != mAction.num ) continue;

                optnum = optParse(arg, val);

                while ( true )
                {
                    if ( cen )
                    {
                        cer(CE_CFGFILE, path, line, cem);
                        fileClose(file);
                        return;
                    }

                    if ( optnum == OPT_NONE ) break;

                    if ( optnum == OPT_CF )
                    {
                        cer(CE_CFGOPT, path, line, arg);
                        fileClose(file);
                        return;
                    }

                    mOptions.set(optnum, val);

                    // some options are relevant during configuation

                    switch ( optnum )
                    {
                        case OPT_WD:    setEnvWorkDirectory(true); break;
                        default:        break;
                    }

                    optnum = optParse(0, val);
                }

                break;

            case '@':   // start of action-specific section

                if ( n == 1 )
                {
                    cer(CE_ACTEXP, arg);
                    cer(CE_CFGFILE, path, line, cem);
                    fileClose(file);
                    return;
                }

                actnum = actFind(arg + 1);

                if ( actnum == ACT_NONE )
                {
                    cer(CE_ACTUNK, arg);
                    cer(CE_CFGFILE, path, line, cem);
                    fileClose(file);
                    return;
                }

                break;

            case '#':   // comment

                break;

            default:

                cer(CE_CFGENT, path, line, arg);
                fileClose(file);
                return;
        }
    }

    fileClose(file);
    ASSERT(!cen);
}

void Cmd::setEnv() const
{
    setEnvChunkSize(false);
    setEnvConsoleStreams(false);
    setEnvWorkDirectory(false);
}

void Cmd::setEnvChunkSize(bool force) const
{
    Size cs = cmd.options.chunkSize;

    if ( !force && mEnv.chunkSize != 0 ) return;

    mEnv.chunkSize = cs == 0 ? 4096 : cs;

    // TO DO: auto configuration ( -cs=0 )
    // 4096 is a safe universal default for now
    // (it also happens to be the stdio file buffer size).
    // However, some flexibility may be required in
    // the future e.g. with SSD storage.
    // chunk size should be the lowest common multiple
    // of the page sizes of all accessible file systems
}

void Cmd::setEnvConsoleStreams(bool force) const
{
    ASSERT_ALWAYS(!force); // for future use

    ASSERT(mEnv.consoleStreams.all == 0);

    mEnv.consoleStreams.s   = isConsole(fileDesc(stdout));
    mEnv.consoleStreams.d   = isConsole(fileDesc(stderr));
}

void Cmd::setEnvWorkDirectory(bool force) const
{
    const char* wd = cmd.options.workDirectory.cb();

    if ( !force && mEnv.workDirectory.len() != 0 )
    {
        return;
    }

    // option path may be absolute or relative;

    if ( wd[0] != 0 )
    {
        if ( setDir(wd) < 0 )
        {
            cer(CE_SETWD, wd);
        }
        return;
    }

    // env path is absolute path returned by the system;

    mEnv.workDirectory = getDir();
}

void cerClear()
{
    cenMutable = CE_OK;
    cemMutable[0] = 0;
}

ActNum actFind(const char* name)
{
    ActNum n;

    for (int i = 0; i < ACT_COUNT; i++)
    {
        const ActDef& def = actDefs[i];
        n = ActNum(i);

        if ( strcmp(name, def.name) == 0 )
        {
            return n;
        }
    }

    return ACT_NONE;
}

OptNum optFindByChar(char c)
{
    OptNum n;

    for (int i = 0; i < OPT_COUNT; i++)
    {
        const OptDef& def = optDefs[i];
        n = OptNum(i);

        if ( def.sym[0] == c && def.sym[1] == 0 )
        {
            return n;
        }
    }

    return OPT_NONE;
}

OptNum optFindBySym(const char* sym)
{
    OptNum n;

    for (int i = 0; i < OPT_COUNT; i++)
    {
        const OptDef& def = optDefs[i];
        n = OptNum(i);

        if ( strcmp(sym, def.sym) == 0 )
        {
            return n;
        }
    }

    return OPT_NONE;
}

OptNum optFindByName(const char* name)
{
    OptNum n;

    for (int i = 0; i < OPT_COUNT; i++)
    {
        const OptDef& def = optDefs[i];
        n = OptNum(i);

        if ( strcmp(name, def.name) == 0 )
        {
            return n;
        }
    }

    return OPT_NONE;
}

static OptNum optParse(const char* arg, Value& val)
{
    static const char*  multi_arg = 0;
    static const char*  multi_chr = 0;

    Size            n, ofs;
    char            c;
    bool            long_opt;
    OptNum          optnum;
    const OptDef*   optdef;
    const char*     dlm;
    char            key[OPT_KEY_MAX+1];
    Size            keylen;
    const char*     sval;

    ASSERT(!cen);

    if ( arg == 0 )
    {
        if ( multi_arg == 0 )
        {
            return OPT_NONE;
        }

        ASSERT(multi_chr != 0);
        arg = multi_arg;
    }
    else if ( arg[0] == '+' )
    {
        if ( strlen(arg) == 1 )
        {
            cer(CE_OPTEXPM, arg);
            return OPT_NONE;
        }

        multi_arg = arg;
        multi_chr = arg + 1;
    }
    else
    {
        ASSERT(arg[0] == '-');

        multi_arg = 0;
        multi_chr = 0;
    }

    if ( multi_arg )
    {
        if ( (c = *multi_chr) == 0 )
        {
            multi_arg = 0;
            multi_chr = 0;
            return OPT_NONE;
        }

        optnum = optFindByChar(c);
        if ( optnum == OPT_NONE )
        {
            cer(CE_OPTUNKM, arg, c);
            return OPT_NONE;
        }

        optdef = &optDefs[optnum];
        ASSERT(optdef->var.typnum == TYP_FLAG);

        val.setFlag(true, &optdef->var);

        multi_chr++;
        return optnum;
    }

    ASSERT(arg[0] == '-');

    n = strlen(arg);

    ASSERT(n >= 1);

    if ( n == 1 )
    {
        cer(CE_OPTEXPS, arg);
        return OPT_NONE;
    }

    if ( arg[1] == '-' )
    {
        if ( n == 2 )
        {
            cer(CE_OPTEXPL, arg);
            return OPT_NONE;
        }

        long_opt = true;
        ofs = 2;
    }
    else
    {
        long_opt = false;
        ofs = 1;
    }

    dlm = strchr(arg,'=');

    if ( dlm == 0 )
    {
        keylen = n - ofs;
        sval = 0;
    }
    else
    {
        keylen = dlm - arg - ofs;
        sval = dlm + 1;
    }

    if ( keylen > OPT_KEY_MAX )
    {
        if ( long_opt ) cer(CE_OPTUNKL, arg);
        else            cer(CE_OPTUNKS, arg);
        return OPT_NONE;
    }

    strncpyz(key, arg + ofs, keylen);

    optnum = long_opt ? optFindByName(key) : optFindBySym(key);

    if ( optnum == OPT_NONE )
    {
        if ( long_opt ) cer(CE_OPTUNKL, arg);
        else            cer(CE_OPTUNKS, arg);
        return OPT_NONE;
    }

    optdef = &optDefs[optnum];

    if ( sval == 0 )
    {
        if ( optdef->var.typnum != TYP_FLAG )
        {
            cer(CE_OPTRQD, arg);
            return OPT_NONE;
        }

        val.setFlag(true, &optdef->var);
    }
    else
    {
        val.parse(sval, &optdef->var);
        if ( pen )
        {
            cer(CE_OPTVAL, arg, pem);
            return OPT_NONE;
        }
    }

    ASSERT(!cen);
    return optnum;
}

static void cer(Cen e, ...)
{
    char    fmt[CEM_MAX+1];
    char    msg[CEM_MAX+1];
    CerDef  def;
    va_list args;

    // intermediate msg buffer allows
    // recursive error parameters
    // e.g. cer(CE_CFGFILE, ..., cem)

    fmt[0] = 0;

    va_start (args, e);

    ASSERT(e > 0 && e < CE_COUNT);

    def = cerDefs[e];
    ASSERT(def.num == e);

    if ( !strlenz(def.params) )
    {
        strncpyz(fmt, def.msg, CEM_MAX);
        strncatz(fmt, ": ", CEM_MAX);
        strncatz(fmt, def.params, CEM_MAX);
        vsnprintfz(msg, CEM_MAX, fmt, args);

        strncpyz(cemMutable, msg, CEM_MAX);
    }
    else
    {
        strncpyz(cemMutable, def.msg, CEM_MAX);
    }

    va_end (args);
    cenMutable = e;
}

// EOF