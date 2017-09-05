// Copyright 2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <signal.h>

#include "../core/core.h"

#include "help.h"

void help()
{
    Str                 t;
    Str                 d;
    Str                 min;
    Str                 max;
    Str                 sel;
    int                 sw, nw;

    if ( !cmd.options.rawReporting )
    {
        outR("SCDU Help");
        outR("=========");
        outR();
    }

    outR("SCDU (pronounced \"skidoo\") is a versatile command-line toolkit for");
    outR("analysing, generating and manipulating data.");
    outR();
    outR("SCDU is freely distributed as a series of stand-alone executables");
    outR("supporting a wide range of operating systems and CPU architectures.");
    outR("No installers are required; just copy the appropriate executable to a");
    outR("binary directory on the host or run directly from an external device");
    outR("such as a USB stick.");
    outR();
    outR("SCDU is designed from the ground up to pack as much functionality as");
    outR("possible into a compact binary with no external dependencies beyond a");
    outR("basic OS stack. This means that it is equally at home in embedded");
    outR("devices, boot disks, virtual machines and high-end server clusters.");
    outR();
    outR("SCDU is a veritable \"Swiss Army Knife\" for programmers, sysadmins and");
    outR("data forensics experts in the varied landscape of the Internet Age.");
    outR("It's terse, platform-agnostic and script-friendly interface maximises");
    outR("productivity without compromising performance and reliability. These");
    outR("attributes also make SCDU the engine-of-choice for higher level data");
    outR("utilities and automation solutions");
    outR();
    outR("SCDU is also a useful educational tool for exploring file formats and");
    outR("data protocols.");
    outR();
    outR("SCDU is an acronym for \"Super-Consolidated Data Utility\" while owing");
    outR("its speedy character to the timeless expression \"23 Skidoo\".");
    outR();
    outR();

    outR("General Usage");
    outR("-------------");
    outR();
    outR("scdu [?] [action] [options] [parameters]");
    outR();
    outR("Inserting a '?' before any other argument invokes a \"dummy run\" which");
    outR("recaps all options, environment variables and parameters that would");
    outR("have applied if the action had been executed.");
    outR();
    outR("If no arguments are supplied, the help action is invoked.");
    outR();
    outR();

    outR("Actions");
    outR("-------");
    outR();

    for (int a = 0; a < ACT_COUNT; a++)
    {
        const ActDef& def = actDefs[a];

        oufR("scdu [?] %s [options] %s", def.name, def.usage);
        oufR("%s", def.desc);
        outR();
    }

    outR();

    outR("Options");
    outR("-------");
    outR();
    outR("Options have consistent built-in defaults which may be over-ridden");
    outR("via the command-line and/or config file (see -cf option).");
    outR();
    outR("While both methods employ the same syntax, config file options are");
    outR("specified on separate lines which may also include comments and");
    outR("action declarations. The intention is that built-in defaults can be");
    outR("over-ridden in the  \"global\" section at the top of the config file");
    outR("and further customised in the action-specific sections or on the");
    outR("command-line itself.");
    outR();
    outR("Options are processed in the following order:");
    outR();
    outR("1. Options specified on the command-line BEFORE the -cf option.");
    outR("2. Options specified in the config file global section.");
    outR("3. Options specified in the config file action-specific section.");
    outR("4. Options specified on the command-line AFTER the -cf option.");
    outR();
    outR();

    outR("Option Types");
    outR("------------");
    outR();
    outR("Options can have one of the following types:");
    outR();

    for (int t = 0; t < TYP_COUNT; t++)
    {
        const TypDef& td = typDefs[t];
        oufR("%-6s : %s", td.name, td.desc);
    }

    outR();
    outR();

    outR("Option Syntax");
    outR("-------------");
    outR();
    outR("Options can be specified using three methods:");
    outR();
    outR("long:         --<name>[=<value>]  e.g. --progress-width=120 --recurse");
    outR("short:       -<symbol>[=<value>]  e.g.  -pw=120 -r");
    outR("group:  [+|~]<symbols>            e.g.  +rfap");
    outR();
    outR("Long syntax requires descriptive option names which are preferred in");
    outR("config files.");
    outR();
    outR("Short and group syntax requires abbreviated option symbols which are");
    outR("more suited to the command-line where space is at a premium.");
    outR();
    outR("Group syntax can only be used with SINGLE character symbols which are");
    outR("reserved for the most ubiquituous FLAG options. The group prefix");
    outR("determines whether the specified flags are enabled(+) or disabled(~).");
    outR();
    outR("Flag options are disabled by default and can be enabled by specifying");
    outR("the option name or symbol without a value. Explicit values are only");
    outR("required when DISABLING a flag; however, explicit enabling may also");
    outR("be used to add emphasis if a flag was previously disabled.");
    outR();
    outR("Null values are assigned by appending the \"=\" sign to the option name");
    outR("or symbol without a value e.g. -rd=.");
    outR();
    outR("Text values can be assigned with or without quotes. Quotes are only");
    outR("required when the text contains spaces e.g. -lf=\"my log file\".");
    outR();
    outR("Config files support the following additional declarations:");
    outR();
    outR("@<action>     start of action-specific section");
    outR("#             comment");
    outR();
    outR("Leading whitespace is ignored in config files, so indents may be used");
    outR("freely to assist readability.");
    outR();
    outR();

    outR("Options Index");
    outR("-------------");
    outR();

    for (int o = 0; o < OPT_COUNT; o++)
    {
        const OptDef& od = optDefs[o];

        min = od.var.min;
        max = od.var.max;
        sel = od.var.sel;

        t = "<";
        t += typDefs[od.var.typnum].name;
        t += ">";

        switch ( od.var.typnum )
        {
            case TYP_FLAG:

                d = "<disabled>";

                break;

            case TYP_FNUM:
            case TYP_INUM:

                if ( min.len() == 0 && max.len() > 0 )
                {
                    t += " <= ";
                    t += max;
                }
                else if ( min.len() > 0 && max.len() == 0 )
                {
                    t += " >= ";
                    t += min;
                }
                else if ( min.len() > 0 && max.len() > 0 )
                {
                    t += " = [";
                    t += min;
                    t += " to ";
                    t += max;
                    t += "]";
                }

                if ( sel.len() > 0 )
                {
                    if ( min.len() == 0 && max.len() == 0 )
                    {
                        t += " =";
                    }
                    else
                    {
                        t += " or";
                    }

                    t += " [";
                    t += sel;
                    t += "]";
                }

                d = od.dval;

                break;

            case TYP_PICK:

                if ( min.len() > 0 && max.len() > 0 )
                {
                    if ( min == max )
                    {
                        t += " ";
                        t += min;
                        t += min == "1" ? " selection" : " selections";
                    }
                    else
                    {
                        t += " between ";
                        t += min;
                        t += " and ";
                        t += max;
                        t += " selections";
                    }
                }
                else if ( min.len() == 0 && max.len() > 0 )
                {
                    t += " at most ";
                    t += max;
                    t += max == "1" ? " selection" : " selections";
                }
                else if ( min.len() > 0 && max.len() == 0 )
                {
                    t += " at least ";
                    t += min;
                    t += min == "1" ? " selection" : " selections";
                }
                else
                {
                    t += " any or none";
                }

                t += " from [";
                t += sel;
                t += "]";

                d = strlenz(od.dval) ? "<null>" : od.dval;

                break;

            case TYP_TEXT:

                if ( min.len() > 0 )
                {
                    t += " (required)";
                }
                else
                {
                    t += " (optional)";
                }

                d = strlenz(od.dval) ? "<null>" : od.dval;

                break;

            default:

                ASSERT(false);
        }

        sw = OPT_SYM_MAX;
        nw = OPT_NAME_MAX;

        oufR("Option:      -%-*s --%-*s", sw, od.sym, nw, od.name);
        oufR("Type:        %s", t.cb());
        oufR("Default:     %s", d.cb());
        oufR("Description: %s", od.desc);
        outR();
    }

    outR();

    outR("Channels");
    outR("--------");
    outR();
    outR("Program output is routed via channels which are selected using simple");
    outR("letter designations:");
    outR();

    for (int c = 0; c < CH_COUNT; c++)
    {
        const ChanDef& cd = chanDefs[c];
        oufR("%s : %-10s : %s", cd.sym, cd.name, cd.desc);
    }

    outR();

    outR("Channel routing to output streams is controlled using the following");
    outR("options: -rs -rd -rl.");
    outR();
    outR("Note: it is possible to route a given channel to more than one output");
    outR("stream simultaneously.");
    outR();
    outR();

    outR("Command Line Examples");
    outR("---------------------");
    outR();

    for (int a = 0; a < ACT_COUNT; a++)
    {
        const ActDef& ad = actDefs[a];
        oufR("scdu %s %s", ad.name, ad.example);
    }

    outR();
    outR();

    outR("Sample Config File");
    outR("------------------");
    outR();
    outR("# scdu.cfg by Joe Bloggs");
    outR();
    outR("# global options");
    outR();
    outR("    --work-directory=~/scdu");
    outR("    --log-file=default.log");
    outR("    --log-mode=A");
    outR();
    outR("    --progress-width=120");
    outR("    --progress-stats=*");
    outR("    --progress-rate=1.0");
    outR();
    outR("    --ascii-path");
    outR("    --chunk-size=1Mi");
    outR("    --buffer-size=1");
    outR();
    outR("# action-specific options");
    outR();
    outR("@copy");
    outR();
    outR("    # keep separate log and over-write");
    outR();
    outR("    --log-file=copy.log");
    outR("    --log-mode=O");
    outR();
    outR("    --buffer-size=10");
    outR("    --rate-metric=S");
    outR("    --progress-rate=0.5");
    outR("    --summary-stats=D");
    outR();
    outR("@help");
    outR();
    outR("    # just show help output and nothing else");
    outR();
    outR("    --route-dgn=");
    outR("    --route-log=");
    outR();
    outR("# end-of-file");
    outR();
    outR();

    outR("Signals");
    outR("-------");
    outR();
    oufR("%-3d %-12s : %s", SIGINT, "SIGINT", "user interrupt (ctrl C)");
    outR();
    outR();

    outR("Exit Codes");
    outR("----------");
    outR();

    for (int e = 0; e < XE_COUNT; e++)
    {
        const XerDef& ed = xerDefs[e];
        oufR("%-3d %-12s : %s", exitCode(e), ed.sym, ed.msg);
    }

    outR();

    outR("Positive exit codes from 1 to 63 are reserved for OS errors.");
    outR();
    outR("Negative exit codes usually indicate an abnormal termination or bug.");
    outR();
    oufR("Please report suspected bugs to: %s.", SCDU_BUGS);
    outR("A verbose log will assist debugging!");
}

// EOF