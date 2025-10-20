#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TICK_LENGTH 8 // length of 1 tick, needs to be updated manually
#define TICK_TIME_SIZE 2
#define TICK_DATA_SIZE 4
#define TICK_FLAG_SIZE 1

#define NAME 1 // name for test tracks
#define LENGTH 400 // test track length in ms 

#define SECOND_TICK 0x0001
#define BYTES_IN_TRACK MAX_TICKS_PER_TRACK*TICK_LENGTH

#define SUCCESS_STATUS 0
#define FAIL_STATUS 1

// be warned, this file contains a lot of hard-to-follow memory operations
// this is because we are testing hard-to-follow memory operations
// the only other option is hard coding expected results
// and in addition to better code, this works on any MAX_TICKS_PER_TRACK within reason

void test_prereqConfig(void){
    // if the max ticks per track is less than 4, it makes no sense to continue
    // what sequencer handles less than 4 notes per track like come on
    // it *might* work but also unit tests fail then regardless
    // max is around 60k
    const uint8_t actualTickLength = sizeof(struct Tick);
    TEST_ASSERT_EQUAL_INT(actualTickLength, TICK_LENGTH);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(4, MAX_TICKS_PER_TRACK);
}

void test_createTrack(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    // EXPECTED RESULT: empty track with default values
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK);
    TEST_ASSERT_EQUAL_INT(NAME, testTrack.name);
    TEST_ASSERT_EQUAL_INT(LENGTH, testTrack.length);
    TEST_ASSERT_EQUAL_INT(0, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_PTR(NULL, testTrack.heapInstance);
};

// this fill track with time 0x0000 data 0x11 0x11 0x11 0x11 flag 0x12
// then increments time, flag and each data byte by 1 and repeats
void helperFillTrackToPoint(struct Tick* targetTicksMemory, struct TrackData* testTrack, int tickCount){
    if (tickCount==0) return;

    uint16_t timeBytes = 0x0000;
    uint8_t dataByte = 0x11;
    uint8_t targetFlag = 0x12;
    uint8_t targetByte8 = 0x00; // this is the unused byte in the struct

    TICK_T targetTime;
    union TickData targetTickData;
    struct Tick targetFirstRow;
    
    for (uint32_t currentTick = 0; currentTick<MAX_TICKS_PER_TRACK && currentTick<tickCount; currentTick++){
        memcpy(&targetTime, &timeBytes, sizeof(TICK_T)); // set new target time
        memset(&targetTickData, dataByte, TICK_DATA_SIZE); // set new target data
        
        targetFirstRow.time = targetTime;
        targetFirstRow.data = targetTickData;
        targetFirstRow.flag = targetFlag;
        targetFirstRow.byte8 = targetByte8;
        
        memcpy(&targetTicksMemory[currentTick], &targetFirstRow, TICK_LENGTH); // add new tick to target 
        addTickToTrack(testTrack, targetTime, &targetTickData, targetFlag);

        timeBytes++;
        dataByte++;
        targetFlag++;
    }
}

void test_addTickToTrack_first(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK);

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    // insert one tick
    helperFillTrackToPoint(targetTicksMemory, &testTrack, 1);

    // EXPECTED RESULT: 1 tick is added
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(1, testTrack.ticksUsed);
};

void test_addTickToTrack_fill(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    helperFillTrackToPoint(targetTicksMemory, &testTrack, MAX_TICKS_PER_TRACK);

    // EXPECTED RESULT: track is full, all ticks added
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, testTrack.ticksUsed);
};

void test_addTickToTrack_fill_plus(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    helperFillTrackToPoint(targetTicksMemory, &testTrack, MAX_TICKS_PER_TRACK);

    // make a new tick, try adding it
    TICK_T targetTime = 0x1234;
    union TickData targetTickData;
    memset(&targetTickData, 0x10, TICK_DATA_SIZE); // make new target data

    struct Tick targetFirstRow;
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;

    int status = addTickToTrack(&testTrack, targetTime, &targetTickData, 0x10);

    // EXPECTED RESULT: track is full, all ticks *except last* added
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
};

void test_addTickToTrack_one_third_full(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    // fill track to 1/3
    int oneThirdFull = MAX_TICKS_PER_TRACK/3;
    helperFillTrackToPoint(targetTicksMemory, &testTrack, oneThirdFull);

    // EXPECTED RESULT: a track is filled to 1/3 with ticks added
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(oneThirdFull, testTrack.ticksUsed);
};

void test_addTickToTrack_middle_insert(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    const TICK_COUNT_T lastTickInTrack = MAX_TICKS_PER_TRACK-1;

    // fill track to 1 from max
    helperFillTrackToPoint(targetTicksMemory, &testTrack, lastTickInTrack);

    int middleOfTrack = MAX_TICKS_PER_TRACK/2;
    
    memmove(&targetTicksMemory[middleOfTrack+1], &targetTicksMemory[middleOfTrack], 
        (TICK_LENGTH*(lastTickInTrack-middleOfTrack))); // move over the ticks after middle tick in target

    // create a new tick and add it to track
    TICK_T targetTime;
    memcpy(&targetTime, &targetTicksMemory[middleOfTrack+1], 2); // get target time from middle tick
    targetTime++;

    union TickData targetTickData;
    memset(&targetTickData, 0x10, TICK_DATA_SIZE); // set new tick data

    struct Tick targetFirstRow;
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;

    memcpy(&targetTicksMemory[middleOfTrack+1], &targetFirstRow, TICK_LENGTH); // write new row to target

    int status = addTickToTrack(&testTrack, targetTime, &targetTickData, 0x10);

    // EXPECTED RESULT: track filled, last tick inserted at expected position
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_remTickFromTrack(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    // fill track to 3/4 and remove second tick
    int threeQuartersFull = 3*MAX_TICKS_PER_TRACK/4;
    helperFillTrackToPoint(targetTicksMemory, &testTrack, threeQuartersFull);

    int status = remTickFromTrack(&testTrack, SECOND_TICK); 

    const TICK_COUNT_T lastTick = threeQuartersFull-1;
    const TICK_COUNT_T secondToLastTick = threeQuartersFull-2;
    const TICK_COUNT_T ticksCountFromSecondToLast = threeQuartersFull-2;
    const TICK_COUNT_T ticksCountAfterRemoval = threeQuartersFull-1;

    // adjust target to expected result, old data is ignored and written over but not wiped
    // write over old tick
    memmove(&targetTicksMemory[1], &targetTicksMemory[2], ticksCountFromSecondToLast*TICK_LENGTH);
    // copy last tick to the now empty position in target
    memcpy(&targetTicksMemory[secondToLastTick], &targetTicksMemory[lastTick], TICK_LENGTH);
    
    // EXPECTED RESULT: track filled to 3/4 except removed tick SECOND_TICK
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(ticksCountAfterRemoval, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_changeTickInTrack(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    // fill track to 3/4 and change tick data
    int threeQuartersFull = 3*MAX_TICKS_PER_TRACK/4;
    helperFillTrackToPoint(targetTicksMemory, &testTrack, threeQuartersFull);

    const uint8_t testNewDataByte = 0xDA;
    const uint8_t testNewFlagByte = 0xFA;

    union TickData targetTickData;
    memset(&targetTickData, testNewDataByte, TICK_DATA_SIZE);  // prepare new tick data for track
    memset(&((uint8_t*)targetTicksMemory)[(TICK_LENGTH*1)+TICK_TIME_SIZE], testNewDataByte, 4); // change tick data in target
    memset(&((uint8_t*)targetTicksMemory)[(TICK_LENGTH*1)+TICK_TIME_SIZE+TICK_DATA_SIZE], testNewFlagByte, 1); // change tick flag in target

    int status = changeTickInTrack(&testTrack, SECOND_TICK, &targetTickData, testNewFlagByte);

    // EXPECTED RESULT: tick SECOND_TICK data changed
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(threeQuartersFull, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_moveTickInTrack(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);

    helperFillTrackToPoint(targetTicksMemory, &testTrack, MAX_TICKS_PER_TRACK-1);

    struct Tick tmpLine;
    memcpy(&tmpLine, &targetTicksMemory[1], TICK_LENGTH); // copy target line to be moved to tmp space
    memmove(&targetTicksMemory[1], &targetTicksMemory[2], 
        (TICK_LENGTH*(MAX_TICKS_PER_TRACK-3))); // move ticks back over old tick

    TICK_T targetTime;
    memcpy(&targetTime, &targetTicksMemory[(MAX_TICKS_PER_TRACK-4)], 2); // get time of tick before new one
    targetTime++;

    memcpy(&targetTicksMemory[(MAX_TICKS_PER_TRACK-3)], &tmpLine, TICK_LENGTH); // copy tmp line to target
    memcpy(&targetTicksMemory[(MAX_TICKS_PER_TRACK-3)], &targetTime, 2); // set new time 

    int status = moveTickInTrack(&testTrack, SECOND_TICK, targetTime);

    // EXPECTED RESULT: tick SECOND_TICK moved to new time, moving position 1 to max -3
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK-1, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void test_remMovChngNonExistentTick(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);
    int status = -1;

    int threeQuartersFull = 3*MAX_TICKS_PER_TRACK/4;
    helperFillTrackToPoint(targetTicksMemory, &testTrack, threeQuartersFull);

    status = remTickFromTrack(&testTrack, 0xFFFF);

    // EXPECTED RESULT: track filled to 3/4, non existent tick not removed
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(threeQuartersFull, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);

    union TickData newTickData;
    memset(&newTickData, 0x11, TICK_DATA_SIZE); // create new tick data

    status = changeTickInTrack(&testTrack, 0xFFFF, &newTickData, 0x11);

    // EXPECTED RESULT: track filled to 3/4, non existent tick not changed
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(threeQuartersFull, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);

    status = moveTickInTrack(&testTrack, 0xFFFF, 0xFFF1);

    // EXPECTED RESULT: track filled to 3/4, non existent tick not moved
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(threeQuartersFull, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(FAIL_STATUS, status);
};

void test_addTickToTrack_add_remove_add(void){
    struct Tick targetTicksMemory[MAX_TICKS_PER_TRACK];
    memset(targetTicksMemory, 0x00, BYTES_IN_TRACK); 

    struct TrackData testTrack = createTrack(NAME, LENGTH);
    int status = -1;

    // fill track to three quarters
    int threeQuartersFull = 3*MAX_TICKS_PER_TRACK/4;
    helperFillTrackToPoint(targetTicksMemory, &testTrack, threeQuartersFull);

    status = remTickFromTrack(&testTrack, SECOND_TICK);

    const TICK_COUNT_T lastTick = threeQuartersFull-1;
    const TICK_COUNT_T secondToLastTick = threeQuartersFull-2;
    const TICK_COUNT_T ticksCountFromSecondToLast = threeQuartersFull-2;
    const TICK_COUNT_T ticksCountAfterRemoval = threeQuartersFull-1;

    // adjust target to expected result, old data is ignored and written over but not wiped
    // write over old tick
    memmove(&targetTicksMemory[1], &targetTicksMemory[2], ticksCountFromSecondToLast*TICK_LENGTH);
    // copy last tick to the now empty position in target
    memcpy(&targetTicksMemory[secondToLastTick], &targetTicksMemory[lastTick], TICK_LENGTH);

    // EXPECTED RESULT: track filled to 3/4, tick SECOND_TICK removed
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(ticksCountAfterRemoval, testTrack.ticksUsed);

    const TICK_COUNT_T nowDeletedLastTick = threeQuartersFull-1;

    // this is just adding the new tick
    TICK_T targetTime;
    memcpy(&targetTime, &targetTicksMemory[nowDeletedLastTick], 2); // set new target tick time
    targetTime++;

    union TickData targetTickData;
    memset(&targetTickData, 0x10, TICK_DATA_SIZE); // set new tick data

    struct Tick targetFirstRow;
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;

    memcpy(&targetTicksMemory[lastTick], &targetFirstRow, TICK_LENGTH); // add new tick to target

    status = addTickToTrack(&testTrack, targetTime, &targetTickData, 0x10);
    
    // EXPECTED RESULT: another tick added, succesfully overwriting old data
    TEST_ASSERT_EQUAL_MEMORY(targetTicksMemory, testTrack.ticks, BYTES_IN_TRACK); 
    TEST_ASSERT_EQUAL_INT(threeQuartersFull, testTrack.ticksUsed);
    TEST_ASSERT_EQUAL_INT(SUCCESS_STATUS, status);
};

void run_all_tests_TrackData(void){
    RUN_TEST(test_prereqConfig); // some prerequisites
    RUN_TEST(test_createTrack); // create track
    RUN_TEST(test_addTickToTrack_first); // add first tick
    RUN_TEST(test_addTickToTrack_fill); // fill track fully
    RUN_TEST(test_addTickToTrack_fill_plus); // fill track fully and try to add
    RUN_TEST(test_addTickToTrack_add_remove_add); // remove then add, tests overwriting of old mem
    RUN_TEST(test_addTickToTrack_one_third_full); // fill to half
    RUN_TEST(test_addTickToTrack_middle_insert); // add tick that would be in middle
    RUN_TEST(test_remTickFromTrack); // remove tick
    RUN_TEST(test_changeTickInTrack); // change tick
    RUN_TEST(test_moveTickInTrack); // move tick
    RUN_TEST(test_remMovChngNonExistentTick); // all operations on non existent tick
};