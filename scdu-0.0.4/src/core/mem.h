// Copyright 2015-2016 RVJ Callanan.
// Released under the GNU General Public License (Version 3).

#if !defined CORE_INCLUDE
    #error "do not include mem.h separately - use core.h instead!"
#endif

struct Allocs
{
    Allocs();
    Size cur;
    Size max;
};

extern const Allocs& allocs;

#define memAlloc(a, s) memAlloc_(a, s, CUR_FUNC, CUR_FILE, CUR_LINE)

template <typename T>
void memAlloc_(    T** addr_ptr,
                    Size size,
                    const char* func,
                    const char* file,
                    int line )
{
    extern Allocs allocs_m;

    size >>= sizeof(T) - 1;

    if ( size == 0 )
    {
        panic(func, file, line, "memory allocation size is zero");
    }

    if ( *addr_ptr != 0 )
    {
        panic(func, file, line, "memory already allocated (or pointer illegally set)");
    }

    *addr_ptr = (T*) malloc(size);

    if ( *addr_ptr == 0 )
    {
        xer(XE_MEMOUT);
    }

    allocs_m.cur += size;

    if ( allocs_m.cur > allocs_m.max )
    {
        allocs_m.max = allocs_m.cur;
    }
}

#define memRealloc(a, n, o) memRealloc_(a, n, o, CUR_FUNC, CUR_FILE, CUR_LINE)

template <typename T>
void memRealloc_(  T** addr_ptr,
                    Size new_size,
                    Size old_size_chk,
                    const char* func,
                    const char* file,
                    int line )
{
    extern Allocs allocs_m;
    void*  ptr;
    Size   old_size;

    ptr = (void*) *addr_ptr;
    old_size = mallocSize(ptr);

    new_size >>= sizeof(T) - 1;
    old_size_chk >>= sizeof(T) - 1;

    if ( new_size == 0 )
    {
        panic(func, file, line, "memory realloc size is zero");
    }

    if ( ptr == 0 )
    {
        panic(func, file, line, "realloc memory already freed (or pointer illegally cleared)");
    }

    if ( old_size != old_size_chk )
    {
        panic(func, file, line, "realloc memory size check failed");
    }

    *addr_ptr = (T*) realloc(ptr, new_size);

    if (*addr_ptr == 0)
    {
        xer(XE_MEMOUT);
    }

    allocs_m.cur += (new_size - old_size);

    if ( allocs_m.cur > allocs_m.max )
    {
        allocs_m.max = allocs_m.cur;
    }
}

#define memFree(a, s) memFree_(a, s, CUR_FUNC, CUR_FILE, CUR_LINE)

template <typename T>
void memFree_( T** addr_ptr,
                Size size_chk,
                const char* func,
                const char* file,
                int line )
{
    extern Allocs allocs_m;
    Size    size;
    void*   ptr;

    size_chk >>= sizeof(T) - 1;

    ptr = (void*) *addr_ptr;

    if ( ptr == 0 )
    {
        panic(func, file, line, "memory already freed (or pointer illegally cleared)");

    }

    size = mallocSize(ptr);

    if ( size_chk != 0 && size != size_chk )
    {
        panic(func, file, line, "free memory size check failed");
    }

    free(ptr);
    *addr_ptr = 0;

    allocs_m.cur -= size;

    ASSERT(allocs_m.cur >= 0);
}

// EOF