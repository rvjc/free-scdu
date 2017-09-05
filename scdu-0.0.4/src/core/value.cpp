// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#include <string.h>

#include "core.h"

Value::Value()
{
    setNull();
}

bool Value::isNull() const
{
    return (mVar == 0);
}

TypNum Value::typNum() const
{
    ASSERT(mVar != 0);
    return mVar->typnum;
}

QtyNum Value::qtyNum() const
{
    ASSERT(mVar != 0);
    return mVar->qtynum;
}

const Flag& Value::flag() const
{
    ASSERT(mVar != 0 && mVar->typnum == TYP_FLAG);
    return mFlag;
}

const Text& Value::text() const
{
    ASSERT(mVar != 0 && mVar->typnum == TYP_TEXT);
    return mText;
}

const Inum& Value::inum() const
{
    ASSERT(mVar != 0 && mVar->typnum == TYP_INUM);
    return mInum;
}

const Fnum& Value::fnum() const
{
    ASSERT(mVar != 0 && mVar->typnum == TYP_FNUM);
    return mFnum;
}

const Pick& Value::pick() const
{
    ASSERT(mVar != 0 && mVar->typnum == TYP_PICK);
    return mPick;
}

void Value::setNull()
{
    mVar = 0;
}

void Value::setFlag(const Flag& flag, const Var* var)
{
    ASSERT(var->typnum == TYP_FLAG);
    mFlag = flag;
    mVar = var;
}

void Value::setText(const Text& text, const Var* var)
{
    ASSERT(var->typnum == TYP_TEXT);
    mText = text;
    mVar = var;
}

void Value::setInum(const Inum& inum, const Var* var)
{
    ASSERT(var->typnum == TYP_INUM);
    mInum = inum;
    mVar = var;
}

void Value::setFnum(const Fnum& fnum, const Var* var)
{
    ASSERT(var->typnum == TYP_FNUM);
    mFnum = fnum;
    mVar = var;
}

void Value::setPick(const Pick& pick, const Var* var)
{
    ASSERT(var->typnum == TYP_PICK);
    mPick = pick;
    mVar = var;
}

void Value::parse(const char* s, const Var* var)
{
    ASSERT(s != 0);

    perClear();
    setNull();

    switch (var->typnum)
    {
        case TYP_FLAG: parseFlag(s, var, mFlag); break;
        case TYP_TEXT: parseText(s, var, mText); break;
        case TYP_INUM: parseInum(s, var, mInum); break;
        case TYP_FNUM: parseFnum(s, var, mFnum); break;
        case TYP_PICK: parsePick(s, var, mPick); break;

        default: ASSERT(false);
    }

    if ( pen == PE_OK ) mVar = var;
}

int Value::format(char* s, Size max, FmtSpec spec) const
{
    if ( mVar == 0 )
    {
        if ( max < 6 ) return -1;
        strcpy(s, "<NULL>");
        return 6;
    }

    switch (mVar->typnum)
    {
        case TYP_FLAG: return formatFlag(s, max, spec, mVar, mFlag);
        case TYP_TEXT: return formatText(s, max, spec, mVar, mText);
        case TYP_INUM: return formatInum(s, max, spec, mVar, mInum);
        case TYP_FNUM: return formatFnum(s, max, spec, mVar, mFnum);
        case TYP_PICK: return formatPick(s, max, spec, mVar, mPick);

        default: ASSERT(false);
    }

    return -1;
}

// EOF