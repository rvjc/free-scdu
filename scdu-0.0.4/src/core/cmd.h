// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include cmd.h separately - use core.h instead!"
#endif

const Size CEM_MAX          = 80;
const Size ENV_NAME_MAX     = 16;
const Size ACT_NAME_MAX     = 8;
const Size OPT_SYM_MAX      = 3;
const Size OPT_NAME_MAX     = 16;
const Size OPT_KEY_MAX      = OPT_NAME_MAX;
const Size PARAMS_MAX       = 16;
const Size CFG_ENTRY_MAX    = 255;

enum Cen
{
    CE_OK = 0,
    CE_ACTEXP,
    CE_ACTUNK,
    CE_OPTEXPS,
    CE_OPTEXPL,
    CE_OPTEXPM,
    CE_OPTUNKS,
    CE_OPTUNKL,
    CE_OPTUNKM,
    CE_OPTRQD,
    CE_OPTVAL,
    CE_PRMMORE,
    CE_PRMLESS,
    CE_PRMOPT,
    CE_CFGOPEN,
    CE_CFGFILE,
    CE_CFGOPT,
    CE_CFGENT,
    CE_SETWD,
    CE_COUNT
};

struct CerDef
{
    Cen         num;
    const char* sym;
    const char* msg;
    const char* params;
};

enum EnvNum
{
    ENV_NONE = -1,
    ENV_CS = 0,
    ENV_CST,
    ENV_WD,
    ENV_COUNT
};

struct EnvDef
{
    const EnvNum    num;
    const char*     name;
    const Var       var;
};

class Env
{
public:
    Env();
    void get(EnvNum envnum, Value& val) const;

    Size    chunkSize;
    Pick    consoleStreams;
    Str     workDirectory;
};

enum ActNum
{
    ACT_NONE = -1,
    ACT_COPY = 0,
    ACT_HELP,
    ACT_INFO,
    ACT_SHOW,
    ACT_VIEW,
    ACT_COUNT
};

struct ActDef
{
    const ActNum    num;
    const char*     name;
    const Size      pmin;
    const Size      pmax;
    const char*     usage;
    const char*     desc;
    const char*     example;
};

class Action
{
public:
    Action();
    Action(const Action&) = delete;
    Action& operator=(const Action&) = delete;
    void set(ActNum n);

    ActNum      num;
    Str         name;
};

enum OptNum
{
    OPT_NONE = -1,
    OPT_A = 0,
    OPT_F,
    OPT_P,
    OPT_R,
    OPT_AP,
    OPT_AT,
    OPT_BP,
    OPT_BS,
    OPT_CF,
    OPT_CS,
    OPT_FD,
    OPT_FF,
    OPT_FL,
    OPT_FST,
    OPT_LF,
    OPT_LM,
    OPT_NS,
    OPT_ND,
    OPT_NL,
    OPT_PF,
    OPT_PR,
    OPT_PS,
    OPT_PW,
    OPT_RS,
    OPT_RD,
    OPT_RL,
    OPT_RM,
    OPT_RR,
    OPT_SS,
    OPT_WD,
    OPT_COUNT
};

struct OptDef
{
    const OptNum    num;
    const char*     sym;        // short option symbol
    const char*     name;       // long option name
    const char*     dval;       // default value
    const Var       var;
    const char*     desc;
};

class Options
{
public:
    Options();
    Options(const Options&) = delete;
    Options& operator=(const Options&) = delete;

    void set(OptNum optnum, const Value& val);
    void get(OptNum optnum, Value& val) const;

    bool    all;
    bool    force;
    bool    preserve;
    bool    recurse;
    bool    asciiPath;
    bool    asciiText;
    bool    binaryPrefix;
    Size    bufferSize;
    Str     configFile;
    Size    chunkSize;
    Size    flushDelay;
    Size    flushFactor;
    Size    flushLimit;
    Pick    fastStreams;
    Str     logFile;
    Pick    logMode;
    Pick    newlineStd;
    Pick    newlineDgn;
    Pick    newlineLog;
    Pick    progressFeed;
    double  progressRate;
    Pick    progressStats;
    Size    progressWidth;
    Pick    routeStd;
    Pick    routeDgn;
    Pick    routeLog;
    Pick    rateMetric;
    bool    rawReporting;
    Pick    summaryStats;
    Str     workDirectory;
};

class Params
{
public:
    Params();
    Params(const Params&) = delete;
    Params& operator=(const Params&) = delete;
    const Str& operator[](const Size i) const;
    void add(const char* param);

    const Size& count;

private:
    Size  mCount;
    Str   mItems[PARAMS_MAX];
};

class Cmd
{
public:
    Cmd();
    void init(int argc, char* argv[]) const;

    const bool&     initialised;
    const bool&     recap;
    const Env&      env;
    const Action&   action;
    const Options&  options;
    const Params&   params;

private:
    void config() const;
    void setEnv() const;
    void setEnvChunkSize(bool force) const;
    void setEnvConsoleStreams(bool force) const;
    void setEnvWorkDirectory(bool force) const;

    mutable bool    mInitialised;
    mutable bool    mRecap;
    mutable Env     mEnv;
    mutable Action  mAction;
    mutable Options mOptions;
    mutable Params  mParams;
};

extern const Cen&   cen;
extern const char*  const cem;
extern const CerDef cerDefs[CE_COUNT];
extern const Cmd    cmd;
extern const EnvDef envDefs[ENV_COUNT];
extern const ActDef actDefs[ACT_COUNT];
extern const OptDef optDefs[OPT_COUNT];

extern void cerClear();
extern ActNum actFind(const char* name);
extern OptNum optFindByChar(char c);
extern OptNum optFindBySym(const char* sym);
extern OptNum optFindByName(const char* name);

// EOF