// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined FILE_H

    #define FILE_H

    const Size FEM_MAX = 80;

    enum Fen
    {
        FE_OK = 0,
        FE_OPEN,
        FE_SEEK,
        FE_READ,
        FE_WRITE,
        FE_COUNT
    };

    struct FerDef
    {
        Fen         num;
        const char* sym;
        const char* msg;
        const char* params;
    };

    class FileBuffer
    {

    public:
        FileBuffer();
        ~FileBuffer();
        Size cap() const;
        Size len() const;
        Int64 lastCount() const;
        const Str& path() const;
        bool isReserved() const;
        bool isOpen() const;
        void reserve(Size chunks);
        void release();
        virtual void open(const char* path) = 0;
        virtual void close() = 0;
        virtual Int64 size() const = 0;
        virtual Int64 pos() const = 0;

    protected:
        Uint8*  mBase;                  // buffer allocation base address
        Uint8*  mTop;                   // buffer allocation top address
        Uint8*  mStart;                 // enqueued data start address
        Uint8*  mEnd;                   // enqueued data end address
        Int64   mLastCount;             // last fill/flush count for current file
        File*   mFile;                  // stream associated with current file
        Str     mPath;                  // path of current file
    };

    class FileReader : public FileBuffer
    {
    friend class FileWriter;

    public:
        FileReader();
        ~FileReader();
        FileReader(const FileReader&) = delete;
        FileReader& operator=(const FileReader&) = delete;
        void open(const char* path);
        void close();
        Int64 size() const;
        Int64 pos() const;
        void fill();
        Uint8 get();

    private:
        Int64 mSize;
    };

    class FileWriter : public FileBuffer
    {
    friend class FileReader;

    public:
        FileWriter();
        ~FileWriter();
        FileWriter(const FileWriter&) = delete;
        FileWriter& operator=(const FileReader&) = delete;
        void open(const char* path);
        void close();
        Int64 size() const;
        Int64 pos() const;
        void flush();
        void flushToDisk();
        void put(Uint8 data);
        void put(FileReader& r, Int64 count = -1);
    };

    extern const Fen&   fen;
    extern const char*  const fem;
    extern const FerDef ferDefs[FE_COUNT];

    extern void ferClear();

#endif // FILE_H

// EOF