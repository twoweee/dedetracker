#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define HEAP_LEN_BLOCKS 8
#define BLOCK_SIZE_LONG 4

#define SUCCESS_STATUS 0
#define FAIL_STATUS 1
#define FRAGMENTED_STATUS 2

#define DEFAULT_LAST_USED_BYTE UINT32_MAX

// THIS TEST WORKS BUT SUCKS, DONT BOTHER READING

// yes we are using local vars... but its to make the whole thing much easier to use
BYTE_COUNT_T allocatedAddressLong_A;
const int sizeToAllocateLong_A = 19;
uint8_t dataToStoreLong_A[19] = {0x01};

BYTE_COUNT_T allocatedAddressLong_B;
const int sizeToAllocateLong_B = 7;
uint8_t dataToStoreLong_B[7] = {0x11};

BYTE_COUNT_T allocatedAddressLong_C;
const int sizeToAllocateLong_C = 1;
uint8_t dataToStoreLong_C[1] = {0x21};

int assertAllocatedMemoryLong(struct PseudoHeapInstance* heapInstancevoid, BYTE_COUNT_T* allocatedSpace, void* sourceMemoryAddress){
    const BYTE_COUNT_T dataLength = ((uint8_t*)heapInstancevoid->memSpace)[*allocatedSpace] - 1;
    const BYTE_COUNT_T targetDataAddress = *allocatedSpace + 1;
    void* heapAddress = &(((uint8_t*)heapInstancevoid->memSpace)[targetDataAddress]);
    TEST_ASSERT_EQUAL_MEMORY(sourceMemoryAddress, heapAddress, dataLength);
    return 0;
};

void test_createPseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];
    uint8_t targetHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG] = {0x00};

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    // EXPECTED RESULT: empty heap with default values
    TEST_ASSERT_EQUAL_MEMORY(targetHeapMemory, heapInstance.memSpace, HEAP_LEN_BLOCKS);
    TEST_ASSERT_EQUAL_INT(HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG, heapInstance.length);
    TEST_ASSERT_EQUAL_INT(BLOCK_SIZE_LONG, heapInstance.blockSize);
    TEST_ASSERT_EQUAL_INT(HEAP_LEN_BLOCKS, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, heapInstance.lastUsedBlock);
};

int helperPrefillLong(uint8_t* testHeapMemory, struct PseudoHeapInstance* heapInstance){
    int status;

    // reserve and write A
    allocatedAddressLong_A = -1;
    // const int sizeToAllocateLong_A = 3;
    // uint8_t dataToStoreLong_A[3] = {0x01, 0x02, 0x03};

    status = reservePseudoHeap(heapInstance, sizeToAllocateLong_A, &allocatedAddressLong_A);
    
    if (!status) writeToPseudoHeapAddress(heapInstance, &allocatedAddressLong_A, dataToStoreLong_A);

    assertAllocatedMemoryLong(heapInstance, &allocatedAddressLong_A, dataToStoreLong_A);
    TEST_ASSERT_EQUAL_INT(3, heapInstance->freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write B
    allocatedAddressLong_B = -1;
    // const int sizeToAllocateLong_B = 1;
    // uint8_t dataToStoreLong_B[1] = {0x11};

    status = reservePseudoHeap(heapInstance, sizeToAllocateLong_B, &allocatedAddressLong_B);

    if (!status) writeToPseudoHeapAddress(heapInstance, &allocatedAddressLong_B, dataToStoreLong_B);

    assertAllocatedMemoryLong(heapInstance, &allocatedAddressLong_A, dataToStoreLong_A);
    assertAllocatedMemoryLong(heapInstance, &allocatedAddressLong_B, dataToStoreLong_B);
    TEST_ASSERT_EQUAL_INT(1, heapInstance->freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write C

    allocatedAddressLong_C = -1;
    // const int sizeToAllocateLong_C = 1;
    // uint8_t dataToStoreLong_C[1] = {0x21};

    status = reservePseudoHeap(heapInstance, sizeToAllocateLong_C, &allocatedAddressLong_C);
    
    if (!status) writeToPseudoHeapAddress(heapInstance, &allocatedAddressLong_C, dataToStoreLong_C);

    assertAllocatedMemoryLong(heapInstance, &allocatedAddressLong_A, dataToStoreLong_A);
    assertAllocatedMemoryLong(heapInstance, &allocatedAddressLong_B, dataToStoreLong_B);
    assertAllocatedMemoryLong(heapInstance, &allocatedAddressLong_C, dataToStoreLong_C);
    TEST_ASSERT_EQUAL_INT(0, heapInstance->freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    return status;
};

void test_fillPseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    int status = helperPrefillLong(testHeapMemory, &heapInstance);
};

void test_overfillFullPseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    int status = helperPrefillLong(testHeapMemory, &heapInstance);

    // reserve and write D (FAIL)

    BYTE_COUNT_T allocatedAddressLong_D;
    const int sizeToAllocateLong_D = 1;
    uint8_t dataToStoreLong_D[1] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocateLong_D, &allocatedAddressLong_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);

    TEST_ASSERT_EQUAL_INT(0, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
};

void test_overfillAlmostFullPseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    int status = helperPrefillLong(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddressLong_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (FAIL)

    BYTE_COUNT_T allocatedAddressLong_D;
    const int sizeToAllocateLong_D = 4;
    uint8_t dataToStoreLong_D[4] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocateLong_D, &allocatedAddressLong_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);

    TEST_ASSERT_EQUAL_INT(1, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
};

void test_topoffPseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    int status = helperPrefillLong(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddressLong_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (SUCCESS)

    BYTE_COUNT_T allocatedAddressLong_D;
    const int sizeToAllocateLong_D = 1;
    uint8_t dataToStoreLong_D[1] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocateLong_D, &allocatedAddressLong_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);

    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_A, dataToStoreLong_A);
    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_B, dataToStoreLong_B);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddressLong_C);
    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);
    TEST_ASSERT_EQUAL_INT(0, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_replaceMiddlePseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    int status = helperPrefillLong(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddressLong_A);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (SUCCESS)

    BYTE_COUNT_T allocatedAddressLong_D = (uint32_t) DEFAULT_LAST_USED_BYTE;
    const int sizeToAllocateLong_D = 11;
    uint8_t dataToStoreLong_D[11] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocateLong_D, &allocatedAddressLong_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);

    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddressLong_A);
    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_B, dataToStoreLong_B);
    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_C, dataToStoreLong_C);
    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);
    TEST_ASSERT_EQUAL_INT(2, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_replaceFailDueToFragPseudoHeapLong(void){
    uint8_t testHeapMemory[HEAP_LEN_BLOCKS * BLOCK_SIZE_LONG];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, HEAP_LEN_BLOCKS, BLOCK_SIZE_LONG);

    int status = helperPrefillLong(testHeapMemory, &heapInstance);

    status = freePseudoHeap(&heapInstance, &allocatedAddressLong_A);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    status = freePseudoHeap(&heapInstance, &allocatedAddressLong_C);

    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

    // reserve and write D (FAIL)

    BYTE_COUNT_T allocatedAddressLong_D = (uint32_t) DEFAULT_LAST_USED_BYTE;
    const int sizeToAllocateLong_D = 21;
    uint8_t dataToStoreLong_D[21] = {0x31};

    status = reservePseudoHeap(&heapInstance, sizeToAllocateLong_D, &allocatedAddressLong_D);
    
    if (!status) writeToPseudoHeapAddress(&heapInstance, &allocatedAddressLong_D, dataToStoreLong_D);

    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddressLong_A);
    assertAllocatedMemoryLong(&heapInstance, &allocatedAddressLong_B, dataToStoreLong_B);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddressLong_C);
    TEST_ASSERT_EQUAL_INT((uint32_t) DEFAULT_LAST_USED_BYTE, allocatedAddressLong_D);
    TEST_ASSERT_EQUAL_INT(6, heapInstance.freeBlocks);
    TEST_ASSERT_EQUAL_INT(FRAGMENTED_STATUS, status);
};

void run_all_tests_PseudoHeapManagerLong(void){
    RUN_TEST(test_createPseudoHeapLong);
    RUN_TEST(test_fillPseudoHeapLong); 
    RUN_TEST(test_overfillFullPseudoHeapLong); 
    RUN_TEST(test_overfillAlmostFullPseudoHeapLong); 
    RUN_TEST(test_topoffPseudoHeapLong); 
    RUN_TEST(test_replaceMiddlePseudoHeapLong); 
    RUN_TEST(test_replaceFailDueToFragPseudoHeapLong);
};