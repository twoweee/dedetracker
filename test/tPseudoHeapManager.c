#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TEST_HEAP_LEN_SMALL 8
#define TEST_HEAP_LEN_LARGE 64
#define BLOCK_SIZE_SMALL 1
#define BLOCK_SIZE_LARGE 8

#define SUCCESS_STATUS 0
#define FAIL_STATUS 1
#define FRAGMENTED_STATUS 2

#define DEFAULT_LAST_USED_BYTE -1

// void assertAllocatedMemory(void* targetHeapMemory, void* memSpace){
//     const uint32_t dataLength = ((uint8_t*)memSpace)[0];
//     const uint8_t* dataAddress = &((uint8_t*)memSpace)[1];

//     TEST_ASSERT_EQUAL_MEMORY(targetHeapMemory, dataAddress, dataLength);
// };

// void test_createPseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];
//     uint8_t targetHeapMemory[TEST_HEAP_LEN_SMALL] = {0x00};

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     // EXPECTED RESULT: empty heap with default values
//     TEST_ASSERT_EQUAL_MEMORY(targetHeapMemory, heapInstance.memSpace, TEST_HEAP_LEN_SMALL);
//     TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN_SMALL, heapInstance.length);
//     TEST_ASSERT_EQUAL_INT(BLOCK_SIZE_SMALL, heapInstance.blockSize);
//     TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN_SMALL, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(DEFAULT_LAST_USED_BYTE, heapInstance.lastUsedBlock);
// };

// void test_createPseudoHeap_large(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_LARGE];
//     uint8_t targetHeapMemory[TEST_HEAP_LEN_LARGE] = {0x00};

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_LARGE, BLOCK_SIZE_LARGE);

//     // EXPECTED RESULT: empty heap with default values
//     TEST_ASSERT_EQUAL_MEMORY(targetHeapMemory, heapInstance.memSpace, TEST_HEAP_LEN_LARGE);
//     TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN_LARGE, heapInstance.length);
//     TEST_ASSERT_EQUAL_INT(BLOCK_SIZE_LARGE, heapInstance.blockSize);
//     TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN_LARGE, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(DEFAULT_LAST_USED_BYTE, heapInstance.lastUsedBlock);
// };

// void test_allocatePseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     void* allocatedAddress = NULL;
//     const int sizeToAllocate = 3;
//     uint8_t dataToStore[3] = {0x01, 0x02, 0x03};
//     int status = reservePseudoHeap(&heapInstance, sizeToAllocate, &allocatedAddress);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress, dataToStore, 3); // write to newly allocated address, it will be memcpy in real use

//     // EXPECTED RESULT: allocated memory of len 3
//     assertAllocatedMemory(dataToStore, allocatedAddress);
//     TEST_ASSERT_EQUAL_INT(4, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(3, heapInstance.lastUsedBlock);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
// };

// void test_fillPseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     int status;

//     // reserve and write A
//     void* allocatedAddress_A = NULL;
//     const int sizeToAllocate_A = 3;
//     uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

//     assertAllocatedMemory(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
//     TEST_ASSERT_EQUAL_INT(5, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(UINT16_MAX, heapInstance.lastFailedAllocateSize);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

//     // reserve and write B
//     void* allocatedAddress_B = NULL;
//     const int sizeToAllocate_B = 1;
//     uint8_t dataToStore_B[1] = {0x11};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

//     assertAllocatedMemory(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     TEST_ASSERT_EQUAL_INT(4, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

//     // reserve and write C

//     void* allocatedAddress_C = NULL;
//     const int sizeToAllocate_C = 2;
//     uint8_t dataToStore_C[2] = {0x21, 0x22};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_C, dataToStore_C, sizeToAllocate_C);

//     assertAllocatedMemory(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     assertAllocatedMemory(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
//     TEST_ASSERT_EQUAL_INT(2, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

//     // reserve and write D

//     void* allocatedAddress_D = NULL;
//     const int sizeToAllocate_D = 2;
//     uint8_t dataToStore_D[2] = {0x31, 0x32};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_D, dataToStore_D, sizeToAllocate_D);

//     assertAllocatedMemory(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     assertAllocatedMemory(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
//     assertAllocatedMemory(dataToStore_D, allocatedAddress_D, sizeToAllocate_D);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
// };

// void test_allocateAndFreePseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     int status;

//     // reserve and write A
//     void* allocatedAddress_A = NULL;
//     const int sizeToAllocate_A = 3;
//     uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

//     // reserve and write B

//     void* allocatedAddress_B = NULL;
//     const int sizeToAllocate_B = 2;
//     uint8_t dataToStore_B[2] = {0x11, 0x12};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

//     status = freePseudoHeap(&heapInstance, &allocatedAddress_A, sizeToAllocate_A);

//     TEST_ASSERT_NULL(allocatedAddress_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     TEST_ASSERT_EQUAL_INT(6, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
// };

// void test_fillPlusPseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     int status;

//     // reserve and write A
//     void* allocatedAddress_A = NULL;
//     const int sizeToAllocate_A = 3;
//     uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

//     // reserve and write B
//     void* allocatedAddress_B = NULL;
//     const int sizeToAllocate_B = 1;
//     uint8_t dataToStore_B[1] = {0x11};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

//     // reserve and write C

//     void* allocatedAddress_C = NULL;
//     const int sizeToAllocate_C = 2;
//     uint8_t dataToStore_C[2] = {0x21, 0x22};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_C, dataToStore_C, sizeToAllocate_C);

//     // reserve and write D

//     void* allocatedAddress_D = NULL;
//     const int sizeToAllocate_D = 2;
//     uint8_t dataToStore_D[2] = {0x31, 0x32};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);

//     if (!status) writeToPseudoHeapAddress(allocatedAddress_D, dataToStore_D, sizeToAllocate_D);

//     // reserve and write E

//     void* allocatedAddress_E = NULL;
//     const int sizeToAllocate_E = 1;
//     uint8_t dataToStore_E[1] = {0x41};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_E, &allocatedAddress_E);

//     assertAllocatedMemory(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     assertAllocatedMemory(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
//     assertAllocatedMemory(dataToStore_D, allocatedAddress_D, sizeToAllocate_D);
//     TEST_ASSERT_NULL(allocatedAddress_E);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(FAIL_STATUS, heapInstance.lastFailedAllocateSize);
//     TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
// };



// void test_allocateAndFreeComplexPseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     int status;

//     // reserve and write A
//     void* allocatedAddress_A = NULL;
//     const int sizeToAllocate_A = 3;
//     uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

//     // reserve and write B

//     void* allocatedAddress_B = NULL;
//     const int sizeToAllocate_B = 2;
//     uint8_t dataToStore_B[2] = {0x11, 0x12};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

//     status = freePseudoHeap(&heapInstance, &allocatedAddress_A, sizeToAllocate_A);

//     // reserve and write C

//     void* allocatedAddress_C = NULL;
//     const int sizeToAllocate_C = 2;
//     uint8_t dataToStore_C[2] = {0x21, 0x22};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_C, dataToStore_C, sizeToAllocate_C);

//     // reserve and write D

//     void* allocatedAddress_D = NULL;
//     const int sizeToAllocate_D = 1;
//     uint8_t dataToStore_D[1] = {0x31};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);

//     if (!status) writeToPseudoHeapAddress(allocatedAddress_D, dataToStore_D, sizeToAllocate_D);

//     status = freePseudoHeap(&heapInstance, &allocatedAddress_D, sizeToAllocate_D);

//     // reserve and write E

//     void* allocatedAddress_E = NULL;
//     const int sizeToAllocate_E = 3;
//     uint8_t dataToStore_E[3] = {0x41, 0x42, 0x43};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_E, &allocatedAddress_E);

//     if (!status) writeToPseudoHeapAddress(allocatedAddress_E, dataToStore_E, sizeToAllocate_E);

//     TEST_ASSERT_NULL(allocatedAddress_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     assertAllocatedMemory(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
//     TEST_ASSERT_NULL(allocatedAddress_D);
//     assertAllocatedMemory(dataToStore_E, allocatedAddress_E, sizeToAllocate_E);
//     TEST_ASSERT_EQUAL_INT(FAIL_STATUS, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);

//     // reserve and write F
//     void* allocatedAddress_F = NULL;
//     const int sizeToAllocate_F = 2;
//     uint8_t dataToStore_F[2] = {0x51, 0x52};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_F, &allocatedAddress_F);

//     TEST_ASSERT_NULL(allocatedAddress_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     assertAllocatedMemory(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
//     TEST_ASSERT_NULL(allocatedAddress_D);
//     assertAllocatedMemory(dataToStore_E, allocatedAddress_E, sizeToAllocate_E);
//     TEST_ASSERT_NULL(allocatedAddress_F);
//     TEST_ASSERT_EQUAL_INT(FAIL_STATUS, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(2, heapInstance.lastFailedAllocateSize);
//     TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
// };

// void test_fragmentedDetectPseudoHeap_small(void){
//     uint8_t testHeapMemory[TEST_HEAP_LEN_SMALL];

//     struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN_SMALL, BLOCK_SIZE_SMALL);

//     int status;

//     // reserve and write A
//     void* allocatedAddress_A = NULL;
//     const int sizeToAllocate_A = 1;
//     uint8_t dataToStore_A[1] = {0x01};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

//     // reserve and write B

//     void* allocatedAddress_B = NULL;
//     const int sizeToAllocate_B = 6;
//     uint8_t dataToStore_B[6] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
//     if (!status) writeToPseudoHeapAddress(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

//     status = freePseudoHeap(&heapInstance, &allocatedAddress_A, sizeToAllocate_A);

//     // reserve and write C

//     void* allocatedAddress_C = NULL;
//     const int sizeToAllocate_C = 2;
//     uint8_t dataToStore_C[2] = {0x21, 0x22};

//     status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);

//     TEST_ASSERT_NULL(allocatedAddress_A);
//     assertAllocatedMemory(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
//     TEST_ASSERT_NULL(allocatedAddress_C);
//     TEST_ASSERT_EQUAL_INT(2, heapInstance.freeBlocks);
//     TEST_ASSERT_EQUAL_INT(2, status);
// };

void run_all_tests_PseudoHeapManager_small(void){
    // RUN_TEST(test_createPseudoHeap_small); // create track small
    // RUN_TEST(test_createPseudoHeap_large); // create track large
    // RUN_TEST(test_allocatePseudoHeap_small); // reserve heap
    // RUN_TEST(test_fillPseudoHeap_small); // fill heap
    // RUN_TEST(test_allocateAndFreePseudoHeap_small); // fill heap, try adding
    // RUN_TEST(test_fillPlusPseudoHeap_small); // fill heap, try adding
    // RUN_TEST(test_allocateAndFreeComplexPseudoHeap_small); // reserve and free heap multiple times
    // RUN_TEST(test_fragmentedDetectPseudoHeap_small); // test if fragmentation is detected
};