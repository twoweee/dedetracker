#include <stdio.h>
#include <stdint.h>
#include "PseudoHeapManager.h"

static inline int helperPrintMemLine(int lineWidth, unsigned char *startAddr, int row, uint16_t *pbytes, int lastRowLen){
    printf("%04X %04X", row*lineWidth, pbytes[0]+row*lineWidth);
    for (int col = 0; col < lastRowLen; col++){
        printf("   %02X", startAddr[col+(row*lineWidth)]);
    }
    return 0;
}

int printMemSpace(void* memSpace, int length, int lineWidth){
    unsigned char *startAddr = (unsigned char *)memSpace;
    uint16_t *pbytes = (uint16_t *)&startAddr;
    printf("\n\t ");
    for (int i = 0; i<lineWidth; i++) printf(" %04X", i);
    printf("\n\t ");
    for (int i = 0; i<lineWidth; i++) printf(" %04X", pbytes[0]+ i);
    printf("\n");
    int row = 0;
    int col = 0;
    for (; row < length/lineWidth; row++){
        helperPrintMemLine(lineWidth, startAddr, row, pbytes, lineWidth);
        printf("\n");
    }
    int lastRowLen = length % lineWidth;
    if (lastRowLen > 0){
        helperPrintMemLine(lineWidth, startAddr, row, pbytes, lastRowLen);
    }
    printf("\n");
    return 0;
}

int printPseudoHeapInstance(struct PseudoHeapInstance* heapInstance, int lineWidth) {
    printf("ADDR: %p, LEN: %d, free:%d, block:%d", 
        heapInstance->memSpace, heapInstance->length,
        heapInstance->freeBlocks, heapInstance->blockSize);
    int status = printMemSpace(heapInstance->memSpace, heapInstance->length, lineWidth);
    return status;
}