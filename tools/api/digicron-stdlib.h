typedef unsigned long size_t;

namespace dc::heap {
    typedef size_t Block;

    extern const Block* base;
    extern Block* firstFreeBlock;
}

extern "C" {
    void* memset(void* destination, char value, size_t size);
    void* memcpy(void* destination, void* source, size_t size);
    void* malloc(size_t size);
    void free(void* ptr);
    void* calloc(size_t count, size_t size);
    void* realloc(void* ptr, size_t size);
}

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, size_t size) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, size_t size) noexcept;

extern "C" int __cxa_atexit(void (*function)(void*), void* argument, void* handle);