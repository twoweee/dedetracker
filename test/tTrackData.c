#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TICK_LENGTH_M 8 // length of 1 tick, needs to be updated manually
#define TICK_DATA_LENGTH 4 // length of ticks data section

#define NAME 1 // name for test tracks
#define LENGTH 400 // test track length in ms 

// be warned, this file contains a lot of hard-to-follow memory operations
// this is because we are testing hard-to-follow memory operations
// the only other option is hard coding expected results
// and in addition to better code, this works on any MAX_TICKS_PER_TRACK within reason

void test_prereqConfig(void){
    // if the max ticks per track is less than 4, it makes no sense to continue
    // what sequencer handles less than 4 notes per track like come on
    // it *might* work but also unit tests fail then regardless
    TEST_ASSERT_GREATER_OR_EQUAL_INT(4, MAX_TICKS_PER_TRACK);
}

void test_createTrack(void){
    uint8_t empty[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // EXPECTED RESULT: empty track with default values
    TEST_ASSERT_EQUAL_MEMORY(empty, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); // ticks
    TEST_ASSERT_EQUAL_INT(NAME, track1.name); // name
    TEST_ASSERT_EQUAL_INT(LENGTH, track1.length); // length ms 
    TEST_ASSERT_EQUAL_INT(0, track1.ticksUsed); // ticks used
    TEST_ASSERT_EQUAL_PTR(NULL, track1.heapInstance); // pseudo heap pointer
};

// this fill track with time 0x1010 data 0x11111111 flag 0x12
// then increments each byte by 1 and repeats
void helperFillTrackToPoint(uint8_t* target, struct TrackData* track1, int tickCount){
    if (tickCount==0) return;
    TICK_T targetTime;
    union TickData targetTickData;
    struct Tick targetFirstRow;

    uint8_t timeByte = 0x10;
    uint8_t dataByte = 0x11;
    uint8_t targetFlag = 0x12;
    uint8_t targetByte8 = 0x00; // last unused byte

    for (int currentTick = 0; currentTick<MAX_TICKS_PER_TRACK && currentTick<tickCount; currentTick++){
        memset(&targetTime, timeByte, sizeof(TICK_T)); // set new target time
        memset(&targetTickData, dataByte, sizeof(union TickData)); // set new target data
        
        targetFirstRow.time = targetTime;
        targetFirstRow.data = targetTickData;
        targetFirstRow.flag = targetFlag;
        targetFirstRow.byte8 = targetByte8;
        
        // add new tick to target
        memcpy(target+(currentTick*TICK_LENGTH_M), &targetFirstRow, TICK_LENGTH_M); 
        addTickToTrack(track1, targetTime, &targetTickData, targetFlag);

        timeByte++;
        dataByte++;
        targetFlag++;
    }
}

void test_addTickToTrack_first(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // insert one tick
    helperFillTrackToPoint(target, &track1, 1);

    // EXPECTED RESULT: 1 tick is added
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(1, track1.ticksUsed);
};

void test_addTickToTrack_fill(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // fill track
    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK);

    // EXPECTED RESULT: track is full, all ticks added
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, track1.ticksUsed);
};

void test_addTickToTrack_fill_plus(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // fill track
    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK);

    // make a new tick, try adding it
    TICK_T targetTime = 0x1234;
    union TickData targetTickData;
    struct Tick targetFirstRow;
    memset(&targetTickData, 0x10, sizeof(union TickData)); // make new target data
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;
    int status = addTickToTrack(&track1, targetTime, &targetTickData, 0x10);

    // EXPECTED RESULT: track is full, all ticks *except last* added
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(1, status);
};

void test_addTickToTrack_half_full(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // fill track to 1/3
    int third = MAX_TICKS_PER_TRACK/3;
    helperFillTrackToPoint(target, &track1, third);

    // EXPECTED RESULT: a track is filled to 1/3 with ticks added
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(third, track1.ticksUsed);
};

void test_addTickToTrack_middle_insert(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // fill track to 1 from max
    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK-1);

    int middle = MAX_TICKS_PER_TRACK/2;
    // move over the ticks after middle tick in target
    memmove(target+((middle+1)*TICK_LENGTH_M), target+(middle*TICK_LENGTH_M), 
        (TICK_LENGTH_M*(MAX_TICKS_PER_TRACK-1-middle)));

    // create a new tick and add it to track
    TICK_T targetTime;
    memcpy(&targetTime, target+((middle+1)*TICK_LENGTH_M), 2); // get target time from middle tick
    targetTime++;
    union TickData targetTickData;
    struct Tick targetFirstRow;
    memset(&targetTickData, 0x10, sizeof(union TickData)); // set new tick data
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;
    memcpy(target+((middle+1)*TICK_LENGTH_M), &targetFirstRow, TICK_LENGTH_M); // write new row to target
    int status = addTickToTrack(&track1, targetTime, &targetTickData, 0x10);

    // EXPECTED RESULT: track filled, last tick inserted at expected position
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_remTickFromTrack(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // fill track to 3/4 and remove tick 0x1111
    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    int status = remTickFromTrack(&track1, 0x1111); // as per fill helper function, 0x1111 is second tick

    // adjust target to expected result, old data is ignored and written over but not wiped
    // write over old tick
    memmove(target+(TICK_LENGTH_M), target+(2*TICK_LENGTH_M), (threequarters-2)*TICK_LENGTH_M);
    // copy last tick to the now empty position in target
    memcpy(target+((threequarters-2)*TICK_LENGTH_M), target+((threequarters-1)*TICK_LENGTH_M), TICK_LENGTH_M);
    
    // EXPECTED RESULT: track filled to 3/4 except removed tick 0x1111
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters-1, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_changeTickInTrack(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    // fill track to 3/4 and change tick data
    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    union TickData targetTickData;
    memset(&targetTickData, 0x11, sizeof(union TickData));  // prepare new tick data for track
    memset(target+(TICK_LENGTH_M)+2, 0x11, 5); // change tick data in target
    int status = changeTickInTrack(&track1, 0x1111, &targetTickData, 0x11);

    // EXPECTED RESULT: tick 0x1111 data changed
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_moveTickInTrack(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);
    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK-1);

    // I MIGHT HAVE EXPLAINED THE POSITIONS WRONG IN COMMENTS BUT THE TEST WORKS
    struct Tick tmpLine;
    memcpy(&tmpLine, target+(TICK_LENGTH_M), TICK_LENGTH_M); // copy target line to be moved to tmp space
    memmove(target+(TICK_LENGTH_M), target+(2*TICK_LENGTH_M), 
        (TICK_LENGTH_M*(MAX_TICKS_PER_TRACK-3))); // move ticks 1 to max -3 one position back in target

    TICK_T targetTime;
    memcpy(&targetTime, target+((MAX_TICKS_PER_TRACK-4)*TICK_LENGTH_M), 2); // get time of tick max -4
    targetTime++;

    memcpy(target+((MAX_TICKS_PER_TRACK-3)*TICK_LENGTH_M), &tmpLine, TICK_LENGTH_M); // copy tmp line to target
    memcpy(target+((MAX_TICKS_PER_TRACK-3)*TICK_LENGTH_M), &targetTime, 2); // set new time of tmp line

    // move tick 1 to position max -3
    int status = moveTickInTrack(&track1, 0x1111, targetTime);
    // EXPECTED RESULT: tick 0x1111 moved to new time, moving position 1 to max -3
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK-1, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void test_remMovChngNonExistentTick(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);
    int status = -1;

    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    status = remTickFromTrack(&track1, 0xFFFF);

    // EXPECTED RESULT: track filled to 3/4, non existent tick not removed
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(1, status);

    union TickData newTickData;
    memset(&newTickData, 0x11, sizeof(union TickData)); // create new tick data
    status = changeTickInTrack(&track1, 0xFFFF, &newTickData, 0x11);

    // EXPECTED RESULT: track filled to 3/4, non existent tick not changed
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(1, status);

    status = moveTickInTrack(&track1, 0xFFFF, 0xFFF1);

    // EXPECTED RESULT: track filled to 3/4, non existent tick not moved
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(1, status);
};

void test_addTickToTrack_add_remove_add(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);
    int status = -1;

    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    status = remTickFromTrack(&track1, 0x1111); // as per fill helper function, 0x1111 is second tick

    // adjust target to expected result, old data is ignored and written over but not wiped
    // write over old tick
    memmove(target+(TICK_LENGTH_M), target+(2*TICK_LENGTH_M), (threequarters-2)*TICK_LENGTH_M);
    // copy last tick to the now empty position in target
    memcpy(target+((threequarters-2)*TICK_LENGTH_M), target+((threequarters-1)*TICK_LENGTH_M), TICK_LENGTH_M);

    // EXPECTED RESULT: track filled to 3/4, tick 0x1111 removed
    TEST_ASSERT_EQUAL_INT(0, status);
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters-1, track1.ticksUsed);

    // this is just adding the new tick
    TICK_T targetTime;
    memcpy(&targetTime, target+((threequarters-1)*TICK_LENGTH_M), 2); // set new target tick time
    targetTime++;
    union TickData targetTickData;
    struct Tick targetFirstRow;
    memset(&targetTickData, 0x10, sizeof(union TickData)); // set new tick data
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;
    memcpy(target+((threequarters-1)*TICK_LENGTH_M), &targetFirstRow, TICK_LENGTH_M); // add new tick to target
    status = addTickToTrack(&track1, targetTime, &targetTickData, 0x10);
    
    // EXPECTED RESULT: another tick added, succesfully overwriting old data
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
    TEST_ASSERT_EQUAL_INT(0, status);
};

void run_all_tests_TrackData(void){
    RUN_TEST(test_prereqConfig); // some prerequisites
    RUN_TEST(test_createTrack); // create track
    RUN_TEST(test_addTickToTrack_first); // add first tick
    RUN_TEST(test_addTickToTrack_fill); // fill track fully
    RUN_TEST(test_addTickToTrack_fill_plus); // fill track fully and try to add
    RUN_TEST(test_addTickToTrack_add_remove_add); // remove then add, tests overwriting of old mem
    RUN_TEST(test_addTickToTrack_half_full); // fill to half
    RUN_TEST(test_addTickToTrack_middle_insert); // add tick that would be in middle
    RUN_TEST(test_remTickFromTrack); // remove tick
    RUN_TEST(test_changeTickInTrack); // change tick
    RUN_TEST(test_moveTickInTrack); // move tick
    RUN_TEST(test_remMovChngNonExistentTick); // all operations on non existent tick
};