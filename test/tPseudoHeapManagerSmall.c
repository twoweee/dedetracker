#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define HEAP_LEN_BLOCKS 8
#define BLOCK_SIZE 1

#define SUCCESS_STATUS 0
#define FAIL_STATUS 1
#define FRAGMENTED_STATUS 2

#define DEFAULT_LAST_USED_BYTE UINT32_MAX

// yes we are using local vars... but its to make the whole thing much easier to use
BYTE_COUNT_T allocatedAddress_A;
const int sizeToAllocate_A = 3;
uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

BYTE_COUNT_T allocatedAddress_B;
const int sizeToAllocate_B = 1;
uint8_t dataToStore_B[1] = {0x11};

BYTE_COUNT_T allocatedAddress_C;
const int sizeToAllocate_C = 1;
uint8_t dataToStore_C[1] = {0x21};

int assertAllocatedMemory(struct PseudoHeapInstance* heapInstancevoid, BYTE_COUNT_T* allocatedSpace, void* sourceMemoryAddress){
    const BYTE_COUNT_T dataLength = ((uint8_t*)heapInstancevoid->memSpace)[*allocatedSpace] - 1;
    const BYTE_COUNT_T targetDataAddress = *allocatedSpace + 1;
    void* heapAddress = &(((uint8_t*)heapInstancevoid->memSpace)[targetDataAddress]);
    TEST_ASSERT_EQUAL_MEMORY(sourceMemoryAddress, heapAddress, dataLength);
    return 0;
};

void test_createPseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];
    uint8_t targetHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE] = {0x00};

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    // EXPECTED RESULT: empty heap with default values
    TEST_ASSERT_EQUAL_MEMORY(targetHeapMemory, heapInstance.memSpace, HEAP_LEN_BLOCKS);
    TEST_ASSERT_EQUAL_INT(HEAP_LEN_BLOCKS * BLOCK_SIZE, heapInstance.length);
    TEST_ASSERT_EQUAL_INT(BLOCK_SIZE, heapInstance.blockSize);
    TEST_ASSERT_EQUAL_INT(HEAP_LEN_BLOCKS, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, heapInstance.lastUsedBlock);
};

int helperPrefillSmall(uint8_t* testHeapMemory, struct PseudoHeapInstance* heapInstance){
    int status;

    // reserve and write A
    allocatedAddress_A = -1;
    // const int sizeToAllocate_A = 3;
    // uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

    status = reservePseudoHeap(heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
    if (!status) writeToPseudoHeapAddress(heapInstance, &allocatedAddress_A, dataToStore_A);

    assertAllocatedMemory(heapInstance, &allocatedAddress_A, dataToStore_A);
    TEST_ASSERT_EQUAL_INT(4, heapInstance->freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write B
    allocatedAddress_B = -1;
    // const int sizeToAllocate_B = 1;
    // uint8_t dataToStore_B[1] = {0x11};

    status = reservePseudoHeap(heapInstance, sizeToAllocate_B, &allocatedAddress_B);

    if (!status) writeToPseudoHeapAddress(heapInstance, &allocatedAddress_B, dataToStore_B);

    assertAllocatedMemory(heapInstance, &allocatedAddress_A, dataToStore_A);
    assertAllocatedMemory(heapInstance, &allocatedAddress_B, dataToStore_B);
    TEST_ASSERT_EQUAL_INT(2, heapInstance->freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write C

    allocatedAddress_C = -1;
    // const int sizeToAllocate_C = 1;
    // uint8_t dataToStore_C[1] = {0x21};

    status = reservePseudoHeap(heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
    if (!status) writeToPseudoHeapAddress(heapInstance, &allocatedAddress_C, dataToStore_C);

    assertAllocatedMemory(heapInstance, &allocatedAddress_A, dataToStore_A);
    assertAllocatedMemory(heapInstance, &allocatedAddress_B, dataToStore_B);
    assertAllocatedMemory(heapInstance, &allocatedAddress_C, dataToStore_C);
    TEST_ASSERT_EQUAL_INT(0, heapInstance->freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    return status;
};

void test_fillPseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    int status = helperPrefillSmall(testHeapMemory, &heapInstance);
};

void test_overfillFullPseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    int status = helperPrefillSmall(testHeapMemory, &heapInstance);

    // reserve and write D (FAIL)

    BYTE_COUNT_T allocatedAddress_D;
    const int sizeToAllocate_D = 1;
    uint8_t dataToStore_D[1] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddress_D, dataToStore_D);

    TEST_ASSERT_EQUAL_INT(0, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
};

void test_overfillAlmostFullPseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    int status = helperPrefillSmall(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (FAIL)

    BYTE_COUNT_T allocatedAddress_D;
    const int sizeToAllocate_D = 2;
    uint8_t dataToStore_D[2] = {0x31, 0x32};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddress_D, dataToStore_D);

    TEST_ASSERT_EQUAL_INT(2, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
};

void test_topoffPseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    int status = helperPrefillSmall(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (SUCCESS)

    BYTE_COUNT_T allocatedAddress_D;
    const int sizeToAllocate_D = 1;
    uint8_t dataToStore_D[1] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddress_D, dataToStore_D);

    assertAllocatedMemory(&heapInstance, &allocatedAddress_A, dataToStore_A);
    assertAllocatedMemory(&heapInstance, &allocatedAddress_B, dataToStore_B);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddress_C);
    assertAllocatedMemory(&heapInstance, &allocatedAddress_D, dataToStore_D);
    TEST_ASSERT_EQUAL_INT(0, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_replaceMiddlePseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    int status = helperPrefillSmall(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_A);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (SUCCESS)

    BYTE_COUNT_T allocatedAddress_D;
    const int sizeToAllocate_D = 2;
    uint8_t dataToStore_D[2] = {0x31, 0x32};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddress_D, dataToStore_D);

    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddress_A);
    assertAllocatedMemory(&heapInstance, &allocatedAddress_B, dataToStore_B);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddress_C);
    assertAllocatedMemory(&heapInstance, &allocatedAddress_D, dataToStore_D);
    TEST_ASSERT_EQUAL_INT(3, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_replaceFailDueToFragPseudoHeapSmall(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE);

    int status = helperPrefillSmall(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_A);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (SUCCESS)

    BYTE_COUNT_T allocatedAddress_D;
    const int sizeToAllocate_D = 2;
    uint8_t dataToStore_D[2] = {0x31, 0x32};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddress_D, dataToStore_D);

    // reserve and write E (FAIL)

    BYTE_COUNT_T allocatedAddress_E = (uint32_t) DEFAULT_LAST_USED_BYTE;
    const int sizeToAllocate_E = 2;
    uint8_t dataToStore_E[2] = {0x41, 0x42};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_E, &allocatedAddress_E);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddress_E, dataToStore_E);

    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddress_A);
    assertAllocatedMemory(&heapInstance, &allocatedAddress_B, dataToStore_B);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddress_C);
    assertAllocatedMemory(&heapInstance, &allocatedAddress_D, dataToStore_D);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddress_E);
    TEST_ASSERT_EQUAL_INT(3, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(FRAGMENTED_STATUS, status);
};

void run_all_tests_PseudoHeapManagerShort(void){
    RUN_TEST(test_createPseudoHeapSmall);
    RUN_TEST(test_fillPseudoHeapSmall); 
    RUN_TEST(test_overfillFullPseudoHeapSmall); 
    RUN_TEST(test_overfillAlmostFullPseudoHeapSmall); 
    RUN_TEST(test_topoffPseudoHeapSmall); 
    RUN_TEST(test_replaceMiddlePseudoHeapSmall); 
    RUN_TEST(test_replaceFailDueToFragPseudoHeapSmall); 
};