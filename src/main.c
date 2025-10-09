//#pragma once
#include <stdio.h>
#include <stdint.h>
#include "TrackData.c"

int incr(uint8_t* num){
    for (int i = 0; i<4; i++) num[i]++;
    return 0;
}

int main() {
    struct TrackData track1 = createTrack(1, 400);
    union TickData tickData1;
    uint8_t bytes1[4] = {0x20, 0x30, 0x40, 0x50};
    uint8_t bytes2[4] = {0x28, 0x38, 0x48, 0x58};
    memcpy(tickData1.bytes, bytes1, 4);
    int i = 0;
    for (; i < 8; i+=1) {
        addTickToTrack(&track1, i+((i*256+(16*16*16))), &tickData1, 0xF6);
        incr(tickData1.bytes);
    }
    // printTrack(&track1); // 00 10 20 ... 01 11 21 ... 02 12 22 ...
    moveTickInTrack(&track1, 4+((4*256+(16*16*16))), 6+((6*256+(16*16*16)+(16*16)))); // move tick 04 14 to tick 06 17
    // printTrack(&track1);
    remTickFromTrack(&track1, 2+((2*256+(16*16*16)))); // remove tick 02 12
    trackMemCleanup(&track1); // if cleanup ISNT ran, the memory will be considered free but NOT zeroed out
    // printTrack(&track1);
    // changeTickInTrack(&track1, 1+(1*256+(16*16*16)), bytes2);
    // i tested this with a full array too i prommy :3
    printTrack(&track1);
}
