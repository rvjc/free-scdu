// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
#error "do not include value.h separately - use core.h instead!"
#endif

class Value
{
public:
    Value();
    bool isNull() const;
    TypNum typNum() const;
    QtyNum qtyNum() const;
    const Flag& flag() const;
    const Fnum& fnum() const;
    const Inum& inum() const;
    const Pick& pick() const;
    const Text& text() const;
    void setNull();
    void setFlag(const Flag& flag, const Var* var);
    void setText(const Text& text, const Var* var);
    void setInum(const Inum& inum, const Var* var);
    void setFnum(const Fnum& fnum, const Var* var);
    void setPick(const Pick& pick, const Var* var);
    void parse(const char* s, const Var* var);
    int format(char* s, Size max, FmtSpec spec) const;

private:
    const Var*  mVar;

    // note: cannot use union due to non-trivial types

    Flag        mFlag;
    Text        mText;
    Inum        mInum;
    Fnum        mFnum;
    Pick        mPick;
};

// EOF