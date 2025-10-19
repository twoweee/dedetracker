#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TEST_HEAP_LEN 8 // changing this will break some tests

void test_prereqConfigChanged(void){
    // this is effectively a reminder that tests will break if this is changed
    TEST_ASSERT_GREATER_OR_EQUAL_INT(8, TEST_HEAP_LEN);
}

void test_createPseudoHeap(void){
    uint8_t heapMem[TEST_HEAP_LEN];
    uint8_t empty[TEST_HEAP_LEN] = {0x00};
    struct PseudoHeapInstance heap = initializePseudoHeap(heapMem, TEST_HEAP_LEN);
    TEST_ASSERT_EQUAL_MEMORY(empty, heap.memSpace, TEST_HEAP_LEN); // mem space
    TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN, heap.length); // length
    TEST_ASSERT_EQUAL_INT(-1, heap.lastUsedByte); // last used byte
    TEST_ASSERT_EQUAL_INT(UINT16_MAX, heap.lastFailedAllocateSize); // last failed allocate
};

void test_reservePseudoHeap(void){
    uint8_t heapMem[TEST_HEAP_LEN];
    uint8_t target[TEST_HEAP_LEN] = {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
    struct PseudoHeapInstance heap = initializePseudoHeap(heapMem, TEST_HEAP_LEN);
    void* var1;
    int status = reservePseudoHeap(&heap, 3, &var1);
    TEST_ASSERT_EQUAL_MEMORY(target, heap.memSpace, TEST_HEAP_LEN);
    TEST_ASSERT_EQUAL_INT(TEST_HEAP_LEN, heap.length); // length
    TEST_ASSERT_EQUAL_INT(2, heap.lastUsedByte); // last used byte
    TEST_ASSERT_EQUAL_INT(UINT16_MAX, heap.lastFailedAllocateSize); // last failed allocate
    TEST_ASSERT_EQUAL_INT(0, status);
};

void run_all_tests_PseudoHeapManager(void){
    RUN_TEST(test_prereqConfigChanged); // prereq
    RUN_TEST(test_createPseudoHeap); // create track
    RUN_TEST(test_reservePseudoHeap); // 
};