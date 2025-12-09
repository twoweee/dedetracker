#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "TrackData.h"
#include "PseudoHeapManager.h"

// find pos for adding
static inline uint8_t findTickBetweenPos(struct TrackData* trackToAddTo, TICK_T newTime, TICK_COUNT_T* desiredPosition){
    while (trackToAddTo->ticks[*desiredPosition].time < newTime // i should probably do binary search here too
        && *desiredPosition < trackToAddTo->ticksUsed) (*desiredPosition)++;
    return 0;
};

// find pos for removing
static inline uint8_t findTickExactPos(struct TrackData* trackDeleteFrom, TICK_T removeTime, TICK_COUNT_T* desiredPosition){
    while (trackDeleteFrom->ticks[*desiredPosition].time != removeTime){ // binary search this
        (*desiredPosition)++;
        if (*desiredPosition >= trackDeleteFrom->ticksUsed) return 1; // didnt find
    };
    return 0;
};

// lowest level of writing the tick to 
static inline uint8_t justAddTick(struct TrackData* trackToAddTo, TICK_T newTime, TICK_COUNT_T desiredPosition, 
    TICK_DATA* bytes, uint8_t flag){
    trackToAddTo->ticksUsed++;
    trackToAddTo->ticks[desiredPosition].time = newTime;
    trackToAddTo->ticks[desiredPosition].flag = flag;
    // trackToAddTo->ticks[desiredPosition].byte8;
    memcpy(trackToAddTo->ticks[desiredPosition].data, bytes, DATA_LENGTH);
    return 0;
};

// reusable add helper
static inline uint8_t addTickToTrackByPos(struct TrackData* trackToAddTo, TICK_T newTime, TICK_COUNT_T desiredPosition,
    TICK_DATA* bytes, uint8_t flag){
    TICK_COUNT_T movableTicksCount = trackToAddTo->ticksUsed - desiredPosition;
    // copy over the rest of the items
    if (movableTicksCount){ // if movableTicksCount is more than 0
        memmove(trackToAddTo->ticks+desiredPosition+1, trackToAddTo->ticks+desiredPosition, 
            TICK_LENGTH * movableTicksCount);
    }
    justAddTick(trackToAddTo, newTime, desiredPosition, bytes, flag);
    return 0;
};

// reusable remove helper
static inline uint8_t remTickFromTrackByPos(struct TrackData* trackDeleteFrom, TICK_COUNT_T pos){
    TICK_COUNT_T movableTicksCount = trackDeleteFrom->ticksUsed - (pos+1);
    if (movableTicksCount){ // if movableTicksCount is more than 0
        memmove(&(trackDeleteFrom->ticks[pos]), &(trackDeleteFrom->ticks[pos+1]), 
            TICK_LENGTH * movableTicksCount);
    }
    trackDeleteFrom->ticksUsed--;
    return 0;
};

// add tick
uint8_t addTickToTrack(struct TrackData* trackToAddTo, TICK_T newTime,
    TICK_DATA* bytes, uint8_t flag){
    switch (trackToAddTo->ticksUsed) {
        case 0: // empty 
            justAddTick(trackToAddTo, newTime, 0, bytes, flag);
            break;
        case MAX_TICKS_PER_TRACK: // full
            return 1;
            break; // probably dont need this...
        default: // THIS WORKS IN O(N) TIME, I CAN GET IT LOWER IF I USE BINARY SEARCH
            TICK_COUNT_T desiredPosition = 0;
            findTickBetweenPos(trackToAddTo, newTime, &desiredPosition);
            addTickToTrackByPos(trackToAddTo, newTime, desiredPosition, bytes, flag);
            break;
    } 
    return 0;
};

// delete tick but writing remaining ticks over it
uint8_t remTickFromTrack(struct TrackData* trackDeleteFrom, TICK_T removeTime){ 
    if (trackDeleteFrom->ticksUsed) {
        TICK_COUNT_T desiredPosition = 0;
        if (findTickExactPos(trackDeleteFrom, removeTime, &desiredPosition)) return 1;
        remTickFromTrackByPos(trackDeleteFrom, desiredPosition);
    } else {
        return 1; // track is already empty
    }
    return 0;
};

// change tick value but not time
uint8_t changeTickInTrack(struct TrackData* trackToEdit, TICK_T changeTime,
    TICK_DATA* bytes, uint8_t flag) { 
    if (trackToEdit->ticksUsed) {
        TICK_COUNT_T desiredPosition = 0;
        if (findTickExactPos(trackToEdit, changeTime, &desiredPosition)) return 1;
        memcpy(trackToEdit->ticks[desiredPosition].data, bytes, DATA_LENGTH);
        trackToEdit->ticks[desiredPosition].flag = flag;
    } else {
        return 1; // track is empty
    }
    return 0;
};

// move tick, or rather delete it and add it back in the new time position
uint8_t moveTickInTrack(struct TrackData* trackToEdit, TICK_T oldTime, TICK_T newTime){ 
    if (trackToEdit->ticksUsed) {
        TICK_COUNT_T desiredPosition = 0;
        if (findTickExactPos(trackToEdit, oldTime, &desiredPosition)) return 1;
        TICK_DATA tmpTickData;
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
    TICK_COUNT_T i = 0;
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
uint8_t printTrackNoHeader(void* memToPrint){
    printMemSpace(memToPrint, MAX_TICKS_PER_TRACK*TICK_LENGTH, TICK_LENGTH);
    return 0;
};

// use printMemSpace() from PseudoHeap to print the ticks 
uint8_t printTrack(struct TrackData* track){
    printf("name: %04X, ticksUsed: %d, length: %d, heapInstance: %p", 
        track->name, track->ticksUsed, track->length, track->heapInstance);
    printf("\n\t  tick \t   |data\t       |flag|byte");
    printTrackNoHeader(&(track->ticks));
    return 0;
};