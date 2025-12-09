#ifndef PSEUDOHEAPMANAGER_H
#define PSEUDOHEAPMANAGER_H
#define BYTE_COUNT_T uint32_t
#define BLOCK_SIZE_T uint8_t

struct PseudoHeapInstance {
    void* memSpace;
    BYTE_COUNT_T freeBlocks; 
    BYTE_COUNT_T lastUsedBlock;
    const BYTE_COUNT_T length;
    const BLOCK_SIZE_T blockSize;
};

struct PseudoHeapInstance initializePseudoHeap(void* heapStartingAddress, BYTE_COUNT_T lengthInBlocks, BLOCK_SIZE_T blockSize);
int writeToPseudoHeapAddress(struct PseudoHeapInstance* heapInstancevoid, BYTE_COUNT_T* allocatedSpace, void* sourceMemoryAddress);
// int justAllocate(struct PseudoHeapInstance* heapInstance, BLOCK_SIZE_T* confirmedBlocks, BYTE_COUNT_T* currentOffset, 
//     BLOCK_SIZE_T neededBlocks, BLOCK_SIZE_T neededBytes, 
//     BYTE_COUNT_T* newAllocatedSpace);
// int checkAndAllocate(struct PseudoHeapInstance* heapInstance, BLOCK_SIZE_T* confirmedBlocks, BYTE_COUNT_T* currentOffset, 
//     BLOCK_SIZE_T neededBlocks, BLOCK_SIZE_T neededBytes, 
//     BYTE_COUNT_T* newAllocatedSpace);
int reservePseudoHeap(struct PseudoHeapInstance* heapInstance, BYTE_COUNT_T neededBytesData, BYTE_COUNT_T* newAllocatedSpace);
int freePseudoHeap(struct PseudoHeapInstance* heapInstance, BYTE_COUNT_T* allocatedSpace);

//int helperPrintMemLine(int lineWidth, unsigned char *startAddr, int row, uint16_t *pbytes, int lastRowLen);
int printMemSpace(void* memSpace, int length, int lineWidth);
int printPseudoHeapInstance(struct PseudoHeapInstance* heapInstance, int lineWidth);
#endif //PSEUDOHEAPMANAGER_H