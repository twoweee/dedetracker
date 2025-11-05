#pragma once
#include <stdint.h>

#define BYTE_COUNT_T uint32_t
#define BLOCK_SIZE_T uint8_t

struct PseudoHeapInstance {
    void* memSpace;
    BYTE_COUNT_T freeBlocks; 
    BYTE_COUNT_T lastUsedBlock;
    const BYTE_COUNT_T length;
    const BLOCK_SIZE_T blockSize;
};