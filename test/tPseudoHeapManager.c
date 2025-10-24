#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TEST_HEAP_LEN 8 // changing this will break some tests

void test_prereqConfigChanged(void){
    // this is effectively a reminder that tests will break if this is changed
    TEST_ASSERT_GREATER_OR_EQUAL_INT(8, TEST_HEAP_LEN);
}

void test_createPseudoHeap(void){
    uint8_t testHeapMemory[TEST_HEAP_LEN];
    uint8_t targetHeapMemory[TEST_HEAP_LEN] = {0x00};

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN);

    // EXPECTED RESULT: empty heap with default values
    TEST_ASSERT_EQUAL_MEMORY(targetHeapMemory, heapInstance.memSpace, TEST_HEAP_LEN);
    TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN, heapInstance.length);
    TEST_ASSERT_EQUAL_INT(-1, heapInstance.lastUsedByte);
    TEST_ASSERT_EQUAL_INT(UINT16_MAX, heapInstance.lastFailedAllocateSize);
};

void test_reservePseudoHeap(void){
    uint8_t testHeapMemory[TEST_HEAP_LEN];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN);

    void* allocatedAddress = NULL;
    const int sizeToAllocate = 3;
    uint8_t dataToStore[3] = {0x01, 0x02, 0x03};
    int status = reservePseudoHeap(&heapInstance, sizeToAllocate, &allocatedAddress);
    
    if (!status) memcpy(allocatedAddress, dataToStore, 3); // write to newly allocated address, it will be memcpy in real use

    // EXPECTED RESULT: allocated memory of len 3
    TEST_ASSERT_EQUAL_MEMORY(dataToStore, allocatedAddress, sizeToAllocate);
    TEST_ASSERT_EQUAL_INT(2, heapInstance.lastUsedByte);
    TEST_ASSERT_EQUAL_INT(UINT16_MAX, heapInstance.lastFailedAllocateSize);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_fillPseudoHeap(void){
    uint8_t testHeapMemory[TEST_HEAP_LEN];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN);

    int status;

    // reserve and write A
    void* allocatedAddress_A = NULL;
    const int sizeToAllocate_A = 3;
    uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
    if (!status) memcpy(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

    TEST_ASSERT_EQUAL_MEMORY(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
    TEST_ASSERT_EQUAL_INT(UINT16_MAX, heapInstance.lastFailedAllocateSize);
    TEST_ASSERT_EQUAL_INT(0, status);

    // reserve and write B
    void* allocatedAddress_B = NULL;
    const int sizeToAllocate_B = 1;
    uint8_t dataToStore_B[2] = {0x11};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
    if (!status) memcpy(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

    TEST_ASSERT_EQUAL_MEMORY(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
    TEST_ASSERT_EQUAL_INT(0, status);

    // reserve and write C

    void* allocatedAddress_C = NULL;
    const int sizeToAllocate_C = 2;
    uint8_t dataToStore_C[2] = {0x21, 0x22};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
    if (!status) memcpy(allocatedAddress_C, dataToStore_C, sizeToAllocate_C);

    TEST_ASSERT_EQUAL_MEMORY(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
    TEST_ASSERT_EQUAL_INT(0, status);

    // reserve and write D

    void* allocatedAddress_D = NULL;
    const int sizeToAllocate_D = 2;
    uint8_t dataToStore_D[2] = {0x31, 0x32};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);
    
    if (!status) memcpy(allocatedAddress_D, dataToStore_D, sizeToAllocate_D);

    TEST_ASSERT_EQUAL_MEMORY(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_D, allocatedAddress_D, sizeToAllocate_D);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_fillPlusPseudoHeap(void){
    uint8_t testHeapMemory[TEST_HEAP_LEN];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN);

    int status;

    // reserve and write A
    void* allocatedAddress_A = NULL;
    const int sizeToAllocate_A = 3;
    uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
    if (!status) memcpy(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

    // reserve and write B
    void* allocatedAddress_B = NULL;
    const int sizeToAllocate_B = 1;
    uint8_t dataToStore_B[2] = {0x11};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
    if (!status) memcpy(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

    // reserve and write C

    void* allocatedAddress_C = NULL;
    const int sizeToAllocate_C = 2;
    uint8_t dataToStore_C[2] = {0x21, 0x22};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
    if (!status) memcpy(allocatedAddress_C, dataToStore_C, sizeToAllocate_C);

    // reserve and write D

    void* allocatedAddress_D = NULL;
    const int sizeToAllocate_D = 2;
    uint8_t dataToStore_D[2] = {0x31, 0x32};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);

    if (!status) memcpy(allocatedAddress_D, dataToStore_D, sizeToAllocate_D);

    // reserve and write E

    void* allocatedAddress_E = NULL;
    const int sizeToAllocate_E = 1;
    uint8_t dataToStore_E[1] = {0x41};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_E, &allocatedAddress_E);

    TEST_ASSERT_EQUAL_MEMORY(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_D, allocatedAddress_D, sizeToAllocate_D);
    TEST_ASSERT_NULL(allocatedAddress_E);
    TEST_ASSERT_EQUAL_INT(1, status);
};

void test_allocateAndFreePseudoHeap(void){
    uint8_t testHeapMemory[TEST_HEAP_LEN];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN);

    int status;

    // reserve and write A
    void* allocatedAddress_A = NULL;
    const int sizeToAllocate_A = 3;
    uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
    if (!status) memcpy(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

    // reserve and write B

    void* allocatedAddress_B = NULL;
    const int sizeToAllocate_B = 2;
    uint8_t dataToStore_B[2] = {0x21, 0x22};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
    if (!status) memcpy(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

    status = freePseudoHeap(&heapInstance, &allocatedAddress_A, sizeToAllocate_A);

    TEST_ASSERT_NULL(allocatedAddress_A);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_reserveAndFreeComboPseudoHeap(void){
    uint8_t testHeapMemory[TEST_HEAP_LEN];

    struct PseudoHeapInstance heapInstance = initializePseudoHeap(testHeapMemory, TEST_HEAP_LEN);

    int status;

    // reserve and write A
    void* allocatedAddress_A = NULL;
    const int sizeToAllocate_A = 3;
    uint8_t dataToStore_A[3] = {0x01, 0x02, 0x03};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_A, &allocatedAddress_A);
    
    if (!status) memcpy(allocatedAddress_A, dataToStore_A, sizeToAllocate_A);

    // reserve and write B
    void* allocatedAddress_B = NULL;
    const int sizeToAllocate_B = 1;
    uint8_t dataToStore_B[2] = {0x11};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_B, &allocatedAddress_B);
    
    if (!status) memcpy(allocatedAddress_B, dataToStore_B, sizeToAllocate_B);

    // reserve and write C

    void* allocatedAddress_C = NULL;
    const int sizeToAllocate_C = 2;
    uint8_t dataToStore_C[2] = {0x21, 0x22};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_C, &allocatedAddress_C);
    
    if (!status) memcpy(allocatedAddress_C, dataToStore_C, sizeToAllocate_C);

    // reserve and write D

    void* allocatedAddress_D = NULL;
    const int sizeToAllocate_D = 2;
    uint8_t dataToStore_D[2] = {0x31, 0x32};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_D, &allocatedAddress_D);

    // reserve and write E

    void* allocatedAddress_E = NULL;
    const int sizeToAllocate_E = 1;
    uint8_t dataToStore_E[1] = {0x41};

    status = reservePseudoHeap(&heapInstance, sizeToAllocate_E, &allocatedAddress_E);

    TEST_ASSERT_EQUAL_MEMORY(dataToStore_A, allocatedAddress_A, sizeToAllocate_A);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_B, allocatedAddress_B, sizeToAllocate_B);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_C, allocatedAddress_C, sizeToAllocate_C);
    TEST_ASSERT_EQUAL_MEMORY(dataToStore_D, allocatedAddress_D, sizeToAllocate_D);
    TEST_ASSERT_NULL(allocatedAddress_E);
    TEST_ASSERT_EQUAL_INT(1, status);
};

void run_all_tests_PseudoHeapManager(void){
    RUN_TEST(test_prereqConfigChanged); // prereq
    RUN_TEST(test_createPseudoHeap); // create track
    RUN_TEST(test_reservePseudoHeap); // reserve heap
    RUN_TEST(test_fillPseudoHeap); // fill heap
    RUN_TEST(test_fillPlusPseudoHeap); // fill heap, try adding
    RUN_TEST(test_allocateAndFreePseudoHeap); // fill heap, try adding
    RUN_TEST(test_reserveAndFreeComboPseudoHeap); // reserve and free heap multiple times
};