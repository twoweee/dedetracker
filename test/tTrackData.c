#include "../src/TrackData.c"
#include "../third-party/test/unity/src/unity.h"
#include <stdlib.h>

#define TICK_LENGTH_M 8 // length of 1 tick, needs to be updated manually
#define TICK_DATA_LENGTH 4

#define NAME 1
#define LENGTH 400

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

    TEST_ASSERT_EQUAL_MEMORY(empty, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); // ticks
    TEST_ASSERT_EQUAL_INT(NAME, track1.name); // name
    TEST_ASSERT_EQUAL_INT(LENGTH, track1.length); // length ms 
    TEST_ASSERT_EQUAL_INT(0, track1.ticksUsed); // ticks used
    TEST_ASSERT_EQUAL_PTR(NULL, track1.heapInstance); // pseudo heap pointer
};

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
        memset(&targetTime, timeByte, sizeof(TICK_T));
        memset(&targetTickData, dataByte, sizeof(union TickData));
        
        targetFirstRow.time = targetTime;
        targetFirstRow.data = targetTickData;
        targetFirstRow.flag = targetFlag;
        targetFirstRow.byte8 = targetByte8;
        
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

    helperFillTrackToPoint(target, &track1, 1);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(1, track1.ticksUsed);
};

void test_addTickToTrack_fill(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, track1.ticksUsed);
};

void test_addTickToTrack_fill_plus(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK);

    // this is just adding the new tick
    TICK_T targetTime = 0x1234;
    targetTime++;
    union TickData targetTickData;
    struct Tick targetFirstRow;
    memset(&targetTickData, 0x10, sizeof(union TickData));
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;
    addTickToTrack(&track1, targetTime, &targetTickData, 0x10);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, track1.ticksUsed);
};

void test_addTickToTrack_half_full(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    int third = MAX_TICKS_PER_TRACK/3;
    helperFillTrackToPoint(target, &track1, third);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(third, track1.ticksUsed);
};

void test_addTickToTrack_middle_insert(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK-1);

    // move over the ticks in target 
    int middle = MAX_TICKS_PER_TRACK/2;
    memmove(target+((middle+1)*TICK_LENGTH_M), target+(middle*TICK_LENGTH_M), 
        (TICK_LENGTH_M*(MAX_TICKS_PER_TRACK-1-middle)));

    // this is just adding the new tick
    TICK_T targetTime;
    memcpy(&targetTime, target+((middle+1)*TICK_LENGTH_M), 2);
    targetTime++;
    union TickData targetTickData;
    struct Tick targetFirstRow;
    memset(&targetTickData, 0x10, sizeof(union TickData));
    targetFirstRow.time = targetTime;
    targetFirstRow.data = targetTickData;
    targetFirstRow.flag = 0x10;
    targetFirstRow.byte8 = 0x00;
    memcpy(target+((middle+1)*TICK_LENGTH_M), &targetFirstRow, TICK_LENGTH_M);
    addTickToTrack(&track1, targetTime, &targetTickData, 0x10);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK, track1.ticksUsed);
};

void test_remTickFromTrack(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    remTickFromTrack(&track1, 0x1111); // as per fill helper function, 0x1111 is second tick

    memmove(target+(1*TICK_LENGTH_M), target+(2*TICK_LENGTH_M), (threequarters-2)*TICK_LENGTH_M);
    memcpy(target+((threequarters-2)*TICK_LENGTH_M), target+((threequarters-1)*TICK_LENGTH_M), 1*TICK_LENGTH_M);
    
    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters-1, track1.ticksUsed);
};

void test_changeTickInTrack(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    union TickData targetTickData;
    memset(&targetTickData, 0x11, sizeof(union TickData));
    memset(target+(1*TICK_LENGTH_M)+2, 0x11, 5);

    changeTickInTrack(&track1, 0x1111, &targetTickData, 0x11);


    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
};

void test_moveTickInTrack(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);
    helperFillTrackToPoint(target, &track1, MAX_TICKS_PER_TRACK-1);

    struct Tick tmpLine;
    memcpy(&tmpLine, target+(1*TICK_LENGTH_M), TICK_LENGTH_M);

    memmove(target+(1*TICK_LENGTH_M), target+(2*TICK_LENGTH_M), 
        (TICK_LENGTH_M*(MAX_TICKS_PER_TRACK-3)));

    TICK_T targetTime;
    memcpy(&targetTime, target+((MAX_TICKS_PER_TRACK-4)*TICK_LENGTH_M), 2);
    targetTime++;

    memcpy(target+((MAX_TICKS_PER_TRACK-3)*TICK_LENGTH_M), &tmpLine, TICK_LENGTH_M);

    memcpy(target+((MAX_TICKS_PER_TRACK-3)*TICK_LENGTH_M), &targetTime, 2);

    moveTickInTrack(&track1, 0x1111, targetTime);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(MAX_TICKS_PER_TRACK-1, track1.ticksUsed);
};

void test_remMovChngNonExistentTick(void){
    uint8_t target[MAX_TICKS_PER_TRACK*TICK_LENGTH_M] = {0x00};
    struct TrackData track1 = createTrack(NAME, LENGTH);

    int threequarters = 3*(MAX_TICKS_PER_TRACK/4);
    helperFillTrackToPoint(target, &track1, threequarters);
    remTickFromTrack(&track1, 0xFFFF);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);

    union TickData targetTickData;
    memset(&targetTickData, 0x11, sizeof(union TickData));

    changeTickInTrack(&track1, 0xFFFF, &targetTickData, 0x11);


    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);

    moveTickInTrack(&track1, 0xFFFF, 0xFFF1);

    TEST_ASSERT_EQUAL_MEMORY(target, track1.ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH_M); 
    TEST_ASSERT_EQUAL_INT(threequarters, track1.ticksUsed);
};

void run_all_tests_TrackData(void){
    RUN_TEST(test_prereqConfig); // some prerequisites
    RUN_TEST(test_createTrack); // create track
    RUN_TEST(test_addTickToTrack_first); // add first tick
    RUN_TEST(test_addTickToTrack_fill); // fill track fully
    RUN_TEST(test_addTickToTrack_fill_plus); // fill track fully and try to add
    RUN_TEST(test_addTickToTrack_half_full); // fill to half
    RUN_TEST(test_addTickToTrack_middle_insert); // add tick that would be in middle
    RUN_TEST(test_remTickFromTrack); // remove tick
    RUN_TEST(test_changeTickInTrack); // change tick
    RUN_TEST(test_moveTickInTrack); // move tick
    RUN_TEST(test_remMovChngNonExistentTick); // all operations on non existent tick
};