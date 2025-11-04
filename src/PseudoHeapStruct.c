#pragma once
#include <stdint.h>

struct PseudoHeapInstance {
    void* memSpace;
    uint32_t freeBlocks; 
    uint32_t lastUsedBlock;
    const uint32_t length;
    const uint8_t blockSize;
};