
#include <stdio.h>
#include <stdlib.h>
#include <new>

#include "SkTypes.h"
#include "SkThread.h"

// 这里sk_malloc_flags()的实现以及相关函数与SkMemory_malloc.cpp一致,
// 只是在malloc()时, 如果没设置SK_MALLOC_THROW, 禁止CRT的new_handler
// (也就是想要sk_malloc_flags()在NULL时不抛异常).

static SkMutex gSkNewHandlerMutex;

void sk_throw()
{
    SkASSERT(!"sk_throw");
    abort();
}

void sk_out_of_memory(void)
{
    SkASSERT(!"sk_out_of_memory");
    abort();
}

void* sk_malloc_throw(size_t size)
{
    return sk_malloc_flags(size, SK_MALLOC_THROW);
}

void* sk_realloc_throw(void* addr, size_t size)
{
    void* p = realloc(addr, size);
    if(size == 0)
    {
        return p;
    }
    if(p == NULL)
    {
        sk_throw();
    }
    return p;
}

void sk_free(void* p)
{
    if(p)
    {
        free(p);
    }
}

void* sk_malloc_flags(size_t size, unsigned flags)
{
    void* p;
    if(!(flags & SK_MALLOC_THROW))
    {
        SkAutoMutexAcquire lock(gSkNewHandlerMutex);
        std::new_handler old_handler = std::set_new_handler(NULL);
        p = malloc(size);
        std::set_new_handler(old_handler);
    }
    else
    {
        p = malloc(size);
    }
    if(p == NULL)
    {
        if(flags & SK_MALLOC_THROW)
        {
            sk_throw();
        }
    }
    return p;
}