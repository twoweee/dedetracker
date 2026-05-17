# To run:
So far only tests are meningful, compile and run test/test.c.
`cmake -S . -B build `
`cmake --build build `
`-DMAX_TICKS_PER_TRACK` parameter to set max ticks per track

# Situation as of now
Tests and whatever I have in main works.
Work in progress, building out files one by one.

To do soon:
- rework PseudoHeap - length isn't tracked, 0x00 is seen as empty, a more robust solution needed,
- use PseudoHeap with TrackData.

To do next:
- timer - need to be able to get precise time, must keep in mind I may want it to be portable to bare-metal,
- sequencing - read MIDI messages from TrackData based on time, 
- MIDI connection - OS features here should be fine as it's *really* easy to output the same bytes over serial output later.

# Design
## Design on larger scale
Some design will go here.

## On each file 
### TrackData
Time points = "time", the entire row of data in that time point (including time) = "tick".

Time is stored across 2 bytes, yet to decide on format, it'll be a fixed-point decimal likely or integer of 100 microsecond resolution or something. 

### PseudoHeapManager
This will manage a simulated heap inside a stack mem space. We need this because:
- writing a dynamic memory allocator is fun,
- I want the longest possible MIDI message to be longer than 4 bytes, 
- I want the possibility to easily port this to bare-metal, where malloc() may not be available. For examle, an ATmega 328p, if the overall memory footprint doesn't get too big. 
