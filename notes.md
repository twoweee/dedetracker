cd src
gcc Dedetracker.c TrackData.c PseudoHeapPrint.c PseudoHeapManager.c -o track_exe

gcc src/TrackData.c src/PseudoHeapPrint.c src/PseudoHeapManager.c test/test.c -o test_exe

main compiles, tests work, need to set up CMakeLists
then rearrange tests to make them make a bit more sense