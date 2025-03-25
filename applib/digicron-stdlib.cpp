#include "digicron.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"

#define _DC_ALIGN_SIZE 4
#define _DC_ALIGN(value) (((value) + _DC_ALIGN_SIZE - 1) & ~(_DC_ALIGN_SIZE - 1))

#define _DC_FLAG_USED 0x8000
#define _DC_BLOCK_IS_USED(address) (*(address) & _DC_FLAG_USED)
#define _DC_BLOCK_SIZE(address) (*(address) & (_DC_FLAG_USED - 1))
#define _DC_BLOCK_IS_LAST(address) (*(address) == 0)

// #define _DC_DEBUG_HEAP

#ifdef _DC_DEBUG_HEAP
    #define _DC_DEBUG_HEAP_LOG(message) dc_console_logPart(message); dc_console_logNewline()
#else
    #define _DC_DEBUG_HEAP_LOG(message)
#endif

namespace dc::heap {
    const Block* base = (Block*)dc_getGlobalI32("__heap_base");
    Block* firstFreeBlock = (Block*)base;
}

extern "C" {
    void* memset(void* destination, char value, size_t size) {
        for (size_t i = 0; i < size; i++) {
            *((char*)destination + size) = value;
        }


        return destination;
    }

    void* memcpy(void* destination, void* source, size_t size) {
        for (size_t i = 0; i < size; i++) {
            *((char*)destination + i) = *((char*)source + i);
        }

        return destination;
    }

    void* malloc(size_t size) {
        _DC_DEBUG_HEAP_LOG("> malloc");

        if (size == 0) {
            size = _DC_ALIGN_SIZE;
        }

        if (size & _DC_FLAG_USED) {
            return nullptr;
        }

        size = _DC_ALIGN(size);

        dc::heap::Block* currentBlock = dc::heap::firstFreeBlock;

        while (true) {
            if (_DC_BLOCK_IS_LAST(currentBlock)) {
                _DC_DEBUG_HEAP_LOG("Use last block");

                *currentBlock = _DC_FLAG_USED | size;
                *(currentBlock + sizeof(dc::heap::Block) + size) = 0; // Create a new last block after this one

                if (currentBlock == dc::heap::firstFreeBlock) {
                    _DC_DEBUG_HEAP_LOG("  Set next free block");

                    dc::heap::firstFreeBlock = currentBlock + sizeof(dc::heap::Block) + size;
                }

                return currentBlock + sizeof(dc::heap::Block);
            }

            size_t originalBlockSize = _DC_BLOCK_SIZE(currentBlock);

            if (_DC_BLOCK_IS_USED(currentBlock)) {
                // Skip over this block if it's used

                _DC_DEBUG_HEAP_LOG("Skip used");

                currentBlock += sizeof(dc::heap::Block) + originalBlockSize;

                continue;
            }

            if (originalBlockSize < size) {
                // Try and merge subsequent blocks to meet required size; if not, then skip

                dc::heap::Block* originalBlock = currentBlock;
                size_t totalUsableSize = originalBlockSize;
                bool encounteredUsedBlock = false;
                bool encounteredLastBlock = false;

                _DC_DEBUG_HEAP_LOG("Attempt resize");

                do {
                    currentBlock += sizeof(dc::heap::Block) + _DC_BLOCK_SIZE(currentBlock);

                    if (_DC_BLOCK_IS_USED(currentBlock)) {
                        _DC_DEBUG_HEAP_LOG("  Encountered used block");

                        encounteredUsedBlock = true;

                        break;
                    }

                    if (_DC_BLOCK_IS_LAST(currentBlock)) {
                        // Looks like we can use the rest of the available memory

                        _DC_DEBUG_HEAP_LOG("  Encountered last block");

                        encounteredLastBlock = true;

                        break;
                    }

                    totalUsableSize += *currentBlock + sizeof(dc::heap::Block);
                } while (totalUsableSize < size);

                if (encounteredUsedBlock) {
                    continue;
                }

                currentBlock = originalBlock;

                if (encounteredLastBlock) {
                    _DC_DEBUG_HEAP_LOG("  Using last block");

                    *(currentBlock + sizeof(dc::heap::Block) + size) = 0; // Create a new last block after this one
                } else {
                    *currentBlock = totalUsableSize;
                }
            }

            if (originalBlockSize > size + sizeof(dc::heap::Block) + _DC_ALIGN_SIZE) {
                // Split this block so that the rest can be used for other purposes

                _DC_DEBUG_HEAP_LOG("Split block");

                *currentBlock = size;
                *(currentBlock + sizeof(dc::heap::Block) + size) = originalBlockSize - size - sizeof(dc::heap::Block);
            }

            _DC_DEBUG_HEAP_LOG("Mark as used");

            *currentBlock |= _DC_FLAG_USED;

            if (currentBlock == dc::heap::firstFreeBlock) {
                _DC_DEBUG_HEAP_LOG("Search for a new first free block");

                while (_DC_BLOCK_IS_USED(dc::heap::firstFreeBlock)) {
                    _DC_DEBUG_HEAP_LOG("  Search next");

                    dc::heap::firstFreeBlock += _DC_BLOCK_SIZE(dc::heap::firstFreeBlock) + sizeof(dc::heap::Block);
                }
            }

            return currentBlock + sizeof(dc::heap::Block);
        }
    }

    void free(void* ptr) {
        if (!ptr) {
            return;
        }

        dc::heap::Block* block = (dc::heap::Block*)ptr - sizeof(dc::heap::Block);

        _DC_DEBUG_HEAP_LOG("> free");

        *block &= ~_DC_FLAG_USED;
 
        if (block < dc::heap::firstFreeBlock) {
            _DC_DEBUG_HEAP_LOG("Set first free block");

            dc::heap::firstFreeBlock = block;
        }

        dc::heap::Block* currentBlock = block + sizeof(dc::heap::Block) + *block;

        // Merge together any subsequent free blocks
        while (!_DC_BLOCK_IS_USED(currentBlock)) {
            if (_DC_BLOCK_IS_LAST(currentBlock)) {
                _DC_DEBUG_HEAP_LOG("Convert to last block");

                *block = 0; // Make the penultimate block the last one instead

                return;
            }

            _DC_DEBUG_HEAP_LOG("Merge subsequent block");

            *block += _DC_BLOCK_SIZE(currentBlock) + sizeof(dc::heap::Block);
            currentBlock += _DC_BLOCK_SIZE(currentBlock) + sizeof(dc::heap::Block);
        }
    }

    void* calloc(size_t count, size_t size) {
        void* memory = malloc(count * size);

        if (memory) {
            memset(memory, '\0', count * size);
        }

        return memory;
    }

    void* realloc(void* ptr, size_t size) {
        _DC_DEBUG_HEAP_LOG("> realloc");

        if (size & _DC_FLAG_USED) {
            return nullptr;
        }

        if (!ptr) {
            return malloc(size);
        }

        if (size == 0) {
            size = _DC_ALIGN_SIZE;
        }

        size = _DC_ALIGN(size);

        dc::heap::Block* blockPtr = (dc::heap::Block*)ptr;
        dc::heap::Block* block = blockPtr - sizeof(dc::heap::Block);

        if (size == _DC_BLOCK_SIZE(block)) {
            _DC_DEBUG_HEAP_LOG("Equal size");

            return ptr;
        }

        if (_DC_BLOCK_IS_LAST(blockPtr + _DC_BLOCK_SIZE(block))) {
            // Modify last block to match new size

            _DC_DEBUG_HEAP_LOG("Modify last block");

            if (blockPtr + _DC_BLOCK_SIZE(block) == dc::heap::firstFreeBlock) {
                // Push pointer to first free block further down if it currently lies in the extended space

                _DC_DEBUG_HEAP_LOG("  Increase first free block");

                dc::heap::firstFreeBlock = blockPtr + size;
            }

            *block = _DC_FLAG_USED | size;
            *(blockPtr + size) = 0; // Set new position for last block

            return ptr;
        }

        if (size + sizeof(dc::heap::Block) < _DC_BLOCK_SIZE(block)) {
            // Truncate current block by splitting truncated portion off into new block

            _DC_DEBUG_HEAP_LOG("Truncate block");

            *(blockPtr + size) = _DC_BLOCK_SIZE(block) - size - sizeof(dc::heap::Block);
            *block = _DC_FLAG_USED | size;

            return ptr;
        }

        // Perform full reallocation

        _DC_DEBUG_HEAP_LOG("Fully reallocate");

        void* newPtr = malloc(size);

        if (!newPtr) {
            return nullptr;
        }

        memcpy(newPtr, ptr, _DC_BLOCK_SIZE(block));
        free(ptr);

        return newPtr;
    }
}

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete(void* ptr, size_t size) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr, size_t size) noexcept {
    free(ptr);
}

extern "C" int __cxa_atexit(void (*function)(void*), void* argument, void* handle) {
    return 0;
}

#pragma clang diagnostic pop