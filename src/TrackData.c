#pragma once
#include <stdint.h>
#include <stdio.h>
#include "PseudoHeapManager.c"

#define TICK_T uint16_t // n-1 bits for tick, 1 bit for 
#define POINTER_T uint8_t // pointer to an address that is kept
#define MEM_LENGTH_T uint8_t  // length of data at the pointer
#define MAX_TICKS_PER_TRACK 20 // maximum ticks count per track
#define COUNT_SLOTS_T uint8_t // count taken up slots inside the array, should fit MAX_TICKS_PER_TRACK
#define TRACK_LENGTH_T uint32_t // track length in 1/TICK_PER_MS ms
#define TICK_PER_MS 100
#define NAME_T uint32_t // type for name

// #define DATA_LENGTH (sizeof(POINTER_T) + sizeof(MEM_LENGTH_T)) // length of data section
// #define TICK_LENGTH (sizeof(TICK_T) + DATA_LENGTH) 
// #define MAX_TICKS_PER_TRACK 20 // maximum ticks count per track
// #define TRACK_TICKS_LENGTH (TICK_LENGTH * MAX_TICKS_PER_TRACK)

struct TickData_Pointer {
    // POINTER_T address; // POINTER_T is uint16_t
    POINTER_T address[3];
    MEM_LENGTH_T length;
    // uint8_t byte4; // unused byte
};

const uint8_t DATA_LENGTH = sizeof(struct TickData_Pointer);

union TickData {
    struct TickData_Pointer pointer;
    uint8_t bytes[sizeof(struct TickData_Pointer)];
};

// 2 bytes time, 4 bytes address + len / data, 1 byte flag, 1 byte byte8 unused byte
struct Tick {
    TICK_T time;
    union TickData data;
    uint8_t flag;
    uint8_t byte8; // unused byte
};

const uint8_t TICK_LENGTH = sizeof(struct Tick);
const uint8_t TRACK_TICKS_LENGTH = sizeof(struct Tick) * MAX_TICKS_PER_TRACK;

struct TrackData {
    NAME_T name;
    COUNT_SLOTS_T ticksUsed;
    struct Tick ticks[MAX_TICKS_PER_TRACK];
    uint32_t length;
    struct PseudoHeapInstance* heapInstance;
};

// just return a new track struct with defaul values
struct TrackData createTrack(NAME_T newName, TRACK_LENGTH_T newLength){
    struct TrackData newTrack;
    newTrack.name = newName;
    newTrack.length = newLength;
    newTrack.ticksUsed = 0;
    memset(((uint8_t*)newTrack.ticks), '\0', TRACK_TICKS_LENGTH);
    newTrack.heapInstance = NULL;
    return newTrack;
};

// find pos for adding
inline uint8_t findTickBetweenPos(struct TrackData* trackToAddTo, TICK_T newTime, COUNT_SLOTS_T* desiredPosition){
    while (trackToAddTo->ticks[*desiredPosition].time < newTime // i should probably do binary search here too
        && *desiredPosition < trackToAddTo->ticksUsed) (*desiredPosition)++;
    return 0;
};

// find pos for removing
inline uint8_t findTickExactPos(struct TrackData* trackDeleteFrom, TICK_T removeTime, COUNT_SLOTS_T* desiredPosition){
    while (trackDeleteFrom->ticks[*desiredPosition].time != removeTime){ // binary search this
        (*desiredPosition)++;
        if (*desiredPosition >= trackDeleteFrom->ticksUsed) return 1; // didnt find
    };
    return 0;
};

// lowest level of writing the tick to 
inline uint8_t justAddTick(struct TrackData* trackToAddTo, TICK_T newTime, COUNT_SLOTS_T desiredPosition, 
    union TickData* bytes, uint8_t flag){
    trackToAddTo->ticksUsed++;
    trackToAddTo->ticks[desiredPosition].time = newTime;
    trackToAddTo->ticks[desiredPosition].flag = flag;
    // trackToAddTo->ticks[desiredPosition].byte8;
    memcpy(trackToAddTo->ticks[desiredPosition].data.bytes, bytes, DATA_LENGTH);
    return 0;
};

// reusable add helper
inline uint8_t addTickToTrackByPos(struct TrackData* trackToAddTo, TICK_T newTime, COUNT_SLOTS_T desiredPosition,
    union TickData* bytes, uint8_t flag){
    COUNT_SLOTS_T movableTicksCount = trackToAddTo->ticksUsed - desiredPosition;
    // copy over the rest of the items
    if (movableTicksCount){ // if movableTicksCount is more than 0
        memmove(trackToAddTo->ticks+desiredPosition+1, trackToAddTo->ticks+desiredPosition, 
            TICK_LENGTH * movableTicksCount);
    }
    justAddTick(trackToAddTo, newTime, desiredPosition, bytes, flag);
    return 0;
};

// reusable remove helper
inline uint8_t remTickFromTrackByPos(struct TrackData* trackDeleteFrom, COUNT_SLOTS_T pos){
    COUNT_SLOTS_T movableTicksCount = trackDeleteFrom->ticksUsed - (pos+1);
    if (movableTicksCount){ // if movableTicksCount is more than 0
        memmove(&(trackDeleteFrom->ticks[pos]), &(trackDeleteFrom->ticks[pos+1]), 
            TICK_LENGTH * movableTicksCount);
    }
    trackDeleteFrom->ticksUsed--;
    return 0;
};

// add tick
uint8_t addTickToTrack(struct TrackData* trackToAddTo, TICK_T newTime,
    union TickData* bytes, uint8_t flag){
    switch (trackToAddTo->ticksUsed) {
        case 0: // empty 
            justAddTick(trackToAddTo, newTime, 0, bytes, flag);
            break;
        case MAX_TICKS_PER_TRACK: // full
            return 1;
            break; // probably dont need this...
        default: // THIS WORKS IN O(N) TIME, I CAN GET IT LOWER IF I USE BINARY SEARCH
            COUNT_SLOTS_T desiredPosition = 0;
            findTickBetweenPos(trackToAddTo, newTime, &desiredPosition);
            addTickToTrackByPos(trackToAddTo, newTime, desiredPosition, bytes, flag);
            break;
    } 
    return 0;
};

// delete tick but writing remaining ticks over it
uint8_t remTickFromTrack(struct TrackData* trackDeleteFrom, TICK_T removeTime){ 
    if (trackDeleteFrom->ticksUsed) {
        COUNT_SLOTS_T desiredPosition = 0;
        if (findTickExactPos(trackDeleteFrom, removeTime, &desiredPosition)) return 1;
        remTickFromTrackByPos(trackDeleteFrom, desiredPosition);
    } else {
        return 1; // track is already empty
    }
    return 0;
};

// change tick value but not time
uint8_t changeTickInTrack(struct TrackData* trackToEdit, TICK_T changeTime,
    union TickData* bytes, uint8_t flag) { 
    if (trackToEdit->ticksUsed) {
        COUNT_SLOTS_T desiredPosition = 0;
        if (findTickExactPos(trackToEdit, changeTime, &desiredPosition)) return 1;
        memcpy(trackToEdit->ticks[desiredPosition].data.bytes, bytes, DATA_LENGTH);
        trackToEdit->ticks[desiredPosition].flag = flag;
    } else {
        return 1; // track is empty
    }
    return 0;
};

// move tick, or rather delete it and add it back in the new time position
uint8_t moveTickInTrack(struct TrackData* trackToEdit, TICK_T oldTime, TICK_T newTime){ 
    if (trackToEdit->ticksUsed) {
        COUNT_SLOTS_T desiredPosition = 0;
        if (findTickExactPos(trackToEdit, oldTime, &desiredPosition)) return 1;
        union TickData tmpTickData;
        uint8_t flag = trackToEdit->ticks[desiredPosition].flag;
        memcpy(&tmpTickData, &trackToEdit->ticks[desiredPosition].data, DATA_LENGTH);
        remTickFromTrackByPos(trackToEdit, desiredPosition);
        addTickToTrack(trackToEdit, newTime, &tmpTickData, flag);
    } else {
        return 1; // track is empty
    }
    return 0;
};

// return 1 if ticks are out of order, zero out unused space
// this isnt needed technically, but the print looks nicer with 0s instead of junk
// and it should only return 1 if something is wrong 
uint8_t trackMemCleanup(struct TrackData* track){
    uint8_t i = 0;
    uint16_t prev = 0;
    uint8_t blank[sizeof(struct Tick)] = {0x00};
    for (;i<track->ticksUsed; i++) {
        if (track->ticks[i].time<prev) return 1; // return 1 if track is out of order
    }
    for (;i<MAX_TICKS_PER_TRACK; i++) {
        memcpy(&(track->ticks[i]), blank, sizeof(struct Tick)); // zero out empty space
    }
    return 0;
};

// use printMemSpace() from PseudoHeap to print the ticks 
uint8_t printTrack(struct TrackData* track){
    printf("name: %04X, ticksUsed: %d, length: %d, heapInstance: %p", 
        track->name, track->ticksUsed, track->length, track->heapInstance);
    printMemSpace(track->ticks, MAX_TICKS_PER_TRACK*TICK_LENGTH, TICK_LENGTH);
    return 0;
};