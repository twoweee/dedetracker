cmake_minimum_required(VERSION 3.27)

project(dedetracker, C)

add_executable(main 
    src/main.c
    src/PseudoHeapManager.c
    src/PseudoHeapPrint.c
    src/TrackData.c
)

#add_executable(test 
#    test/test.c
#)