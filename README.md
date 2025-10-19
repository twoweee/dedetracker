# To run:
Just compile and run main.c.

# Situation as of now
Work in progress, building out files one by one.

Everything is work in progress, even the structure and readme.

Currently working in the **main** branch solely because there is no *working* version, so there isn't really much to break as I'm building things out. Once there is an actual version I will add at least a dev branch. 

To do soon:
- ~~structure better,~~
- ~~write better commetns in PseudoHeapManager and a better readme,~~
- ~~create a test directory and test scripts,~~
- PseudoHeap needs reworking,
- figure out (and document) remaining design,
- introduce header files and split mem printing out of PseudoHeapManager - its not needed in release,
- ~~TrackData.c is tested, but PseudoHeapManager just kinda,and certainly not together for those hypothetical long MIDI messages~~.

# Parts list
| File | Description | Status |
| ----------- | ----------- | ----------- |
| [TrackData.c](#trackdata) | track note and tick data structure with tick array, provides adding, removing, changing | Done for now |
| [PseudoHeapManager.c](#PseudoHeapManager) | provides pseudo heap functionality in allocated stack space, intended for long MIDI messages | Done for now |
| the rest | work in progress | work in progress |


# Design
## Design on larger scale
Some design will go here.

## On each file 
### TrackData
Time points = "time", the entire row of data in that time point (including time) = "tick".

Time is stored across 2 bytes, yet to decide on format, it'll be a fixed-point decimal likely or integer of 100 microsecond chunks or something. 

### PseudoHeapManager
This will manage a simulated heap inside a stack mem space. We need this in case we want the MIDI message to be longer than 4 bytes. 