#pragma once
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "PseudoHeapPrint.c"

struct PseudoHeapInstance initializePseudoHeap(void* heapStartingAddress, uint32_t lengthInBlocks, uint8_t blockSize){
    struct PseudoHeapInstance newHeap = {.length = lengthInBlocks, .blockSize = blockSize};
    newHeap.memSpace = heapStartingAddress;
    newHeap.freeBlocks = lengthInBlocks;
    newHeap.lastUsedBlock = -1;

    memset(heapStartingAddress, 0x00, lengthInBlocks*blockSize);
    return newHeap; 
}

int writeToPseudoHeapAddress(void* allocatedMemoryAddress, void* sourceMemoryAddress, int sourceLength){
    memcpy((uint8_t*)allocatedMemoryAddress+1, sourceMemoryAddress, sourceLength);
    return 0;
}

// // just allocate the memory
// inline int justAllocate(struct PseudoHeapInstance* heapInstance, int* confirmedFreeBytes, int* currentOffset, int* neededLengthInBytes, 
//     void** newAllocatedSpace){
//     heapInstance->lastUsedByte = *currentOffset;
//     memset(((uint8_t*)heapInstance->memSpace) + (*currentOffset) - (*neededLengthInBytes-1), 0xFF, *neededLengthInBytes);
//     memset(((uint8_t*)heapInstance->memSpace) + (*currentOffset) - (*neededLengthInBytes-1), *neededLengthInBytes-1, 1);
    
//     // memset(((uint8_t*)heapInstance->memSpace) + (*currentOffset) - (*neededLengthInBytes-1), *currentOffset, *neededLengthInBytes);
//     *newAllocatedSpace = (void*)((&((uint8_t*)(heapInstance->memSpace))[*currentOffset]) - (*neededLengthInBytes-1));
//     heapInstance->freeBlocks-=*neededLengthInBytes;
//     return 0;
// };

// // check if given space is free
// inline int isMemoryTaken(struct PseudoHeapInstance* heapInstance, int* confirmedFreeBytes, int* currentOffset, int* neededLengthInBytes, 
//     void** newAllocatedSpace) {
//     if (((uint8_t*)(heapInstance->memSpace))[*currentOffset] == '\0'){
//         (*confirmedFreeBytes)++;
//         if (*confirmedFreeBytes >= *neededLengthInBytes) {
//             return justAllocate(heapInstance, confirmedFreeBytes, currentOffset, neededLengthInBytes, newAllocatedSpace);
//         }
//     } else {
//         *confirmedFreeBytes = 0;
//     }
//     return 1;
// }

// // reserve space in the heap, newAllocatedSpace holds the stored to address
// int reservePseudoHeap(struct PseudoHeapInstance* heapInstance, int neededLengthInBytesTmp, void** newAllocatedSpace) {
//     int confirmedFreeBytes = 0;
//     int neededLengthInBytes = neededLengthInBytesTmp + 1;
//     if (heapInstance->lastFailedAllocateSize<=neededLengthInBytes || neededLengthInBytes>heapInstance->length) return 1;
//     // try now->end
//     for (int currentOffset = heapInstance->lastUsedByte + 1; currentOffset < heapInstance->length; currentOffset++){
//         if (!isMemoryTaken(heapInstance, &confirmedFreeBytes, &currentOffset, &neededLengthInBytes, newAllocatedSpace)){
//             return 0;
//         };
//     }
//     // try start->end
//     // to a) check before last allocated byte and b) in case since setting last allocated byte, memory as been freed
//     confirmedFreeBytes = 0;
//     for (int currentOffset = 0; currentOffset < heapInstance->length; currentOffset++){
//         if (!isMemoryTaken(heapInstance, &confirmedFreeBytes, &currentOffset, &neededLengthInBytes, newAllocatedSpace)){
//             return 0;
//         };
//     }
//     heapInstance->lastFailedAllocateSize = neededLengthInBytes;
//     if (neededLengthInBytes<=heapInstance->freeBlocks) return 2;
//     else return 1;
// }

// // free some space
// int freePseudoHeap(struct PseudoHeapInstance* heapInstance, void** newAllocatedSpaceint, int lengthInBytes) {
//     heapInstance->lastFailedAllocateSize = INT16_MAX;
//     memset(*newAllocatedSpaceint, '\0', lengthInBytes);
//     *newAllocatedSpaceint = NULL;
//     heapInstance->freeBlocks+=lengthInBytes;
//     return 0;
// }