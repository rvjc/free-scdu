# Copyright 2015-2017 RVJ Callanan.
# Released under the GNU General Public License (Version 3).

# build.sh: portable build script for scdu project
# requires companion build.mak makefile

################################################################################
# VERIFY SHELL
################################################################################

# Before proceeding to script proper, ensure that we are in a compatible shell.
# At a minimum, we need up-to-date bash/coreutils and a functioning log file.
# For initial bash test, use legacy POSIX syntax (supported by 99.9% of shells).
# Once bash and coreutils are verified, we can use modern bash syntax, built-ins
# and coreutils commands.

# Verification of remaining tools (some of which are config-dependent) is
# performed later. To avoid long startup delays, this information is cached
# between script executions along with other stable data. Unverified tools
# may be referenced in function declarations as long as these functions are NOT
# called before the relevant tools have been located and verified. For the same
# reason, manual error handling is required during this shell verification phase.

# Version requirements for general-purpose tools and tool sets are relaxed
# (within reason) and will be updated periodically as the SCDU project evolves.
# However, tools which are intrinsic to binary generation and documentation will
# have a strict correlation with SCDU version.

printf "initialising...\n"

SCDU_ERR=1

case $SHELL in
    *bash)
        if [ ${BASH_VERSINFO[0]} -ge 4 ] && [ ${BASH_VERSINFO[1]} -ge 3 ]; then
            SCDU_ERR=0
        fi
        ;;
esac

if [ $SCDU_ERR -ne 0 ]; then

    printf "\n"
    printf "=================================================================\n"
    printf "*** build error ***    bash 4.2 (or later) required\n"
    printf "=================================================================\n"
    printf "\n"
    exit 1

fi

unset SCDU_ERR

# coreutils can be verified via any individual tool e.g. cp

if   [[ "$(type -p cp)" == "" ]] \
|| ! [[ "$(cp --version | head -n1)" =~ [0-9][.][0-9] ]] \
||   [[ "${BASH_REMATCH[0]}" < "8.2" ]]; then
    printf "\n"
    printf "=================================================================\n"
    printf "*** build error ***    coreutils 8.2 (or later) required\n"
    printf "=================================================================\n"
    printf "\n"
    exit 1
fi

# Before using i/o functions, start off a new build log. If log file cannot be
# created, flag error manually and exit (this would also be indicative that
# directory as a whole is not writable which is good to catch early

printf "BUILD LOG STARTED      $(date -u)\n" > "build.log"

if [ $? -ne 0 ]; then
    printf "\n"
    printf "=================================================================\n"
    printf "*** build error ***    unable to create log file: build.log\n"
    printf "=================================================================\n"
    printf "\n"
    exit 1
fi

################################################################################
# EXPORT VARS
################################################################################

# Note:
#
# 1. Exported variables have an SCDU_ prefix
#
# 2. Only trivial export initialisations can be performed at this point.
#    More complex initialisation will be performed by init() and config().
#
# 3. Export initialisations will yield a functioning i/o with safe/sensible
#    defaults so that i/o-related functions can be called after this point

################################################################################

export SCDU_CONFIGURED="0"
export SCDU_INTERACTIVE=$(if tty -s; then echo 1; else echo 0; fi)
export SCDU_MAKING="0"
export SCDU_TRAPS=""

export SCDU_COLS=$(if (( SCDU_INTERACTIVE==1 )); then tput cols; else echo 80; fi)
export SCDU_PROMPT="0"
export SCDU_SMAKE="0"
export SCDU_KMAKE="0"
export SCDU_RUNSH="current"
export SCDU_ARCH=""
export SCDU_MODE=""

export SCDU_BDIV=$(head -c $((SCDU_COLS-1)) < /dev/zero | tr '\0' '=')
export SCDU_SDIV=$(head -c $((SCDU_COLS-1)) < /dev/zero | tr '\0' '-')

export SCDU_OS=""
export SCDU_OS_ARCHS=""
export SCDU_OS_MODES=""
export SCDU_OS_ARCHS_NUM=""
export SCDU_OS_MODES_NUM=""

export SCDU_HOST=""
export SCDU_HOST_ARCHS=""
export SCDU_HOST_MODES=""
export SCDU_HOST_ARCHS_NUM=""
export SCDU_HOST_MODES_NUM=""
export SCDU_HOST_ARCHS_DEF=""
export SCDU_HOST_MODES_DEF=""

export SCDU_DEF_ARCH=""
export SCDU_DEF_MODE=""

export SCDU_BUILD_CMD=""
export SCDU_TARGET=""
export SCDU_ACTION=""

export SCDU_TAB_SIZE=4
export SCDU_EXTS_NOCR="sh mak cpp h txt css"
export SCDU_EXTS_NOTWS="sh mak cpp h css"
export SCDU_EXTS_NOTAB="sh cpp h txt css"

export SCDU_PRJ_ADIR=""
export SCDU_PRJ=""

export SCDU_VERSION=""
export SCDU_DATETIME="$(date --iso-8601='seconds')"

export SCDU_SRC_RDIR="src"
export SCDU_PKG_RDIR="pkg"
export SCDU_RUN_RDIR="run"
export SCDU_BLD_RDIR="bld"
export SCDU_TMP_RDIR="tmp"

export SCDU_APP_SRC_RDIR="$SCDU_SRC_RDIR/app"
export SCDU_COR_SRC_RDIR="$SCDU_SRC_RDIR/core"
export SCDU_FFS_SRC_RDIR="$SCDU_SRC_RDIR/ffs"
export SCDU_ALG_SRC_RDIR="$SCDU_SRC_RDIR/alg"
export SCDU_DOC_SRC_RDIR="$SCDU_SRC_RDIR/doc"

export SCDU_DOC_CSS_RPATH="$SCDU_DOC_SRC_RDIR/doc.css"
export SCDU_DOC_PRJ_RPATH="project.txt"
export SCDU_DOC_BLD_RPATH="build.txt"
export SCDU_DOC_LIC_RPATH="license.txt"
export SCDU_DOC_BUG_RPATH="bugs.txt"
export SCDU_DOC_CHG_RPATH="changes.txt"
export SCDU_DOC_TDO_RPATH="todo.txt"

export SCDU_APP_BLD_RDIR="$SCDU_BLD_RDIR/app"
export SCDU_COR_BLD_RDIR="$SCDU_BLD_RDIR/core"
export SCDU_FFS_BLD_RDIR="$SCDU_BLD_RDIR/ffs"
export SCDU_ALG_BLD_RDIR="$SCDU_BLD_RDIR/alg"
export SCDU_DOC_BLD_RDIR="$SCDU_BLD_RDIR/doc"

export SCDU_RUN_SCR_RPATH="$SCDU_RUN_RDIR/run.sh"
export SCDU_RUN_CMD=""

export SCDU_CACHE_RPATH=""

export SCDU_BSH_ADIR=""
export SCDU_CRU_ADIR=""
export SCDU_FDU_ADIR=""
export SCDU_GRP_ADIR=""
export SCDU_SED_ADIR=""
export SCDU_AWK_ADIR=""
export SCDU_GRF_ADIR=""
export SCDU_GCC_ADIR=""
export SCDU_PAN_ADIR=""
export SCDU_MAK_ADIR=""
export SCDU_BNU_ADIR=""

export SCDU_BSH_VERSION=""
export SCDU_CRU_VERSION=""
export SCDU_FDU_VERSION=""
export SCDU_GRP_VERSION=""
export SCDU_SED_VERSION=""
export SCDU_AWK_VERSION=""
export SCDU_GRF_VERSION=""
export SCDU_GCC_VERSION=""
export SCDU_PAN_VERSION=""
export SCDU_MAK_VERSION=""
export SCDU_BNU_VERSION=""

export SCDU_APP_CFG_RDIR=""
export SCDU_APP_OBJ_RDIR=""
export SCDU_APP_BIN_RDIR=""

export SCDU_COR_CFG_RDIR=""
export SCDU_COR_OBJ_RDIR=""
export SCDU_COR_LIB_RDIR=""

export SCDU_FFS_CFG_RDIR=""
export SCDU_FFS_OBJ_RDIR=""
export SCDU_FFS_LIB_RDIR=""

export SCDU_ALG_CFG_RDIR=""
export SCDU_ALG_OBJ_RDIR=""
export SCDU_ALG_LIB_RDIR=""

export SCDU_APP_BIN=""
export SCDU_APP_BIN_RPATH=""

export SCDU_COR_LIB=""
export SCDU_COR_LIB_RPATH=""

export SCDU_FFS_LIB=""
export SCDU_FFS_LIB_RPATH=""

export SCDU_ALG_LIB=""
export SCDU_ALG_LIB_RPATH=""

export SCDU_DOC_UG=""
export SCDU_DOC_UGH=""
export SCDU_DOC_UGM=""
export SCDU_DOC_UGT=""
export SCDU_DOC_UGH_RPATH=""
export SCDU_DOC_UGM_RPATH=""
export SCDU_DOC_UGT_RPATH=""

export SCDU_DOC_DG=""
export SCDU_DOC_DGT=""
export SCDU_DOC_DGM=""
export SCDU_DOC_DGH=""
export SCDU_DOC_DGT_RPATH=""
export SCDU_DOC_DGM_RPATH=""
export SCDU_DOC_DGH_RPATH=""

export SCDU_PKB=""
export SCDU_PKB_RPATH=""

export SCDU_PKS=""
export SCDU_PKS_RPATH=""

export SCDU_PKD=""
export SCDU_PKD_RPATH=""

export SCDU_LIBS=""

export SCDU_COR_LIB_DEPS=""
export SCDU_FFS_LIB_DEPS=""
export SCDU_ALG_LIB_DEPS=""
export SCDU_APP_BIN_DEPS=""
export SCDU_DOC_UGT_DEPS=""
export SCDU_DOC_UGM_DEPS=""
export SCDU_DOC_UGH_DEPS=""
export SCDU_DOC_DGT_DEPS=""
export SCDU_DOC_DGM_DEPS=""
export SCDU_DOC_DGH_DEPS=""

export SCDU_CFLAGS=""
export SCDU_LFLAGS=""

export SCDU_PATH=""

################################################################################
# EXPORTED FUNCTIONS
################################################################################

# Note:
#
# 1. Exported functions have an scdu_ prefix
#
# 2. Functions called indirectly by exported functions must also be exported.
#
# 3. Variables used by exported functions must also be exported.
#
# 4. When an exported function is called directly in makefile, make will report
#    a 'command not found' error (even in recipes). Apparently make checks for
#    command availability before executing and this check does not recognise
#    exported functions. The best way to work around make's misplaced vigilence
#    is to chain the exported function after a proper command or noop (:)

################################################################################

# scdu_check (no args)

# Recaps exported variables to assist debugging

scdu_check() {

    scdu_trap_set

    scdu_msg "recap only"
    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "external variables"
    scdu_msg "$SCDU_BDIV"
    scdu_msg
    scdu_pvs "SCDU_SET_COLS"
    scdu_pvs "SCDU_SET_PROMPT"
    scdu_pvs "SCDU_SET_SMAKE"
    scdu_pvs "SCDU_SET_KMAKE"
    scdu_pvs "SCDU_SET_RUNSH"
    scdu_pvs "SCDU_SET_ARCH"
    scdu_pvs "SCDU_SET_MODE"
    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "internal variables"
    scdu_msg "$SCDU_BDIV"
    scdu_msg
    scdu_pvs "SCDU_CONFIGURED"
    scdu_pvs "SCDU_INTERACTIVE"
    scdu_pvs "SCDU_MAKING"
    scdu_pvs "SCDU_TRAPS"
    scdu_pvs
    scdu_pvs "SCDU_COLS"
    scdu_pvs "SCDU_PROMPT"
    scdu_pvs "SCDU_SMAKE"
    scdu_pvs "SCDU_KMAKE"
    scdu_pvs "SCDU_RUNSH"
    scdu_pvs "SCDU_ARCH"
    scdu_pvs "SCDU_MODE"
    scdu_msg
    scdu_pvs "SCDU_OS"
    scdu_pvs "SCDU_OS_ARCHS"
    scdu_pvs "SCDU_OS_MODES"
    scdu_pvs "SCDU_OS_ARCHS_NUM"
    scdu_pvs "SCDU_OS_MODES_NUM"
    scdu_msg
    scdu_pvs "SCDU_HOST"
    scdu_pvs "SCDU_HOST_ARCHS"
    scdu_pvs "SCDU_HOST_MODES"
    scdu_pvs "SCDU_HOST_ARCHS_NUM"
    scdu_pvs "SCDU_HOST_MODES_NUM"
    scdu_pvs "SCDU_HOST_ARCHS_DEF"
    scdu_pvs "SCDU_HOST_MODES_DEF"
    scdu_msg
    scdu_pvs "SCDU_DEF_ARCH"
    scdu_pvs "SCDU_DEF_MODE"
    scdu_msg
    scdu_pvs "SCDU_BUILD_CMD"
    scdu_pvs "SCDU_TARGET"
    scdu_pvs "SCDU_ACTION"
    scdu_msg
    scdu_pvs "SCDU_TAB_SIZE"
    scdu_pvs "SCDU_EXTS_NOCR"
    scdu_pvs "SCDU_EXTS_NOTWS"
    scdu_pvs "SCDU_EXTS_NOTAB"
    scdu_msg
    scdu_pvs "SCDU_PRJ_ADIR"
    scdu_pvs "SCDU_PRJ"
    scdu_msg
    scdu_pvs "SCDU_VERSION"
    scdu_pvs "SCDU_DATETIME"
    scdu_msg
    scdu_pvs "SCDU_SRC_RDIR"
    scdu_pvs "SCDU_PKG_RDIR"
    scdu_pvs "SCDU_RUN_RDIR"
    scdu_pvs "SCDU_BLD_RDIR"
    scdu_pvs "SCDU_TMP_RDIR"
    scdu_msg
    scdu_pvs "SCDU_APP_SRC_RDIR"
    scdu_pvs "SCDU_COR_SRC_RDIR"
    scdu_pvs "SCDU_FFS_SRC_RDIR"
    scdu_pvs "SCDU_ALG_SRC_RDIR"
    scdu_pvs "SCDU_DOC_SRC_RDIR"
    scdu_msg
    scdu_pvs "SCDU_DOC_CSS_RPATH"
    scdu_pvs "SCDU_DOC_PRJ_RPATH"
    scdu_pvs "SCDU_DOC_BLD_RPATH"
    scdu_pvs "SCDU_DOC_LIC_RPATH"
    scdu_pvs "SCDU_DOC_BUG_RPATH"
    scdu_pvs "SCDU_DOC_CHG_RPATH"
    scdu_pvs "SCDU_DOC_TDO_RPATH"
    scdu_msg
    scdu_pvs "SCDU_APP_BLD_RDIR"
    scdu_pvs "SCDU_COR_BLD_RDIR"
    scdu_pvs "SCDU_FFS_BLD_RDIR"
    scdu_pvs "SCDU_ALG_BLD_RDIR"
    scdu_pvs "SCDU_DOC_BLD_RDIR"
    scdu_msg
    scdu_pvs "SCDU_RUN_SCR_RPATH"
    scdu_pvs "SCDU_RUN_CMD"
    scdu_msg
    scdu_pvs "SCDU_CACHE_RPATH"
    scdu_msg
    scdu_pvs "SCDU_BSH_ADIR"
    scdu_pvs "SCDU_CRU_ADIR"
    scdu_pvs "SCDU_FDU_ADIR"
    scdu_pvs "SCDU_GRP_ADIR"
    scdu_pvs "SCDU_SED_ADIR"
    scdu_pvs "SCDU_AWK_ADIR"
    scdu_pvs "SCDU_GRF_ADIR"
    scdu_pvs "SCDU_GCC_ADIR"
    scdu_pvs "SCDU_PAN_ADIR"
    scdu_pvs "SCDU_MAK_ADIR"
    scdu_pvs "SCDU_BNU_ADIR"
    scdu_msg
    scdu_pvs "SCDU_BSH_VERSION"
    scdu_pvs "SCDU_CRU_VERSION"
    scdu_pvs "SCDU_FDU_VERSION"
    scdu_pvs "SCDU_GRP_VERSION"
    scdu_pvs "SCDU_SED_VERSION"
    scdu_pvs "SCDU_AWK_VERSION"
    scdu_pvs "SCDU_GRF_VERSION"
    scdu_pvs "SCDU_GCC_VERSION"
    scdu_pvs "SCDU_PAN_VERSION"
    scdu_pvs "SCDU_MAK_VERSION"
    scdu_pvs "SCDU_BNU_VERSION"
    scdu_msg
    scdu_pvs "SCDU_APP_CFG_RDIR"
    scdu_pvs "SCDU_APP_OBJ_RDIR"
    scdu_pvs "SCDU_APP_BIN_RDIR"
    scdu_msg
    scdu_pvs "SCDU_COR_CFG_RDIR"
    scdu_pvs "SCDU_COR_OBJ_RDIR"
    scdu_pvs "SCDU_COR_LIB_RDIR"
    scdu_msg
    scdu_pvs "SCDU_FFS_CFG_RDIR"
    scdu_pvs "SCDU_FFS_OBJ_RDIR"
    scdu_pvs "SCDU_FFS_LIB_RDIR"
    scdu_msg
    scdu_pvs "SCDU_ALG_CFG_RDIR"
    scdu_pvs "SCDU_ALG_OBJ_RDIR"
    scdu_pvs "SCDU_ALG_LIB_RDIR"
    scdu_msg
    scdu_pvs "SCDU_APP_BIN"
    scdu_pvs "SCDU_APP_BIN_RPATH"
    scdu_msg
    scdu_pvs "SCDU_COR_LIB"
    scdu_pvs "SCDU_COR_LIB_RPATH"
    scdu_msg
    scdu_pvs "SCDU_FFS_LIB"
    scdu_pvs "SCDU_FFS_LIB_RPATH"
    scdu_msg
    scdu_pvs "SCDU_ALG_LIB"
    scdu_pvs "SCDU_ALG_LIB_RPATH"
    scdu_msg
    scdu_pvs "SCDU_DOC_UG"
    scdu_pvs "SCDU_DOC_UGH"
    scdu_pvs "SCDU_DOC_UGM"
    scdu_pvs "SCDU_DOC_UGT"
    scdu_pvs "SCDU_DOC_UGH_RPATH"
    scdu_pvs "SCDU_DOC_UGM_RPATH"
    scdu_pvs "SCDU_DOC_UGT_RPATH"
    scdu_msg
    scdu_pvs "SCDU_DOC_DG"
    scdu_pvs "SCDU_DOC_DGT"
    scdu_pvs "SCDU_DOC_DGM"
    scdu_pvs "SCDU_DOC_DGH"
    scdu_pvs "SCDU_DOC_DGT_RPATH"
    scdu_pvs "SCDU_DOC_DGM_RPATH"
    scdu_pvs "SCDU_DOC_DGH_RPATH"
    scdu_msg
    scdu_pvs "SCDU_PKB"
    scdu_pvs "SCDU_PKB_RPATH"
    scdu_msg
    scdu_pvs "SCDU_PKS"
    scdu_pvs "SCDU_PKS_RPATH"
    scdu_msg
    scdu_pvs "SCDU_PKD"
    scdu_pvs "SCDU_PKD_RPATH"
    scdu_msg
    scdu_pvl "SCDU_LIBS"
    scdu_msg
    scdu_pvl "SCDU_COR_LIB_DEPS"
    scdu_msg
    scdu_pvl "SCDU_FFS_LIB_DEPS"
    scdu_msg
    scdu_pvl "SCDU_ALG_LIB_DEPS"
    scdu_msg
    scdu_pvl "SCDU_APP_BIN_DEPS"
    scdu_msg
    scdu_pvl "SCDU_DOC_UGT_DEPS"
    scdu_msg
    scdu_pvl "SCDU_DOC_UGM_DEPS"
    scdu_msg
    scdu_pvl "SCDU_DOC_UGH_DEPS"
    scdu_msg
    scdu_pvl "SCDU_DOC_DGT_DEPS"
    scdu_msg
    scdu_pvl "SCDU_DOC_DGM_DEPS"
    scdu_msg
    scdu_pvl "SCDU_DOC_DGH_DEPS"
    scdu_msg
    scdu_pvl "SCDU_CFLAGS"
    scdu_msg
    scdu_pvl "SCDU_LFLAGS"
    scdu_msg
    scdu_pvl "SCDU_PATH"
    scdu_msg
    scdu_pvl "PATH"

    scdu_trap_clr
}

################################################################################

# scdu_msg [<arg1>] [<arg2>] [<arg3>]

# Outputs and logs column-aligned multi-part message on a single line

scdu_msg() {

    scdu_out "$@"
    scdu_log "$@"
}

################################################################################

# scdu_out [<arg1>] [<arg2>] [<arg3>]

# Outputs column-aligned multi-part message on a single line

scdu_out() {

    if [[ "$1" == "" ]]; then
        printf "\n"
    elif [[ "$2" == "" ]]; then
        printf "%s\n" "$1"
    elif [[ "$3" == "" ]]; then
        printf "%-22s %s\n" "$1" "$2"
    else
        printf "%-22s %-22s %s\n" "$1" "$2" "$3"
    fi
}

################################################################################

# scdu_log [<arg1>] [<arg2>] [<arg3>]

# logs column-aligned multi-part message on a single line

scdu_log() {

    local log

    # use current directory for build.log
    # until project directory is validated and known

    if [[ "$SCDU_PRJ_ADIR" == "" ]]; then
        log="build.log"
    else
        log="$SCDU_PRJ_ADIR/build.log"
    fi

    if [[ "$1" == "" ]]; then
        printf "\n" >> "$log"
    elif [[ "$2" == "" ]]; then
        printf "%s\n" "$1" >> "$log"
    elif [[ "$3" == "" ]]; then
        printf "%-22s %s\n" "$1" "$2" >> "$log"
    else
        printf "%-22s %-22s %s\n" "$1" "$2" "$3" >> "$log"
    fi
}

################################################################################

# scdu_sv <varname>

# Outputs specified varname and its current value in short format

scdu_pvs() {

    scdu_msg "$1" "${!1}"
}

################################################################################

# scdu_lv <varname>

# Outputs specified varname and its current value in long format

scdu_pvl() {

    scdu_msg "$1"
    scdu_msg "$SCDU_SDIV"
    scdu_msg "${!1}"
}

################################################################################

# scdu_continue [<message>]

# Performs 'continue' dialog for interactive shell (when prompting is enabled).

scdu_continue() {

    local r m

    if (( $SCDU_INTERACTIVE == 1 )) && (( $SCDU_PROMPT == 1 )); then

        m="continue? [y/n]"

        if [[ "$1" != "" ]]; then
            m="$1: $m"
        fi

        while true; do
            read -r -n 1 -p "$m " r
            case "$r" in
                [yY]) scdu_msg; return ;;
                   *) scdu_msg ;;
            esac
        done

    fi
}

################################################################################

# scdu_exc <exit code>

# Performs controlled script exit

scdu_exc() {

    scdu_log "BUILD LOG COMPLETED    $(date -u)"
    scdu_continue "about to exit"

    exit "$1"
}

################################################################################

# scdu_eok [<message>]

# Normal termination (with optional warning message)

scdu_eok() {

    scdu_msg
    scdu_msg "$SCDU_BDIV"

    if [[ "$1" == "" ]]; then
        scdu_msg "$SCDU_BUILD_CMD" "done"
    else
        scdu_msg "$SCDU_BUILD_CMD" "done ($1)"
    fi

    scdu_msg "$SCDU_BDIV"
    scdu_msg

    scdu_exc "0"
}

################################################################################

# scdu_bug <message>

# Program error termination

scdu_bug() {

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "*** BUILD BUG ***" "line ${BASH_LINENO[0]}: $1"
    scdu_msg "$SCDU_BDIV"
    scdu_msg

    scdu_exc "1"
}

################################################################################

# scdu_err [<message>]

# Build error termination

scdu_err() {

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "*** BUILD ERROR ***" "$1"
    scdu_msg "$SCDU_BDIV"
    scdu_msg

    scdu_exc "1"
}

################################################################################

# scdu_trap (no arguments)

# Trapped error handler. This function is NOT called directly but is activated
# and deactivated via scdu_trap_set() and scdu_trap_clr() respectively. If an
# error is trapped while a make is in progress (i.e. during a callback of an
# exported function) then a simple exit will be performed using the trapped
# error code. Make will then detect this error and exit with its own error code
# which will be detected in turn by this script just like any other make error.
# If an error is trapped when a make is NOT in progress then a normal build
# error is raised. In this way, individual functions employing error trapping
# may be called directly or indirectly (via make) with sensible outcomes.
# The trap hierarchy is tracked using the SCDU_TRAPS variable. Since arrays
# cannot be exported, SCDU_TRAPS consists of a string of @-delimited function
# names which are pushed and popped as traps are set and cleared. Note that
# traps in this script are assumed to have function scope. Also, trapped errors
# errors will not propagate up the call stack, it traps

scdu_trap() {

    local e="$?"
    local t

    if [ $e -eq 0 ]; then
        scdu_bug "trapped error has zero error code"
    fi

    if [[ "$SCDU_TRAPS" == "" ]]; then
        scdu_bug "error trapped but no trap was set"
    fi

    t=${SCDU_TRAPS##*@}

    scdu_msg "$t fail"

    if (( $SCDU_MAKING == 1 )); then
        exit "$e"
    else
        scdu_err "unexpected error: try build clean!!!"
    fi
}

################################################################################

# scdu_trap_set

# Sets error trap

scdu_trap_set() {

    local f=${FUNCNAME[1]}

    if [[ "$f" == "" ]]; then
        scdu_bug "scdu_trap_set must be called from within a function"
    fi

    trap scdu_trap ERR

    SCDU_TRAPS+="@$f"
}

################################################################################

# scdu_trap_clr

# Clears error trap

scdu_trap_clr() {

    local t f

    f=${FUNCNAME[1]}

    if [[ "$f" == "" ]]; then
        scdu_bug "scdu_trap_clr must be called from within a function"
    fi

    if [[ "$SCDU_TRAPS" == "" ]]; then
        scdu_bug "attempt to clear inactive trap: $f"
    fi

    t=${SCDU_TRAPS##*@}

    if [[ "$f" != "$t" ]]; then
        scdu_bug "scdu_trap_clr function mismatch: $f (expected $t)"
    fi

    SCDU_TRAPS=${SCDU_TRAPS%@*}

    trap - ERR
}

################################################################################

# scdu_make <target>

# Invokes make with appropriate flags and checks.

scdu_make() {

    local mflags=""

    if [[ "$1" == "" ]]; then
        scdu_bug "make needs target"
    fi

    if (( $SCDU_MAKING == 1 )); then
        scdu_bug "make invoked while make still in progress"
    fi

    # KMAKE (keep making) only makes sense with compiled targets

    if (( SCDU_KMAKE == 1 )); then
        case $1 in
            app|libs|core|ffs|alg) mflags+="-k" ;;
        esac
    fi

    # scdu_trap (which may be triggered by make callbacks)
    # needs to be aware when a make is in progress

    SCDU_MAKING="1"

    make -f build.mak -q "$1"

    if [ $? -eq 0 ]; then

        scdu_msg "$1 is up-to-date"

    else

        make -f build.mak $mflags "$1"

        if [ $? -ne 0 ]; then

            scdu_msg
            scdu_msg "$SCDU_BDIV"
            scdu_msg "*** MAKE ERROR ***" "$1"
            scdu_msg "$SCDU_BDIV"
            scdu_msg

            SCDU_MAKING="0"

            if (( SCDU_SMAKE == 1 )); then
                scdu_exc "0"
            else
                scdu_exc "1"
            fi
        fi
    fi

    SCDU_MAKING="0"
}

################################################################################

# scdu_clean (no args)

# Cleans all machine-generated directories forcing re-build next time round

scdu_clean() {

    scdu_clean_dir $SCDU_BLD_RDIR
    scdu_clean_dir $SCDU_PKG_RDIR
    scdu_clean_dir $SCDU_TMP_RDIR
}

################################################################################

# scdu_clean_dir <dir>

# Empties directory of all files and sub-directories
# note: this raw function does NOT trap errors

scdu_clean_dir() {

    scdu_trap_set

    scdu_msg "cleaning" "$1"
    rm -rf $1/*

    scdu_trap_clr
}

################################################################################

# scdu_sanitise (no args)

# Sanitises all human-generated files in project

scdu_sanitise() {

    scdu_trap_set

    scdu_sanitise_dir .
    scdu_sanitise_dir $SCDU_APP_SRC_RDIR
    scdu_sanitise_dir $SCDU_COR_SRC_RDIR
    scdu_sanitise_dir $SCDU_FFS_SRC_RDIR
    scdu_sanitise_dir $SCDU_ALG_SRC_RDIR
    scdu_sanitise_dir $SCDU_DOC_SRC_RDIR

    scdu_trap_set
}

################################################################################

# scdu_sanitise_dir <dir>

# Ensures all relevant files in directory root have correct formats and encodings.
# note: this raw function does NOT trap errors

scdu_sanitise_dir() {

    scdu_trap_set

    local d t

    scdu_msg "sanitising" "$1"

    # remove CRs

    d="$SCDU_TMP_RDIR/sanitise/nocr/$1"

    mkdir -p "$d"

    for x in $SCDU_EXTS_NOCR; do
        for f in $(find "$1" -maxdepth 1 -name "*.$x" -type f); do
            t="$d/$(basename $f)"
            cp -f "$f" "$t"
            dos2unix -q -n "$t" "$t.nocr"
            cp -f "$t.nocr" "$f"
        done
    done

    # remove trailing whitespace

    d="$SCDU_TMP_RDIR/sanitise/notws/$1"

    mkdir -p "$d"

    for x in $SCDU_EXTS_NOTWS; do
        for f in $(find "$1" -maxdepth 1 -name "*.$x" -type f); do
            t="$d/$(basename $f)"
            cp -f "$f" "$t"
            sed 's/\s*$//g' "$t" > "$t.notws"
            cp -f "$t.notws" "$f"
        done
    done

    # replace tabs with spaces

    d="$SCDU_TMP_RDIR/sanitise/notab/$1"

    mkdir -p "$d"

    for x in $SCDU_EXTS_NOTAB; do
        for f in $(find "$1" -maxdepth 1 -name "*.$x" -type f); do
            t="$d/$(basename $f)"
            cp -f "$f" "$t"
            expand -t $SCDU_TAB_SIZE "$t" > "$t.notab"
            cp -f "$t.notab" "$f"
        done
    done

    scdu_trap_clr
}

################################################################################

# scdu_compile <object_path> <source_path>

# Compiles source to object file
# Does not trap errors; just returns compiler error status

scdu_compile() {

    scdu_msg "compiling" "$1"
    g++ $SCDU_CFLAGS -o $1 $2
}

################################################################################

# scdu_archive <lib_path> <obj_dir>

# Archives all object files in <obj_dir> to <lib_path>
# Does not trap errors; just returns archiver error status

scdu_archive() {

    local objs=$2/*.o

    scdu_msg "archiving" "$1"
    ar cr $1 $objs
}

################################################################################

# scdu_link (no args)

# Links object files and libs to create app executable
# Does not trap errors, just returns linker error status

scdu_link() {

    local t="$SCDU_APP_BIN_RPATH"

    scdu_msg "linking" "$t"
    g++ $SCDU_LFLAGS -o $t $SCDU_APP_OBJ_RDIR/*.o $SCDU_LIBS
}

################################################################################

# scdu_acks (no args)

# Outputs 'Acknowledgements' in markdown

scdu_acks() {

    scdu_trap_set

    local acks line

    acks=""

    while read line; do
        acks+="* $line\n"
    done <<< "$(scdu show -rd= -ns=N -rr acks)"

    printf "Acknowledgements"
    printf "================\n"
    printf "\n"
    printf "%s\n" "The SCDU Project acknowledges the following organisations and\n"
    printf "%s\n" "individuals whose tools, frameworks and wider contributions have\n"
    printf "%s\n" "helped bring our work to fruition:\n"
    printf "\n"
    printf "%s\n" "$acks"

    scdu_trap_clr
}

################################################################################

# scdu_authors (no args)

# Outputs 'Authors' in one line (delimited by commas)

scdu_authors() {

    local authors line

    authors=""

    while read line; do
        if [ -z "$authors" ]; then
            authors="$line"
        else
            authors+=", $line"
        fi
    done <<< "$(scdu show -rd= -ns=N -rr authors)"

    printf "$authors\n"
}

################################################################################

# scdu_contribs (no args)

# Outputs 'Contributors' in markdown

scdu_contribs() {

    local contribs line

    contribs=""

    while read line; do
        contribs+="* $line\n"
    done <<< "$(scdu show -rd= -ns=N -rr contribs)"

    printf "Contributors\n"
    printf "============\n"
    printf "\n"
    printf "The SCDU Project recognises the contributions of the following\n"
    printf "individuals in the key areas of design, management, coding,\n"
    printf "testing, documentation and support:\n"
    printf "\n"
    printf "$contribs"
}

################################################################################

# scdu_terms (no args)

# Outputs 'Terms of Use' in markdown

scdu_terms() {

    printf "Terms of Use\n"
    printf "============\n"
    printf "\n"
    scdu show -rd= -ns=N -rr terms
}

################################################################################

# scdu_ugt (no args)

# Generates user guide markdown text file

scdu_ugt() {

    scdu_trap_set

    local t title authors nicedate

    t="$SCDU_DOC_UGT_RPATH"

    scdu_msg "generating" "$t"

    title="SCDU User Guide"
    authors="$(scdu_authors)"
    nicedate="${SCDU_DATETIME:0:10}"

    printf "%% $title\n" > $t
    printf "%% $authors\n" >> $t
    printf "%% $nicedate\n" >> $t

    printf "\n" >> $t
    scdu_terms >> $t

    printf "\n" >> $t
    scdu_contribs >> $t

    printf "\n" >> $t
    scdu_acks >> $t

    #body

    printf "\n" >> $t
    scdu help -rd= -ns=N >> $t

    scdu_trap_clr
}

################################################################################

# scdu_ugm (no args)

# Generates user guide man file

scdu_ugm() {

    scdu_trap_set

    local t="$SCDU_DOC_UGM_RPATH"

    scdu_msg "generating" "$t"
    pandoc -w man -r markdown_github -s --toc --toc-depth=2 -o $t $SCDU_DOC_UGT_RPATH

    scdu_trap_clr
}

################################################################################

# scdu_ugh (no args)

# Generates user guide html file

scdu_ugh() {

    scdu_trap_set

    local t="$SCDU_DOC_UGH_RPATH"

    scdu_msg "generating" "$t"
    pandoc -w html5 -s --self-contained --toc --toc-depth=2 -c $SCDU_DOC_CSS_RPATH -o $t $SCDU_DOC_UGT_RPATH

    scdu_trap_clr
}

################################################################################

# scdu_dgt (no args)

# Generates developer guide markdown text file

scdu_dgt() {

    scdu_trap_set

    local t title authors nicedate

    t="$SCDU_DOC_DGT_RPATH"

    scdu_msg "generating" "$t"

    title="SCDU Developer Guide"
    authors="$(scdu_authors)"
    nicedate="${SCDU_DATETIME:0:10}"

    printf "%% $title\n" > $t
    printf "%% $authors\n" >> $t
    printf "%% $nicedate\n" >> $t

    printf "\n" >> $t
    printf "# ABOUT\n" >> $t

    printf "\n" >> $t
    scdu_terms >> $t

    printf "\n" >> $t
    scdu_contribs >> $t

    printf "\n" >> $t
    scdu_acks >> $t

    printf "\n" >> $t
    printf "# PROJECT\n" >> $t

    # project

    printf "\n" >> $t
    cat $SCDU_DOC_PRJ_RPATH | tail -n +4 >> $t

    printf "\n" >> $t
    cat $SCDU_DOC_BLD_RPATH | tail -n +4 >> $t

    # app

    printf "\n" >> $t
    printf "# APP LIBRARY\n" >> $t

    for f in $SCDU_APP_SRC_RDIR/*.txt; do
        printf "\n" >> $t
        cat $f | tail -n +4 >> $t
    done

    # doc

    printf "\n" >> $t
    printf "# APP LIBRARY\n" >> $t

    for f in $SCDU_DOC_SRC_RDIR/*.txt; do
        printf "\n" >> $t
        cat $f | tail -n +4 >> $t
    done

    # core lib

    printf "\n" >> $t
    printf "# CORE LIBRARY\n" >> $t

    for f in $SCDU_COR_SRC_RDIR/*.txt; do
        printf "\n" >> $t
        cat $f | tail -n +4 >> $t
    done

    # ffs lib

    printf "\n" >> $t
    printf "# FFS LIBRARY\n" >> $t

    for f in $SCDU_FFS_SRC_RDIR/*.txt; do
        printf "\n" >> $t
        cat $f | tail -n +4 >> $t
    done

    # alg lib

    printf "\n" >> $t
    printf "# ALG LIBRARY\n" >> $t

    for f in $SCDU_ALG_SRC_RDIR/*.txt; do
        printf "\n" >> $t
        cat $f | tail -n +4 >> $t
    done

    printf "\n" >> $t
    printf "# APPENDIX\n" >> $t

    # bugs

    printf "\n" >> $t
    cat $SCDU_DOC_BUG_RPATH | tail -n +4 >> $t

    # changes

    printf "\n" >> $t
    cat $SCDU_DOC_CHG_RPATH | tail -n +4 >> $t

    # to do

    printf "\n" >> $t
    cat $SCDU_DOC_TDO_RPATH | tail -n +4 >> $t

    # license

    printf "\n" >> $t
    cat $SCDU_DOC_LIC_RPATH >> $t

    scdu_trap_clr
}

################################################################################

# scdu_dgm (no args)

# Generates developer guide man file

scdu_dgm() {

    scdu_trap_set

    local t="$SCDU_DOC_DGM_RPATH"

    scdu_msg "generating" "$t"
    pandoc -w man -r markdown_github -s --toc --toc-depth=2 -o $t $SCDU_DOC_DGT_RPATH

    scdu_trap_clr
}

################################################################################

# scdu_dgh (no args)

# Generates developer guide html file

scdu_dgh() {

    scdu_trap_set

    local t="$SCDU_DOC_DGH_RPATH"

    scdu_msg "generating" "$t"
    pandoc -w html5 -s --self-contained --toc --toc-depth=2 -c $SCDU_DOC_CSS_RPATH -o $t $SCDU_DOC_DGT_RPATH

    scdu_trap_clr
}

################################################################################

# scdu_pkb (no args)

# Creates binary package and checksum for distribution/download

scdu_pkb() {

    scdu_trap_set

    local d="$SCDU_TMP_RDIR/pkb"

    scdu_msg "packing" "$SCDU_PKB_RPATH"

    mkdir -p "$d"
    rm -rf $d/*

    cp $SCDU_APP_BIN_RPATH $d

    # include copy of license
    # scdu will output OS-appropriate newlines by default

    scdu help -rd= license >> $d/license.txt

    tar -czf $SCDU_PKB_RPATH -C $d .

    pushd $SCDU_PKG_RDIR > /dev/null
    sha256sum -b $SCDU_PKB > $SCDU_PKB.sha256sum
    popd > /dev/null

    scdu_trap_clr
}

################################################################################

# scdu_pks (no args)

# Creates clean source package and checksum for distribution/download

scdu_pks() {

    scdu_trap_set

    local d="$SCDU_TMP_RDIR/pks"
    local p="$d/$SCDU_PRJ"

    scdu_msg "packing" "$SCDU_PKS_RPATH"

    mkdir -p "$d"
    rm -rf $d/*

    mkdir "$p"

    cp -r $SCDU_SRC_RDIR/ $p/
    cp build.sh $p/
    cp build.mak $p/

    # include copy of license
    # source is OS-agnostic, so force NIX newlines for consistency

    scdu help -rd= -ns=N license >> $p/license.txt

    tar -czf $SCDU_PKS_RPATH -C $d .

    pushd $SCDU_PKG_RDIR > /dev/null
    sha256sum -b $SCDU_PKS > $SCDU_PKS.sha256sum
    popd > /dev/null

    scdu_trap_clr
}

################################################################################

# scdu_pkd (no args)

# Creates documentation package and checksum for distribution/download

scdu_pkd() {

    scdu_trap_set

    scdu_msg "packing" "$SCDU_PKD_RPATH"

    tar -czf $SCDU_PKD_RPATH -C $SCDU_DOC_BLD_RDIR .

    pushd $SCDU_PKG_RDIR > /dev/null
    sha256sum -b $SCDU_PKD > $SCDU_PKD.sha256sum
    popd > /dev/null

    scdu_trap_clr
}

################################################################################

export -f scdu_check
export -f scdu_msg
export -f scdu_out
export -f scdu_log
export -f scdu_pvs
export -f scdu_pvl
export -f scdu_continue
export -f scdu_exc
export -f scdu_eok
export -f scdu_bug
export -f scdu_err
export -f scdu_trap
export -f scdu_trap_set
export -f scdu_trap_clr
export -f scdu_make
export -f scdu_clean
export -f scdu_clean_dir
export -f scdu_sanitise
export -f scdu_sanitise_dir
export -f scdu_compile
export -f scdu_archive
export -f scdu_link
export -f scdu_acks
export -f scdu_authors
export -f scdu_contribs
export -f scdu_terms
export -f scdu_ugt
export -f scdu_ugm
export -f scdu_ugh
export -f scdu_dgt
export -f scdu_dgm
export -f scdu_dgh
export -f scdu_pkb
export -f scdu_pks
export -f scdu_pkd

################################################################################
# INTERNAL FUNCTIONS
################################################################################

# add_path <path_var> <directory>

# Adds directory entry to end of path var while ignoring duplicate

add_path() {

    local p

    if [[ "$2" == "" ]]; then
        return
    fi

    p="${!1}"

    if [[ "$p" == "" ]]; then
        eval "$1=\"$2\""
    elif ! [[ ":$p:" =~ ":$2:" ]]; then
        eval "$1=\"$p:$2\""
    fi
}

################################################################################

# ins_path <path_var> <directory>

# Inserts directory entry at start of path var while ignoring duplicate

ins_path() {

    local p

    if [[ "$2" == "" ]]; then
        return
    fi

    p="${!1}"

    if [[ "$p" == "" ]]; then
        eval "$1=\"$2\""
    elif ! [[ ":$p:" =~ ":$2:" ]]; then
        eval "$1=\"$2:$p\""
    fi
}

################################################################################

# check_env (no args)

# Modifies any settings affected by external ENVVAR overrides.

check_env() {

    local arch mode

    ############################################################################
    # i/o settings
    ############################################################################

    # Although export defaults will have yielded a functional i/o sub-system,
    # external overrides should be assimilated as early as possible.

    # default SCDU_COLS = <terminal width> or 80 if no terminal is detected

    case "$SCDU_SET_COLS" in

        "") ;;

         *) if (( $SCDU_SET_COLS < 80 )) || (( $SCDU_SET_COLS > 200 )); then
                scdu_err "invalid envar SCDU_SET_COLS: $SCDU_SET_COLS"
            fi

            SCDU_COLS="$SCDU_SET_COLS"

            SCDU_BDIV=$(head -c $((SCDU_COLS-1)) < /dev/zero | tr '\0' '=')
            SCDU_SDIV=$(head -c $((SCDU_COLS-1)) < /dev/zero | tr '\0' '-')
            ;;
    esac

    # default SCDU_PROMPT = 0

    case "$SCDU_SET_PROMPT" in
        ""|0) ;;
           1) SCDU_PROMPT="1" ;;
           *) scdu_err "invalid envvar SCDU_SET_PROMPT: $SCDU_SET_PROMPT" ;;
    esac

    ############################################################################
    # build settings
    ############################################################################

    # default SCDU_SMAKE=0

    case "$SCDU_SET_SMAKE" in
        ""|0) ;;
           1) SCDU_SMAKE="1" ;;
           *) scdu_err "invalid envvar SCDU_SET_SMAKE: $SCDU_SET_SMAKE" ;;
    esac

    # default SCDU_KMAKE=0

    case "$SCDU_SET_KMAKE" in
        ""|0) ;;
           1) SCDU_KMAKE="1" ;;
           *) scdu_err "invalid envvar SCDU_SET_KMAKE: $SCDU_SET_KMAKE" ;;
    esac

    # default SCDU_RUNSH="current"

    case "$SCDU_SET_RUNSH" in
        ""|current) ;;
           command) SCDU_RUNSH="command"  ;;
          terminal) SCDU_RUNSH="terminal" ;;
                 *) scdu_err "invalid envvar SCDU_SET_RUNSH: $SCDU_SET_RUNSH" ;;
    esac

    ############################################################################
    # host-related settings
    ############################################################################

    # auto-detect OS since only native builds supported (no cross-compiles)
    # note: a given host may NOT support ALL OS archs and modes

    SCDU_HOST=$(uname -s | tr [:upper:] [:lower:])

    case "$SCDU_HOST" in

        mingw32*)

            SCDU_OS="windows"
            SCDU_HOST_ARCHS="i386"
            SCDU_HOST_MODES="release debug"
            ;;

        mingw64*)

            SCDU_OS="windows"
            SCDU_HOST_ARCHS="amd64 i386"
            SCDU_HOST_MODES="release debug"
            ;;

        *linux*)

            SCDU_OS="linux"
            SCDU_HOST_ARCHS="amd64 i386"
            SCDU_HOST_MODES="release debug"
            ;;

        *darwin*)

            SCDU_OS="darwin"
            SCDU_HOST_ARCHS="amd64"
            SCDU_HOST_MODES="release debug"
            ;;

        *)
            scdu_err "unsupported development host: $SCDU_HOST"
            ;;
    esac

    SCDU_HOST_ARCHS_NUM=$(echo "$SCDU_HOST_ARCHS" | wc -w)
    SCDU_HOST_MODES_NUM=$(echo "$SCDU_HOST_MODES" | wc -w)

    case "$SCDU_OS" in

        windows)

            SCDU_OS_ARCHS="amd64 i386"
            SCDU_OS_MODES="release debug"
            ;;

        linux)

            SCDU_OS_ARCHS="amd64 i386"
            SCDU_OS_MODES="release debug"
            ;;

        darwin)

            SCDU_OS_ARCHS="amd64"
            SCDU_OS_MODES="release debug"
            ;;

        *)
            scdu_bug "unexpected OS: $SCDU_OS"
            ;;
    esac

    # by convention, default host ARCH is first in host list

    SCDU_OS_ARCHS_NUM=$(echo "$SCDU_OS_ARCHS" | wc -w)

    for arch in $SCDU_HOST_ARCHS; do
        SCDU_HOST_ARCHS_DEF="$arch"
        break
    done

    # set default ARCH to host default unless over-ridden

    if [[ "$SCDU_SET_ARCH" == "" ]]; then

        SCDU_DEF_ARCH="$SCDU_HOST_ARCHS_DEF"

    elif ! [[ "$SCDU_HOST_ARCHS " =~ "$SCDU_SET_ARCH " ]]; then

        scdu_err "$SCDU_HOST host does not support SCDU_SET_ARCH: $SCDU_SET_ARCH"

    else

        SCDU_DEF_ARCH="$SCDU_SET_ARCH"
    fi

    # by convention, default host MODE is first in host list

    SCDU_OS_MODES_NUM=$(echo "$SCDU_OS_MODES" | wc -w)


    for mode in $SCDU_HOST_MODES; do
        SCDU_HOST_MODES_DEF="$mode"
        break
    done

     # set default MODE to host default unless over-ridden

    if [[ "$SCDU_SET_MODE" == "" ]]; then

        SCDU_DEF_MODE="$SCDU_HOST_MODES_DEF"

    elif ! [[ "$SCDU_HOST_MODES " =~ "$SCDU_SET_MODE " ]]; then

        scdu_err "$SCDU_HOST host does not support SCDU_SET_MODE: $SCDU_SET_MODE"

    else

        SCDU_DEF_MODE="$SCDU_SET_MODE"
    fi
}

################################################################################

# check_args <command-line args>

# Checks script arguments, assigning defaults if necessary

check_args() {

    if (( $# > 2 )); then
        scdu_err "too many build arguments"
    fi

    if [[ "$1" == "" ]]; then

        # use default build arguments

        SCDU_BUILD_CMD="build app"
        SCDU_TARGET="app"
        SCDU_ACTION=""

    else

        SCDU_BUILD_CMD="build $@"
        SCDU_TARGET="$1"
        SCDU_ACTION="$2"

    fi

    case "$SCDU_TARGET" in

        app|doc|libs|core|ffs|alg|ugs|ugt|ugm|ugh|dgs|dgt|dgm|dgh|pks|pkb|pkd|clean|sanitise|all) ;;

        *) scdu_err "invalid target: $SCDU_TARGET" ;;

    esac

    case "$SCDU_ACTION" in

        ""|check)
            ;;

        run)
            case "$SCDU_TARGET" in
                app)
                    ;;
                *)
                    scdu_err "'$SCDU_TARGET' target does not support 'run' action"
                    ;;
            esac
            ;;

        view)
            case "$SCDU_TARGET" in
                doc|ugs|dgs|ugt|ugm|ugh|dgt|dgm|dgh)
                    ;;
                *)
                    scdu_err "'$SCDU_TARGET' target does not support 'view' action"
                    ;;
            esac
            ;;

        *)
            scdu_err "invalid build action: $SCDU_ACTION"
            ;;

    esac
}

################################################################################

# check_dir

# Checks project directory tree essentials (excluding config-dependent items).
# Auto-creates missing files and directories where possible

check_dir() {

    local p d f

    # Build script and accompanying makefile are assumed to be in project root.
    # This provides a consistent point-of-reference for relative paths shared
    # between external tools, the makefile and the build script itself.
    # As such, it should also have been pre-assigned as the working directory

    p="$(readlink -m ${BASH_SOURCE[0]})"

    if [[ "$(basename $p)" != "build.sh" ]]; then
        scdu_err "build script mismatch: $p"
    fi

    SCDU_PRJ_ADIR=$(dirname $p)

    if [[ "$SCDU_PRJ_ADIR" = *[^[:alnum:].\-/]* ]]; then
        scdu_err "project path contains space or other undesirable character: $SCDU_PRJ_ADIR"
    fi

    if [[ "$PWD" != "$SCDU_PRJ_ADIR" ]]; then
        scdu_err "working directory should have been pre-assigned to project root: $PWD"
    fi

     if [[ ! -f "build.mak" ]]; then
        scdu_err "makefile not present in project root: $SCDU_PRJ_ADIR/build.mak"
    fi

    SCDU_PRJ="$(basename "$SCDU_PRJ_ADIR")"

    if [[ "${SCDU_PRJ:0:5}" != "scdu-" ]]; then
        scdu_err "project directory prefix must be 'scdu-': $SCDU_PRJ"
    fi

    SCDU_VERSION="${SCDU_PRJ:5}"

    # source directories should already be present; better to validate early

    for d in $SCDU_SRC_RDIR \
             $SCDU_APP_SRC_RDIR \
             $SCDU_COR_SRC_RDIR \
             $SCDU_FFS_SRC_RDIR \
             $SCDU_ALG_SRC_RDIR \
             $SCDU_DOC_SRC_RDIR; do

        if [[ ! -d "$d" ]]; then
            scdu_err "missing directory: $SCDU_PRJ_ADIR/$d"
        fi

    done

    if (( $(find $SCDU_SRC_RDIR -mindepth 1 -maxdepth 1 -type d | wc -l) != 5 )); then
        scdu_err "source directory contains spurious sub-directories"
    fi

    if (( $(find $SCDU_SRC_RDIR -mindepth 1 -maxdepth 1 -type f | wc -l) != 0 )); then
        scdu_err "source directory root contains spurious files"
    fi

    # essential source doc files should already be present

    for f in $SCDU_DOC_CSS_RPATH \
             $SCDU_DOC_PRJ_RPATH \
             $SCDU_DOC_BLD_RPATH \
             $SCDU_DOC_LIC_RPATH \
             $SCDU_DOC_BUG_RPATH \
             $SCDU_DOC_CHG_RPATH \
             $SCDU_DOC_TDO_RPATH; do

        if [[ ! -f "$f" ]]; then
            scdu_err "missing file: $SCDU_PRJ_ADIR/$f"
        fi
    done

    # other directories in project root can be auto-created

    for d in $SCDU_BLD_RDIR $SCDU_PKG_RDIR $SCDU_RUN_RDIR $SCDU_TMP_RDIR; do

        mkdir -p "$d"

        if [ $? -ne 0 ]; then
            scdu_err "unable to create directory: $d"
        fi

    done

    # auto-generate simple run script if not already present

    if [[ ! -f "$SCDU_RUN_SCR_RPATH" ]]; then
        printf "scdu show version" > "$SCDU_RUN_SCR_RPATH"
    elif [[ $(wc -l < $SCDU_RUN_SCR_RPATH) -ne 0 ]]; then
        scdu_err "run script must be a one-liner: $SCDU_RUN_SCR_RPATH"
    fi

    SCDU_RUN_CMD="$(cat $SCDU_RUN_SCR_RPATH)"
}

################################################################################

# cache_put <var_name>

# Appends <var_name>=<var_value> entry to config cache, escaping embedded
# quotes where applicable

cache_put() {

    local v e

    v="${!1}"

    # escape quotes

    e=${v//\"/\\\"}

    printf "%s=\"%s\"\n" "$1" "$e" >> "$SCDU_CACHE_RPATH"
    if [ $? -ne 0 ]; then
        scdu_err "cache write fail: $SCDU_CACHE_RPATH"
    fi
}

################################################################################

# cache_save

# Saves all cacheable variables and values to config cache

cache_save() {

    printf "# config cache created by build.sh on $(date -u)\n\n" > "$SCDU_CACHE_RPATH"

    if [ $? -ne 0 ]; then
        scdu_err "unable to create cache: $SCDU_CACHE_RPATH"
    fi

    scdu_trap_set

    cache_put "SCDU_BSH_ADIR"
    cache_put "SCDU_CRU_ADIR"
    cache_put "SCDU_FDU_ADIR"
    cache_put "SCDU_GRP_ADIR"
    cache_put "SCDU_SED_ADIR"
    cache_put "SCDU_AWK_ADIR"
    cache_put "SCDU_GRF_ADIR"
    cache_put "SCDU_GCC_ADIR"
    cache_put "SCDU_PAN_ADIR"
    cache_put "SCDU_MAK_ADIR"
    cache_put "SCDU_BNU_ADIR"

    cache_put "SCDU_BSH_VERSION"
    cache_put "SCDU_CRU_VERSION"
    cache_put "SCDU_FDU_VERSION"
    cache_put "SCDU_GRP_VERSION"
    cache_put "SCDU_SED_VERSION"
    cache_put "SCDU_AWK_VERSION"
    cache_put "SCDU_GRF_VERSION"
    cache_put "SCDU_GCC_VERSION"
    cache_put "SCDU_PAN_VERSION"
    cache_put "SCDU_MAK_VERSION"
    cache_put "SCDU_BNU_VERSION"

    cache_put "SCDU_APP_CFG_RDIR"
    cache_put "SCDU_APP_OBJ_RDIR"
    cache_put "SCDU_APP_BIN_RDIR"

    cache_put "SCDU_COR_CFG_RDIR"
    cache_put "SCDU_COR_OBJ_RDIR"
    cache_put "SCDU_COR_LIB_RDIR"

    cache_put "SCDU_FFS_CFG_RDIR"
    cache_put "SCDU_FFS_OBJ_RDIR"
    cache_put "SCDU_FFS_LIB_RDIR"

    cache_put "SCDU_ALG_CFG_RDIR"
    cache_put "SCDU_ALG_OBJ_RDIR"
    cache_put "SCDU_ALG_LIB_RDIR"

    cache_put "SCDU_APP_BIN"
    cache_put "SCDU_APP_BIN_RPATH"

    cache_put "SCDU_COR_LIB"
    cache_put "SCDU_COR_LIB_RPATH"

    cache_put "SCDU_FFS_LIB"
    cache_put "SCDU_FFS_LIB_RPATH"

    cache_put "SCDU_ALG_LIB"
    cache_put "SCDU_ALG_LIB_RPATH"

    cache_put "SCDU_DOC_UG"
    cache_put "SCDU_DOC_UGH"
    cache_put "SCDU_DOC_UGM"
    cache_put "SCDU_DOC_UGT"
    cache_put "SCDU_DOC_UGH_RPATH"
    cache_put "SCDU_DOC_UGM_RPATH"
    cache_put "SCDU_DOC_UGT_RPATH"

    cache_put "SCDU_DOC_DG"
    cache_put "SCDU_DOC_DGT"
    cache_put "SCDU_DOC_DGM"
    cache_put "SCDU_DOC_DGH"
    cache_put "SCDU_DOC_DGT_RPATH"
    cache_put "SCDU_DOC_DGM_RPATH"
    cache_put "SCDU_DOC_DGH_RPATH"

    cache_put "SCDU_PKB"
    cache_put "SCDU_PKB_RPATH"

    cache_put "SCDU_PKS"
    cache_put "SCDU_PKS_RPATH"

    cache_put "SCDU_PKD"
    cache_put "SCDU_PKD_RPATH"

    cache_put "SCDU_LIBS"

    cache_put "SCDU_COR_LIB_DEPS"
    cache_put "SCDU_FFS_LIB_DEPS"
    cache_put "SCDU_ALG_LIB_DEPS"
    cache_put "SCDU_APP_BIN_DEPS"
    cache_put "SCDU_DOC_UGT_DEPS"
    cache_put "SCDU_DOC_UGM_DEPS"
    cache_put "SCDU_DOC_UGH_DEPS"
    cache_put "SCDU_DOC_DGT_DEPS"
    cache_put "SCDU_DOC_DGM_DEPS"
    cache_put "SCDU_DOC_DGH_DEPS"

    cache_put "SCDU_CFLAGS"
    cache_put "SCDU_LFLAGS"

    cache_put "SCDU_PATH"
    cache_put "PATH"

    cache_put "SCDU_CONFIGURED"

    scdu_trap_clr
}

################################################################################

# cache_load

# Loads all cacheable variables from config cache (assumed to be present)

cache_load() {

    scdu_trap_set

    source "$SCDU_CACHE_RPATH"

    scdu_trap_clr

    if [[ "$SCDU_CONFIGURED" != "1" ]]; then
        scdu_err "incomplete config cache (try build clean)"
    fi
}

################################################################################

# config <arch> <mode> [refresh]

# Performs arch/mode-specific configuration for detected host/os. To enhance
# build speeds during typical compile-edit cycles, fast cache retrieval of
# config data is employed UNLESS the 'refresh' argument is specified. Note
# that multiple config calls can occur in a "build all" scenario so we must
# be careful about carrying forward artifacts from previous configurations
# e.g. in the SCDU_PATH variable which is re-initialised for each config.

config() {

    local t p d f

    SCDU_CONFIGURED="0"

    ############################################################################
    # validate arguments
    ############################################################################

    if [[ "$1" == "" ]] || [[ "$2" == "" ]]; then
        scdu_bug "missing config arguments"
    fi

    if ! [[ "$SCDU_HOST_ARCHS " =~ "$1 " ]]; then
        scdu_bug "unexpected $SCDU_HOST arch: $1"
    fi

    if ! [[ "$SCDU_HOST_MODES " =~ "$2 " ]]; then
        scdu_bug "unexpected $SCDU_HOST mode: $2"
    fi

    SCDU_ARCH="$1"
    SCDU_MODE="$2"

    if [[ "$3" != "" ]] && [[ "$3" != "refresh" ]]; then
        scdu_bug "invalid config argument: $3"
    fi

    ############################################################################
    # reload cache (if available) unless optional refresh argument is specified
    ############################################################################

    t="$SCDU_TMP_RDIR/cache"

    mkdir -p "$t"

    if [ $? -ne 0 ]; then
        scdu_err "unable to create cache directory: $d"
    fi

    SCDU_CACHE_RPATH="$t/${SCDU_HOST}-${SCDU_OS}-${SCDU_ARCH}-${SCDU_MODE}.cache"

    if [[ "$3" != "refresh" ]] && [[ -f "$SCDU_CACHE_RPATH" ]]; then

        cache_load

        return
    fi

    ############################################################################
    # validate tools common to all configurations
    ############################################################################

    # original bash validation was too early to register properly

    p="$(type -p bash)"

    if [[ "$p" != "" ]]; then
        SCDU_BSH_ADIR="$(dirname "$p")"
    else
        scdu_err "bash detection inconsistency"
    fi

    SCDU_BSH_VERSION="$(find --version | head -n1)"

    if ! [[ "$SCDU_BSH_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "4.6" ]]; then

        scdu_err "bash version inconsistency"
    fi

    ############################################################################

    # original coreutils validation was too early to register properly

    p="$(type -p cp)"

    if [[ "$p" != "" ]]; then
        SCDU_CRU_ADIR="$(dirname "$p")"
    else
        scdu_err "coreutils detection inconsistency"
    fi

    SCDU_CRU_VERSION="$(cp --version | head -n1)"

    if ! [[ "$SCDU_CRU_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "8.2" ]]; then

        scdu_err "coreutils version inconsistency"
    fi

    ############################################################################

    # findutils can be validated via any individual tool e.g. find

    p="$(type -p find)"

    if [[ "$p" != "" ]]; then
        SCDU_FDU_ADIR="$(dirname "$p")"
    else
        scdu_err "missing findutils"
    fi

    SCDU_FDU_VERSION="$(find --version | head -n1)"

    if ! [[ "$SCDU_FDU_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "4.6" ]]; then

        scdu_err "findutils 4.6 or later required"
    fi

    ############################################################################

    # grep is a standalone tool

    p="$(type -p grep)"

    if [[ "$p" != "" ]]; then
        SCDU_GRP_ADIR="$(dirname "$p")"
    else
        scdu_err "missing grep"
    fi

    SCDU_GRP_VERSION="$(grep --version | head -n1)"

    if ! [[ "$SCDU_GRP_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "2.2" ]]; then

        scdu_err "findutils 2.2 or later required"
    fi

    ############################################################################

    # sed is a standalone tool

    p="$(type -p sed)"

    if [[ "$p" != "" ]]; then
        SCDU_SED_ADIR="$(dirname "$p")"
    else
        scdu_err "missing sed"
    fi

    SCDU_SED_VERSION="$(sed --version | head -n1)"

    if ! [[ "$SCDU_SED_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "4.2" ]]; then

        scdu_err "sed 4.2 or later required"
    fi

    ############################################################################

    # awk is a standalone tool

    p="$(type -p awk)"

    if [[ "$p" != "" ]]; then
        SCDU_AWK_ADIR="$(dirname "$p")"
    else
        scdu_err "missing awk"
    fi

    SCDU_AWK_VERSION="$(awk --version | head -n1)"

    if ! [[ "$SCDU_AWK_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "4.1" ]]; then

        scdu_err "awk 4.1 or later required"
    fi

    ############################################################################

    # groff is a standalone tool

    p="$(type -p groff)"

    if [[ "$p" != "" ]]; then
        SCDU_GRF_ADIR="$(dirname "$p")"
    else
        scdu_err "missing groff"
    fi

    SCDU_GRF_VERSION="$(groff --version | head -n1)"

    if ! [[ "$SCDU_GRF_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "1.2" ]]; then

        scdu_err "findutils 1.2 or later required"
    fi

    ############################################################################
    # initial config path is the union of all essential tool directories
    ############################################################################

    SCDU_PATH=""

    for d in    $SCDU_BSH_ADIR \
                $SCDU_CRU_ADIR \
                $SCDU_FDU_ADIR \
                $SCDU_GRP_ADIR \
                $SCDU_SED_ADIR \
                $SCDU_AWK_ADIR \
                $SCDU_GRF_ADIR; do

        add_path SCDU_PATH $d

    done

    ############################################################################
    # initial compile and link flags are common to all configs
    ############################################################################

    SCDU_CFLAGS="-c"

    SCDU_CFLAGS+=" -D SCDU_VERSION=\"$SCDU_VERSION\""
    SCDU_CFLAGS+=" -D SCDU_DATETIME=\"$SCDU_DATETIME\""
    SCDU_CFLAGS+=" -D SCDU_HOST=\"$SCDU_HOST\""

    SCDU_CFLAGS+=" -MMD -Wall -Wextra -Wconversion -std=c++14 -fno-rtti -fno-exceptions"

    SCDU_LFLAGS="--static"

    ############################################################################
    # config-dependent path, flags and executable
    ############################################################################

    # All kinds of flag combinations could arise in the future as the number of
    # supported targets expands e.g. arch and mode flags which also have OS
    # dependencies. On that basis, it is better to take a more pain-staking
    # approach even if repetitions arise across OSes.

    case "$SCDU_OS" in

        windows)

            SCDU_CFLAGS+=" -D SCDU_OS_WINDOWS"

            add_path SCDU_PATH "/c/windows/system32"
            add_path SCDU_PATH "/c/PROGRAM FILES (X86)/Pandoc"

            SCDU_BIN_SUFFIX=".exe"

            case "$SCDU_ARCH" in

                i386)

                    ins_path SCDU_PATH "/mingw32/bin"

                    SCDU_CFLAGS+=" -D SCDU_ARCH_I386 -m32"
                    SCDU_LFLAGS+=" -m32"
                    ;;

                amd64)

                    ins_path SCDU_PATH "/mingw64/bin"

                    SCDU_CFLAGS+=" -D SCDU_ARCH_AMD64 -m64"
                    SCDU_LFLAGS+=" -m64"
                    ;;

                *)
                    scdu_bug "unexpected $SCDU_OS arch: $SCDU_ARCH"
                    ;;
            esac

            case "$SCDU_MODE" in

                debug)

                    SCDU_CFLAGS+=" -D SCDU_MODE_DEBUG -O0 -g"
                    SCDU_LFLAGS+=" -g"
                    ;;

                release)

                    SCDU_CFLAGS+=" -D SCDU_MODE_RELEASE -O2 -D NDEBUG"
                    ;;

                *)
                    scdu_bug "unexpected $SCDU_OS mode: $SCDU_MODE"
                    ;;
            esac
            ;;

        linux)

            SCDU_CFLAGS+=" -D SCDU_OS_LINUX"

            add_path SCDU_PATH "/usr/local/bin"

            case "$SCDU_ARCH" in

                i386)

                    SCDU_CFLAGS+=" -D SCDU_ARCH_I386 -m32"
                    SCDU_LFLAGS+=" -m32"
                    ;;

                amd64)

                    SCDU_CFLAGS+=" -D SCDU_ARCH_AMD64 -m64"
                    SCDU_LFLAGS+=" -m64"
                    ;;

                *)
                    scdu_bug "unexpected $SCDU_OS arch: $SCDU_ARCH"
                    ;;
            esac

            case "$SCDU_MODE" in

                debug)

                    SCDU_CFLAGS+=" -D SCDU_MODE_DEBUG -O0 -g"
                    SCDU_LFLAGS+=" -g"
                    ;;

                release)

                    SCDU_CFLAGS+=" -D SCDU_MODE_RELEASE -O2 -D NDEBUG"
                    ;;

                *)
                    scdu_bug "invalid $SCDU_OS mode: $SCDU_MODE"
                    ;;
            esac
            ;;

        darwin)

            SCDU_CFLAGS+=" -D SCDU_OS_DARWIN"

            add_path SCDU_PATH "/usr/local/bin"

            case "$SCDU_ARCH" in

                i386)

                    SCDU_CFLAGS+=" -D SCDU_ARCH_I386 -m32"
                    SCDU_LFLAGS+=" -m32"
                    ;;

                amd64)

                    SCDU_CFLAGS+=" -D SCDU_ARCH_AMD64 -m64"
                    SCDU_LFLAGS+=" -m64"
                    ;;

                *)
                    scdu_bug "unexpected $SCDU_OS arch: $SCDU_ARCH"
                    ;;
            esac

            case "$SCDU_MODE" in

                debug)

                    SCDU_CFLAGS+=" -D SCDU_MODE_DEBUG -O0 -g"
                    SCDU_LFLAGS+=" -g"
                    ;;

                release)

                    SCDU_CFLAGS+=" -D SCDU_MODE_RELEASE -O2 -D NDEBUG"
                    ;;

                *)
                    scdu_bug "unexpected $SCDU_OS mode: $SCDU_MODE"
                    ;;
            esac
            ;;

        *)
            scdu_bug "unexpected OS: $SCDU_OS"
            ;;
    esac

    ############################################################################
    # directories
    ############################################################################

    SCDU_APP_CFG_RDIR="$SCDU_APP_BLD_RDIR/$SCDU_OS/$SCDU_ARCH/$SCDU_MODE"
    SCDU_APP_OBJ_RDIR="$SCDU_APP_CFG_RDIR/obj"
    SCDU_APP_BIN_RDIR="$SCDU_APP_CFG_RDIR/bin"
    SCDU_APP_BIN_ADIR="$SCDU_PRJ_ADIR/$SCDU_APP_BIN_RDIR"

    SCDU_COR_CFG_RDIR="$SCDU_COR_BLD_RDIR/$SCDU_OS/$SCDU_ARCH/$SCDU_MODE"
    SCDU_COR_OBJ_RDIR="$SCDU_COR_CFG_RDIR/obj"
    SCDU_COR_LIB_RDIR="$SCDU_COR_CFG_RDIR/lib"
    SCDU_COR_LIB_ADIR="$SCDU_PRJ_ADIR/$SCDU_COR_LIB_RDIR"

    SCDU_FFS_CFG_RDIR="$SCDU_FFS_BLD_RDIR/$SCDU_OS/$SCDU_ARCH/$SCDU_MODE"
    SCDU_FFS_OBJ_RDIR="$SCDU_FFS_CFG_RDIR/obj"
    SCDU_FFS_LIB_RDIR="$SCDU_FFS_CFG_RDIR/lib"
    SCDU_FFS_LIB_ADIR="$SCDU_PRJ_ADIR/$SCDU_FFS_LIB_RDIR"

    SCDU_ALG_CFG_RDIR="$SCDU_ALG_BLD_RDIR/$SCDU_OS/$SCDU_ARCH/$SCDU_MODE"
    SCDU_ALG_OBJ_RDIR="$SCDU_ALG_CFG_RDIR/obj"
    SCDU_ALG_LIB_RDIR="$SCDU_ALG_CFG_RDIR/lib"
    SCDU_ALG_LIB_ADIR="$SCDU_PRJ_ADIR/$SCDU_ALG_LIB_RDIR"

    # build directory tree is automatically maintained

    for d in $SCDU_APP_OBJ_RDIR $SCDU_APP_BIN_RDIR \
             $SCDU_COR_OBJ_RDIR $SCDU_COR_LIB_RDIR \
             $SCDU_FFS_OBJ_RDIR $SCDU_FFS_LIB_RDIR \
             $SCDU_ALG_OBJ_RDIR $SCDU_ALG_LIB_RDIR \
             $SCDU_DOC_BLD_RDIR; do

        mkdir -p "$d"
        if [ $? -ne 0 ]; then
            scdu_err "unable to create directory: $d"
        fi
    done

    ############################################################################
    # files
    ############################################################################

    SCDU_APP_BIN="scdu$SCDU_BIN_SUFFIX"
    SCDU_APP_BIN_RPATH="$SCDU_APP_BIN_RDIR/$SCDU_APP_BIN"

    SCDU_COR_LIB="libcore.a"
    SCDU_COR_LIB_RPATH="$SCDU_COR_LIB_RDIR/$SCDU_COR_LIB"

    SCDU_FFS_LIB="libffs.a"
    SCDU_FFS_LIB_RPATH="$SCDU_FFS_LIB_RDIR/$SCDU_FFS_LIB"

    SCDU_ALG_LIB="libpat.a"
    SCDU_ALG_LIB_RPATH="$SCDU_ALG_LIB_RDIR/$SCDU_ALG_LIB"

    SCDU_DOC_UG="scdu-user-guide"
    SCDU_DOC_UGT="$SCDU_DOC_UG.txt"
    SCDU_DOC_UGM="$SCDU_DOC_UG.man"
    SCDU_DOC_UGH="$SCDU_DOC_UG.html"
    SCDU_DOC_UGT_RPATH="$SCDU_DOC_BLD_RDIR/$SCDU_DOC_UGT"
    SCDU_DOC_UGM_RPATH="$SCDU_DOC_BLD_RDIR/$SCDU_DOC_UGM"
    SCDU_DOC_UGH_RPATH="$SCDU_DOC_BLD_RDIR/$SCDU_DOC_UGH"

    SCDU_DOC_DG="scdu-dev-guide"
    SCDU_DOC_DGT="$SCDU_DOC_DG.txt"
    SCDU_DOC_DGM="$SCDU_DOC_DG.man"
    SCDU_DOC_DGH="$SCDU_DOC_DG.html"
    SCDU_DOC_DGT_RPATH="$SCDU_DOC_BLD_RDIR/$SCDU_DOC_DGT"
    SCDU_DOC_DGM_RPATH="$SCDU_DOC_BLD_RDIR/$SCDU_DOC_DGM"
    SCDU_DOC_DGH_RPATH="$SCDU_DOC_BLD_RDIR/$SCDU_DOC_DGH"

    SCDU_PKB="$SCDU_PRJ-bin-$SCDU_OS-$SCDU_ARCH-$SCDU_MODE.tar.gz"
    SCDU_PKB_RPATH="$SCDU_PKG_RDIR/$SCDU_PKB"

    SCDU_PKS="$SCDU_PRJ-src.tar.gz"
    SCDU_PKS_RPATH="$SCDU_PKG_RDIR/$SCDU_PKS"

    SCDU_PKD="$SCDU_PRJ-doc.tar.gz"
    SCDU_PKD_RPATH="$SCDU_PKG_RDIR/$SCDU_PKD"

    ############################################################################
    # libs
    ############################################################################

    SCDU_LIBS="$SCDU_ALG_LIB_RPATH"
    SCDU_LIBS+=" $SCDU_FFS_LIB_RPATH"
    SCDU_LIBS+=" $SCDU_COR_LIB_RPATH"

    ############################################################################
    # make dependencies
    ############################################################################

    SCDU_COR_LIB_DEPS=""
    for f in $SCDU_COR_SRC_RDIR/*.cpp; do
        SCDU_COR_LIB_DEPS+="$SCDU_COR_OBJ_RDIR/$(basename $f .cpp).o "
    done

    SCDU_FFS_LIB_DEPS=""
    for f in $SCDU_FFS_SRC_RDIR/*.cpp; do
        SCDU_FFS_LIB_DEPS+="$SCDU_FFS_OBJ_RDIR/$(basename $f .cpp).o "
    done

    SCDU_ALG_LIB_DEPS=""
    for f in $SCDU_ALG_SRC_RDIR/*.cpp; do
        SCDU_ALG_LIB_DEPS+="$SCDU_ALG_OBJ_RDIR/$(basename $f .cpp).o "
    done

    SCDU_APP_BIN_DEPS=""
    for f in $SCDU_APP_SRC_RDIR/*.cpp; do
        SCDU_APP_BIN_DEPS+="$SCDU_APP_OBJ_RDIR/$(basename $f .cpp).o "
    done

    SCDU_APP_BIN_DEPS+="$SCDU_LIBS"

    ############################################################################

    SCDU_DOC_UGT_DEPS="$SCDU_APP_BIN_RPATH"

    SCDU_DOC_UGM_DEPS="$SCDU_DOC_UGT_RPATH"

    SCDU_DOC_UGH_DEPS="$SCDU_DOC_UGT_RPATH"
    SCDU_DOC_UGH_DEPS+=" $SCDU_DOC_CSS_RPATH"

    ############################################################################

    SCDU_DOC_DGT_DEPS="$SCDU_APP_BIN_RPATH"

    SCDU_DOC_DGT_DEPS+=" *.txt"
    SCDU_DOC_DGT_DEPS+=" $SCDU_DOC_SRC_RDIR/*.txt"
    SCDU_DOC_DGT_DEPS+=" $SCDU_APP_SRC_RDIR/*.txt"
    SCDU_DOC_DGT_DEPS+=" $SCDU_COR_SRC_RDIR/*.txt"
    SCDU_DOC_DGT_DEPS+=" $SCDU_FFS_SRC_RDIR/*.txt"
    SCDU_DOC_DGT_DEPS+=" $SCDU_ALG_SRC_RDIR/*.txt"

    SCDU_DOC_DGM_DEPS="$SCDU_DOC_DGT_RPATH"

    SCDU_DOC_DGH_DEPS="$SCDU_DOC_DGT_RPATH"
    SCDU_DOC_DGH_DEPS+=" $SCDU_DOC_CSS_RPATH"

    ############################################################################
    # finalise path and assign to system
    ############################################################################

    # place fresh scdu binary at start of path to preempt possible clashes with
    # an existing scdu install

    ins_path SCDU_PATH "$SCDU_APP_BIN_ADIR"

    # note that we are assigning to system path -- not extending!!!
    # i.e. we want a minimal path supporting only the tools that we require,
    # free of unpredictable complications might arise with extraneous entries

    PATH="$SCDU_PATH"

    ############################################################################
    # tool checks (only perform after updating system path)
    ############################################################################

    # although config-agnostic tools have already been validated,
    # check for inconsistencies after updating system path

    if [[ "$(dirname $(type -p bash))" != "$SCDU_BSH_ADIR" ]]; then
        scdu_err "bash inconsistency after path update"
    fi

    if [[ "$(dirname $(type -p cp))" != "$SCDU_CRU_ADIR" ]]; then
        scdu_err "coreutils inconsistency after path update"
    fi

    if [[ "$(dirname $(type -p find))" != "$SCDU_FDU_ADIR" ]]; then
        scdu_err "findutils inconsistency after path update"
    fi

    if [[ "$(dirname $(type -p grep))" != "$SCDU_GRP_ADIR" ]]; then
        scdu_err "grep inconsistency after path update"
    fi

    if [[ "$(dirname $(type -p sed))" != "$SCDU_SED_ADIR" ]]; then
        scdu_err "sed inconsistency after path update"
    fi

    if [[ "$(dirname $(type -p awk))" != "$SCDU_AWK_ADIR" ]]; then
        scdu_err "awk inconsistency after path update"
    fi

    if [[ "$(dirname $(type -p groff))" != "$SCDU_GRF_ADIR" ]]; then
        scdu_err "groff inconsistency after path update"
    fi

    ############################################################################

    # gcc is fundamental and has a strict correllation with scdu version

    p="$(type -p g++)"

    if [[ "$p" != "" ]]; then
        SCDU_GCC_ADIR="$(dirname "$p")"
    else
        scdu_err "missing g++"
    fi

    SCDU_GCC_VERSION="$(g++ --version | head -n1)"

    if ! [[ "$SCDU_GCC_VERSION" =~ "6.2.0" ]]; then
        scdu_err "gcc 6.2.0 required for current scdu version"
    fi

    ############################################################################

    # pandoc is fundamental and has a strict correllation with scdu version

    p="$(type -p pandoc)"

    if [[ "$p" != "" ]]; then
        SCDU_PAN_ADIR="$(dirname "$p")"
    else
        scdu_err "missing pandoc"
    fi

    SCDU_PAN_VERSION="$(pandoc --version | head -n1)"

    if ! [[ "$SCDU_PAN_VERSION" =~ "1.17.2" ]]; then
        scdu_err "pandoc 1.17.2 required for current scdu version"
    fi

    ############################################################################

    # as a general-purpose utility, make has a more relaxed version requirement

    p="$(type -p make)"

    if [[ "$p" != "" ]]; then
        SCDU_MAK_ADIR="$(dirname "$p")"
    else
        scdu_err "missing make"
    fi

    SCDU_MAK_VERSION="$(make --version | head -n1)"

    if ! [[ "$SCDU_MAK_VERSION" =~ [0-9][.][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "4.2" ]]; then

        scdu_err "make 4.2 or later required"
    fi

    ############################################################################

    # binutils can be validated via any individual tool e.g. ar

    p="$(type -p ar)"

    if [[ "$p" != "" ]]; then
        SCDU_BNU_ADIR="$(dirname "$p")"
    else
        scdu_err "missing binutils"
    fi

    SCDU_BNU_VERSION="$(ar --version | head -n1)"

    if ! [[ "$SCDU_BNU_VERSION" =~ [0-9][.][0-9][0-9] ]] \
    ||   [[ "${BASH_REMATCH[0]}" < "2.27" ]]; then

        scdu_err "binutils 2.27 or later required"
    fi

    ############################################################################
    # save configured data to cache for next time
    ############################################################################

    SCDU_CONFIGURED="1"
    cache_save
}

################################################################################

# build <target> [<action>]

# Builds target for current configuration

build() {

    local scope

    if [[ "$SCDU_CONFIGURED" != "1" ]]; then
        scdu_bug "unconfigured build"
    fi

    scope="$SCDU_PRJ: "

    case $1 in
               clean) scope+="machine-generated files" ;;
            sanitise) scope+="human-generated files" ;;
                 pks) scope+="source files and development resources" ;;
                 all) scope+="$SCDU_OS:$SCDU_HOST" ;;
                   *) scope+="$SCDU_OS-$SCDU_ARCH-$SCDU_MODE" ;;
    esac

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "build $1 $2" "$scope"
    scdu_msg "$SCDU_BDIV"
    scdu_msg

    # 'check' action recaps variables without building target
    # performed as a 'make' to verify that vars are exported correctly

    if [[ "$2" == "check" ]]; then
        scdu_make check
        return
    fi

    # 'all' target involves multiple build configurations and targets
    # which will require separate calls to this function

    if [[ "$1" == "all" ]]; then
        build_all
        return
    fi

    # all other targets and actions involve a 'normal' make invocation

    scdu_make "$1"

    case "$2" in
          "") return ;;
         run) run ;;
        view) view ;;
           *) scdu_bug "invalid build action: $2" ;;
    esac
}

################################################################################

# build_all (no arguments)

# Build from scratch all binary, doc and package targets supported by host

build_all() {

    local arch mode

    scdu_msg "archs" "$SCDU_HOST_ARCHS"
    scdu_msg "modes" "$SCDU_HOST_MODES"

    # iterate through various arch and mode configurations
    # while imposing build order rather than leaving it up to make

    build clean
    build sanitise

    for arch in $SCDU_HOST_ARCHS; do
        for mode in $SCDU_HOST_MODES; do

            config $arch $mode refresh

            build core
            build ffs
            build alg
            build app

            build pkb

        done
    done

    # since doc generation depends on app binary
    # restore host arch and mode defaults for build consistency

    config $SCDU_HOST_ARCHS_DEF $SCDU_HOST_MODES_DEF refresh
    build doc
    build pkd

    # finally, if all well, build source package (config-agnostic)

    build pks

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "build summary"
    scdu_msg "$SCDU_BDIV"
    scdu_msg
    scdu_msg "scdu version"     "$SCDU_VERSION"
    scdu_msg "build date/time"  "$SCDU_DATETIME"
    scdu_msg
    scdu_msg "bash"             "$SCDU_BSH_VERSION"
    scdu_msg "coreutils"        "$SCDU_CRU_VERSION"
    scdu_msg "findutils"        "$SCDU_FDU_VERSION"
    scdu_msg "grep"             "$SCDU_GRP_VERSION"
    scdu_msg "sed"              "$SCDU_SED_VERSION"
    scdu_msg "awk"              "$SCDU_AWK_VERSION"
    scdu_msg "groff"            "$SCDU_GRF_VERSION"
    scdu_msg "gcc"              "$SCDU_GCC_VERSION"
    scdu_msg "pandoc"           "$SCDU_PAN_VERSION"
    scdu_msg "make"             "$SCDU_MAK_VERSION"
    scdu_msg "binutils"         "$SCDU_BNU_VERSION"
    scdu_msg
    scdu_msg "os"               "$SCDU_OS"
    scdu_msg "os archs"         "$SCDU_OS_ARCHS"
    scdu_msg "os modes"         "$SCDU_OS_MODES"
    scdu_msg
    scdu_msg "host"             "$SCDU_HOST"
    scdu_msg "host archs"       "$SCDU_HOST_ARCHS"
    scdu_msg "host modes"       "$SCDU_HOST_MODES"
    scdu_msg
    scdu_msg "archs completed"  "$SCDU_HOST_ARCHS_NUM/$SCDU_OS_ARCHS_NUM $SCDU_OS archs"
    scdu_msg "modes completed"  "$SCDU_HOST_MODES_NUM/$SCDU_OS_MODES_NUM $SCDU_OS modes"
}

################################################################################

# run (no arguments)

# Run previouly-built app target in controlled environment

run() {

    local tmpdir="$SCDU_PRJ_ADIR/$SCDU_TMP_RDIR/run"
    local timeref="$tmpdir/timeref"
    local oldlist="$tmpdir/oldlist"
    local newlist="$tmpdir/newlist"
    local addlist="$tmpdir/addlist"
    local dellist="$tmpdir/dellist"
    local reclist="$tmpdir/reclist"
    local updlist="$tmpdir/updlist"

    local xcode beg end sec dur f

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "run" "$SCDU_RUNSH shell"
    scdu_msg "$SCDU_BDIV"
    scdu_msg

    pushd $SCDU_RUN_RDIR > /dev/null

    mkdir -p "$tmpdir"
    touch "$timeref"

    find -printf '%P\n' | awk NF > $oldlist

    scdu_log "(not logged)"

    case "$SCDU_RUNSH" in

        current)

            beg=$(date +%s)

            $SCDU_RUN_CMD

            xcode=$?
            end=$(date +%s)

            sec=$((end-beg))
            dur="$((sec/3600))h:$((sec%3600/60))m:$((sec%60))s"
            ;;

        command)

            xcode="n/a"
            dur="n/a"

            case "$SCDU_OS" in

                windows)
                    cmd //c "$SCDU_RUN_CMD"
                    ;;

                linux)
                    bash -c "$SCDU_RUN_CMD"
                    ;;

                darwin)
                    bash -c "$SCDU_RUN_CMD"
                    ;;

                *)
                    scdu_bug "unexpected OS: $SCDU_OS"
                    ;;
            esac
            ;;

        terminal)

            xcode="n/a"
            dur="n/a"

            scdu_out "opening terminal"

            case "$SCDU_OS" in

                windows)
                    cmd //c "start /wait cmd /c cmd /k $SCDU_RUN_CMD"
                    ;;

                linux)
                    scdu_bug "terminal launch not yet implelemented for $SCDU_OS"
                    ;;

                darwin)
                    scdu_bug "terminal launch not yet implelemented for $SCDU_OS"
                    ;;

                *)
                    scdu_bug "unexpected OS: $SCDU_OS"
                    ;;
            esac

            scdu_out
            scdu_out "terminal closed"
            ;;

        *)
            scdu_bug "invalid SCDU_RUNSH: $SCDU_RUNSH"
            ;;

    esac

    find -printf '%P\n' | awk NF > $newlist
    find -newer $timeref -printf '%P\n' | awk NF > $reclist

    grep -Fxvf $oldlist $newlist > $addlist
    grep -Fxvf $newlist $oldlist > $dellist
    grep -Fxvf $addlist $reclist > $updlist

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "run summary"
    scdu_msg "$SCDU_BDIV"
    scdu_msg
    scdu_msg "binary"       "$SCDU_APP_BIN_RPATH"
    scdu_msg "directory"    "$SCDU_RUN_RDIR"
    scdu_msg "script"       "run.sh"
    scdu_msg "shell"        "$SCDU_RUNSH"
    scdu_msg "command"      "$SCDU_RUN_CMD"
    scdu_msg "exit code"    "$xcode"
    scdu_msg "duration"     "$dur"

    scdu_msg
    scdu_msg "$SCDU_BDIV"
    scdu_msg "run changes"
    scdu_msg "$SCDU_BDIV"

    scdu_msg

    if   [ -s $dellist ]; then
        for f in $(cat $dellist); do
            scdu_msg "deleted" "$f"
        done
    elif [ -s $addlist ]; then
        for f in $(cat $addlist); do
            scdu_msg "added" "$f"
        done
    elif [ -s $updlist ]; then
        for f in $(cat $updlist); do
            scdu_msg "updated" "$f"
        done
    else
        scdu_msg "no changes"
    fi

    popd > /dev/null
}

################################################################################

# view (no arguments)

# View one or more doc files generated by a previously-built target

view() {

    local views f x

    # in case of multiple views, order by extension text/man/html

    case $SCDU_TARGET in
        doc|ugs|ugt) views+=" $SCDU_DOC_UGT_RPATH" ;;&
        doc|dgs|dgt) views+=" $SCDU_DOC_DGT_RPATH" ;;&
        doc|ugs|ugm) views+=" $SCDU_DOC_UGM_RPATH" ;;&
        doc|dgs|dgm) views+=" $SCDU_DOC_DGM_RPATH" ;;&
        doc|ugs|ugh) views+=" $SCDU_DOC_UGH_RPATH" ;;&
        doc|dgs|dgh) views+=" $SCDU_DOC_DGH_RPATH" ;;
    esac

    # unviewable target should have been caught by init()

    if [[ "$views" == "" ]]; then
        scdu_bug "unviewable target"
    fi

    for f in $views; do

        scdu_msg
        scdu_msg "$SCDU_BDIV"
        scdu_msg "view" "$f"
        scdu_msg "$SCDU_BDIV"
        scdu_msg

        x="${f##*.}"

        case $x in

            txt)

                cat $f
                ;;

            man)

                groff -man -Tascii < $f
                ;;

            html)

                scdu_msg "(viewing in browser)"

                case "$SCDU_OS" in
                    windows) cmd //c  "start /wait $f" ;;
                      linux) scdu_err "$SCDU_OS HTML viewer not implemented" ;;
                     darwin) scdu_err "$SCDU_OS HTML viewer not implemented" ;;
                          *) scdu_bug "unexpected OS: $SCDU_OS" ;;
                esac
                ;;

            *)
                scdu_bug "unexpected view extension: $x" ;;
        esac

    done
}

################################################################################

# main <command-line arguments>

# Script proper

main() {

    check_env
    check_args "$@"
    check_dir

    # Two possible initial configs:
    # 1. Fresh host defaults (uncached) for build all/clean/sanitise
    # 2. Session defaults (possibly cached) for all other build targets.

    case "$SCDU_TARGET" in

        all|clean|sanitise)

            config $SCDU_HOST_ARCHS_DEF $SCDU_HOST_MODES_DEF refresh
            ;;
        *)

            config $SCDU_DEF_ARCH $SCDU_DEF_MODE
            ;;
    esac

    build $SCDU_TARGET $SCDU_ACTION

    scdu_eok
}

################################################################################
# Last line must contain main() call to ensure that script is fully loaded
# before execution-proper begins. This means the script can reliably update
# itself while running e.g. during a sanitise or repository operation. Script
# updates will then kick-in the next time the script is executed.
################################################################################

main "$@"