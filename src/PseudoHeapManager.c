#pragma once
#include <string.h>
#include <stdio.h>
#include <stdint.h>

struct PseudoHeapInstance {
    void* memSpace;
    int length;
    int lastUsedByte;
    int lastFailedAllocateSize;
};

// print single mem line 
int inline printMemLine(int lineWidth, unsigned char *startAddr, int row, uint16_t *pbytes, int lastRowLen){
    printf("%04X %04X", row*lineWidth, pbytes[0]+row*lineWidth);
    for (int col = 0; col < lastRowLen; col++){
        printf("   %02X", startAddr[col+(row*lineWidth)]);
    }
    return 0;
}

// print memory bytes for debugging
int printMemSpace(void* memSpace, int length, int lineWidth){
    unsigned char *startAddr = (unsigned char *)memSpace;
    uint16_t *pbytes = (uint16_t *)&startAddr;
    //printf("\t  0000 0001 0002 0003 0004 0005 0006 0007 0008 0009 000A 000B 000C 000D 000E 000F\n");
    printf("\n\t ");
    for (int i = 0; i<lineWidth; i++) printf(" %04X", i);
    printf("\n");
    //printf("\t  %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X\n", 
    //   pbytes[0], pbytes[0]+1, pbytes[0]+2, pbytes[0]+3, pbytes[0]+4, pbytes[0]+5, pbytes[0]+6, pbytes[0]+7,
     //   pbytes[0]+8, pbytes[0]+9, pbytes[0]+10, pbytes[0]+11, pbytes[0]+12, pbytes[0]+13, pbytes[0]+14, pbytes[0]+15);
    printf("\t ");
    for (int i = 0; i<lineWidth; i++) printf(" %04X", pbytes[0]+ i);
    printf("\n");
    int row = 0;
    int col = 0;
    for (; row < length/lineWidth; row++){
        printMemLine(lineWidth, startAddr, row, pbytes, lineWidth);
        printf("\n");
    }
    int lastRowLen = length % lineWidth;
    if (lastRowLen > 0){
        printMemLine(lineWidth, startAddr, row, pbytes, lastRowLen);
    }
    printf("\n");
    return 0;
}

// print PseudoHeap instance
int printPseudoHeapInstance(struct PseudoHeapInstance* heapInstance, int lineWidth) {
    printf("ADDR: %p, LEN: %d, last_used_addr: %d, full: %d\n", 
        heapInstance->memSpace, heapInstance->length, heapInstance->lastUsedByte, heapInstance->lastFailedAllocateSize);
    int status = printMemSpace(heapInstance->memSpace, heapInstance->length, lineWidth);
    return status;
}

struct PseudoHeapInstance initializePseudoHeap(void* startingAddr, int lengthInBytes){
    struct PseudoHeapInstance newHeap;
    newHeap.lastUsedByte = -1;
    newHeap.lastFailedAllocateSize = UINT16_MAX;
    newHeap.memSpace = startingAddr;
    newHeap.length = lengthInBytes;

    memset(startingAddr, '\0', lengthInBytes);
    return newHeap;
}

// just allocate the memory
inline int justAllocate(struct PseudoHeapInstance* heapInstance, int* confirmedFreeBytes, int* currentOffset, int* neededLengthInBytes, 
    void** newAllocatedSpace){
    heapInstance->lastUsedByte = *currentOffset;
    //memset(((uint8_t*)heapInstance->memSpace) + (*currentOffset) - (*neededLengthInBytes-1), '\x01', *neededLengthInBytes);
    memset(((uint8_t*)heapInstance->memSpace) + (*currentOffset) - (*neededLengthInBytes-1), *currentOffset, *neededLengthInBytes);
    *newAllocatedSpace = (void*)((uint8_t*)(heapInstance->memSpace))[*currentOffset];
    return 0;
};

inline int isMemoryTaken(struct PseudoHeapInstance* heapInstance, int* confirmedFreeBytes, int* currentOffset, int* neededLengthInBytes, 
    void** newAllocatedSpace) {
    if (((uint8_t*)(heapInstance->memSpace))[*currentOffset] == '\0'){
        (*confirmedFreeBytes)++;
        if (*confirmedFreeBytes >= *neededLengthInBytes) {
            return justAllocate(heapInstance, confirmedFreeBytes, currentOffset, neededLengthInBytes, newAllocatedSpace);
        }
    } else {
        confirmedFreeBytes = 0;
    }
    return 1;
}

// reserve space in the heap, newAllocatedSpace holds the stored to address
int reservePseudoHeap(struct PseudoHeapInstance* heapInstance, int neededLengthInBytes, void** newAllocatedSpace) {
    int confirmedFreeBytes = 0;
    if (heapInstance->lastFailedAllocateSize<=neededLengthInBytes || neededLengthInBytes>heapInstance->length) return 1;
    // try now->end
    for (int currentOffset = heapInstance->lastUsedByte + 1; currentOffset < heapInstance->length; currentOffset++){
        if (!isMemoryTaken(heapInstance, &confirmedFreeBytes, &currentOffset, &neededLengthInBytes, newAllocatedSpace)){
            return 0;
        };
    }
    // try start->end
    // to a) check before last allocated byte and b) in case since setting last allocated byte, memory as been freed
    confirmedFreeBytes = 0;
    for (int currentOffset = 0; currentOffset < heapInstance->length; currentOffset++){
        if (!isMemoryTaken(heapInstance, &confirmedFreeBytes, &currentOffset, &neededLengthInBytes, newAllocatedSpace)){
            return 0;
        };
    }
    heapInstance->lastFailedAllocateSize = neededLengthInBytes;
    return 1;
}

// free some space
int freePseudoHeap(struct PseudoHeapInstance* heapInstance, int startOffset, int lengthInBytes) {
    heapInstance->lastFailedAllocateSize = INT16_MAX;
    memset(((uint8_t*)heapInstance->memSpace) + startOffset, '\0', lengthInBytes);
    return 0;
}