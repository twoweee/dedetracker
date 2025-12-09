#ifndef TRACKDATA_H
#define TRACKDATA_H

#define TICK_T uint16_t // time
#define DATA_LENGTH 4 // data and pointer length
#define TICK_DATA uint32_t // should fit DATA_LENGTH bytes
#define MAX_TICKS_PER_TRACK 16383 // maximum ticks count per track
#define TICK_COUNT_T uint32_t // count taken up slots inside the array, should fit MAX_TICKS_PER_TRACK
#define TRACK_LENGTH_T uint32_t // track length in 1/TICK_PER_MS ms
#define NAME_T uint32_t // type for name

// #define DATA_LENGTH (sizeof(POINTER_T) + sizeof(MEM_LENGTH_T)) // length of data section
// #define TICK_LENGTH (sizeof(TICK_T) + DATA_LENGTH) 
// #define MAX_TICKS_PER_TRACK 20 // maximum ticks count per track
// #define TRACK_TICKS_LENGTH (TICK_LENGTH * MAX_TICKS_PER_TRACK)
// struct TickData_Pointer {
//     // POINTER_T address; // POINTER_T is uint16_t
//     // POINTER_T address[4];
//     uint32_t length;
//     // uint8_t byte4; // unused byte
// };

// TICK_DATA { // redundant pointer for now
//     uint8_t pointer[DATA_LENGTH];
//     uint8_t bytes[DATA_LENGTH];
// };

// 2 bytes time, 4 bytes address + len / data, 1 byte flag, 1 byte byte8 unused byte
struct Tick {
    TICK_T time;
    uint8_t data[DATA_LENGTH];
    uint8_t flag;
    uint8_t byte8; // unused byte
};

const uint8_t TICK_LENGTH = sizeof(struct Tick);
const uint64_t TRACK_TICKS_LENGTH = sizeof(struct Tick) * MAX_TICKS_PER_TRACK;

struct TrackData {
    NAME_T name;
    TICK_COUNT_T ticksUsed;
    struct Tick ticks[MAX_TICKS_PER_TRACK];
    uint32_t length;
    struct PseudoHeapInstance* heapInstance;
};

struct TrackData createTrack(NAME_T newName, TRACK_LENGTH_T newLength);
uint8_t addTickToTrack(struct TrackData* trackToAddTo, TICK_T newTime,
    TICK_DATA* bytes, uint8_t flag);
uint8_t remTickFromTrack(struct TrackData* trackDeleteFrom, TICK_T removeTime);
uint8_t changeTickInTrack(struct TrackData* trackToEdit, TICK_T changeTime,
    TICK_DATA* bytes, uint8_t flag);
uint8_t moveTickInTrack(struct TrackData* trackToEdit, TICK_T oldTime, TICK_T newTime);
uint8_t trackMemCleanup(struct TrackData* track);
uint8_t printTrackNoHeader(void* memToPrint);
uint8_t printTrack(struct TrackData* track);

#endif //TRACKDATA_H