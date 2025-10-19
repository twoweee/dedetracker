#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TEST_HEAP_LEN 8

void test_createTrack(void){
    uint8_t heapMem[TEST_HEAP_LEN];
    struct PseudoHeapInstance HEAP = initializePseudoHeap(heapMem, TEST_HEAP_LEN);
    // TEST_ASSERT_EQUAL_MEMORY(empty, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); // ticks
    // TEST_ASSERT_EQUAL_INT(NAME, track1.name); // name
    // TEST_ASSERT_EQUAL_INT(LENGTH, track1.length); // length ms 
    // TEST_ASSERT_EQUAL_INT(0, track1.ticksUsed); // ticks used
    // TEST_ASSERT_EQUAL_PTR(NULL, track1.heapInstance); // pseudo heap pointer
};

void run_all_tests_TrackData(void){
    RUN_TEST(test_createTrack); // create track
};