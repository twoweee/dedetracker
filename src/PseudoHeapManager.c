#pragma once
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "PseudoHeapPrint.c"

struct PseudoHeapInstance initializePseudoHeap(void* heapStartingAddress, BYTE_COUNT_T lengthInBlocks, BLOCK_SIZE_T blockSize){
    struct PseudoHeapInstance newHeap = {.length = lengthInBlocks*blockSize, .blockSize = blockSize};
    newHeap.memSpace = heapStartingAddress;
    newHeap.freeBlocks = lengthInBlocks;
    newHeap.lastUsedBlock = -1;

    memset(heapStartingAddress, 0x00, lengthInBlocks*blockSize);
    return newHeap; 
}

inline int justAllocate(struct PseudoHeapInstance* heapInstance, BLOCK_SIZE_T* confirmedBlocks, BYTE_COUNT_T* currentOffset, 
    BLOCK_SIZE_T neededBlocks, BLOCK_SIZE_T neededBytes, 
    BYTE_COUNT_T* newAllocatedSpace){
    const BYTE_COUNT_T currentBlock = *currentOffset / heapInstance->blockSize;
    const BYTE_COUNT_T startingBlock = currentBlock - (*confirmedBlocks - 1);
    const BYTE_COUNT_T startingByte = startingBlock * heapInstance->blockSize;
    heapInstance->lastUsedBlock = currentBlock;

    // printf("currentOffset %d currentBlock %d startingBlock %d startingByte %d currentBlock %d\n\n", *currentOffset, currentBlock, startingBlock, startingByte, currentBlock);
    ((uint8_t*)heapInstance->memSpace)[startingByte] = neededBytes;

    *newAllocatedSpace = startingByte;
    heapInstance->freeBlocks-=neededBlocks;
    return 0;
};

// check if block starting with currentOffset byte is free
// if yes - add to confirmedFreeBlocks, if confirmedFreeBlocks==neededLengthBlocks allocate
// if no - set confirmedFreeBlocks to 0 and keep looking
inline int checkAndAllocate(struct PseudoHeapInstance* heapInstance, BLOCK_SIZE_T* confirmedBlocks, BYTE_COUNT_T* currentOffset, 
    BLOCK_SIZE_T neededBlocks, BLOCK_SIZE_T neededBytes, 
    BYTE_COUNT_T* newAllocatedSpace) {
    if (((uint8_t*)(heapInstance->memSpace))[*currentOffset] != 0x00) {
        *confirmedBlocks = 0;
        while (((uint8_t*)(heapInstance->memSpace))[*currentOffset] != 0x00){ // skip allocated blocks
            const BYTE_COUNT_T dataBytesHere = ((uint8_t*)(heapInstance->memSpace))[*currentOffset];
            const BYTE_COUNT_T jumpBlocks = ((dataBytesHere - 1) / heapInstance->blockSize) + 1;
            // printf("dataBytesHere %d jumpBlocks %d \n", dataBytesHere, jumpBlocks);

            *currentOffset += (jumpBlocks * heapInstance->blockSize);
            if (*currentOffset >= heapInstance->length) return 1; // reached last block
        }
    };
    
    (*confirmedBlocks)++;
    // printf("currentOffset %d confirmedBlocks %d neededBlocks %d \n", *currentOffset, *confirmedBlocks, neededBlocks);
    if (*confirmedBlocks >= neededBlocks) {
        return justAllocate(heapInstance, confirmedBlocks, currentOffset, neededBlocks, neededBytes, newAllocatedSpace);
    }

    return 1; // continue
}

int reservePseudoHeap(struct PseudoHeapInstance* heapInstance, BYTE_COUNT_T neededBytesData, BYTE_COUNT_T* newAllocatedSpace) {
    BLOCK_SIZE_T confirmedBlocks = 0;
    const BLOCK_SIZE_T neededBytes = neededBytesData + 1; // +1 cuz byte for length byte
    const BLOCK_SIZE_T neededBlocks = ((neededBytes - 1) / heapInstance->blockSize) + 1; // +1 cuz division rounds down otherwise
    const BYTE_COUNT_T firstByteToLookAt = (heapInstance->lastUsedBlock + 1) * heapInstance->blockSize; // byte right outside the last used block
    // printf("neededBytes %d neededBlocks %d firstByteToLookAt %d\n", neededBytes, neededBlocks, firstByteToLookAt);
    if (neededBlocks > heapInstance->freeBlocks) return 1;

    // counting bytes but checking blocks
    // last used->end
    for (BYTE_COUNT_T currentOffset = firstByteToLookAt; currentOffset < heapInstance->length; currentOffset += heapInstance->blockSize){
        if (!checkAndAllocate(heapInstance, &confirmedBlocks, &currentOffset, neededBlocks, neededBytes,  newAllocatedSpace)){
            return 0;
        };
    }

    // try start->last used
    // to a) check before last allocated byte and b) in case since setting last allocated byte, memory as been freed
    confirmedBlocks = 0;
    for (BYTE_COUNT_T currentOffset = 0; currentOffset < firstByteToLookAt; currentOffset++){
        if (!checkAndAllocate(heapInstance, &confirmedBlocks, &currentOffset, neededBlocks, neededBytes,  newAllocatedSpace)){
            return 0;
        };
    }
    return 2; // total space is sufficient, but fragmenting makes it not fit
}

// free some space
int freePseudoHeap(struct PseudoHeapInstance* heapInstance, BYTE_COUNT_T* allocatedSpace) {
    const BLOCK_SIZE_T actualLength = ((uint8_t*)heapInstance->memSpace)[*allocatedSpace];
    // printf("len %d\n", actualLength);
    const BLOCK_SIZE_T lengthRoundedBlocks = (((actualLength - 1) / heapInstance->blockSize) + 1);
    // printf("lenbl %d\n", lengthRoundedBlocks);
    const BLOCK_SIZE_T lengthRoundedBytes = lengthRoundedBlocks * heapInstance->blockSize;
    // printf("lenby %d\n", lengthRoundedBytes);
    memset(&(((uint8_t*)heapInstance->memSpace)[*allocatedSpace]), 0x00, lengthRoundedBytes);
    heapInstance->freeBlocks+=lengthRoundedBlocks;
    return 0;
}