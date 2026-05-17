cmake_minimum_required(VERSION 3.27)

project(dedetracker, C)

set(MAX_TICKS_PER_TRACK 16 CACHE STRING "Max ticks")

# Define hard limits
set(MIN_MAX_TICKS_PER_TRACK 16)
set(MAX_MAX_TICKS_PER_TRACK 16383)

if(MAX_TICKS_PER_TRACK LESS MIN_MAX_TICKS_PER_TRACK)
    message(WARNING "MAX_TICKS_PER_TRACK too small, clamping to ${MIN_MAX_TICKS_PER_TRACK}")
    set(MAX_TICKS_PER_TRACK ${MIN_MAX_TICKS_PER_TRACK} CACHE STRING "Max ticks" FORCE)
elseif(MAX_TICKS_PER_TRACK GREATER MAX_MAX_TICKS_PER_TRACK)
    message(WARNING "MAX_TICKS_PER_TRACK too large, clamping to ${MAX_MAX_TICKS_PER_TRACK}")
    set(MAX_TICKS_PER_TRACK ${MAX_MAX_TICKS_PER_TRACK} CACHE STRING "Max ticks" FORCE)
endif()

add_executable(main 
    src/main.c
    src/PseudoHeapManager.c
    src/PseudoHeapPrint.c
    src/TrackData.c
)

add_executable(test 
    test/test.c
    src/PseudoHeapManager.c
    src/PseudoHeapPrint.c
    src/TrackData.c
)

target_compile_definitions(main PRIVATE MAX_TICKS_PER_TRACK=${MAX_TICKS_PER_TRACK})
target_compile_definitions(test PRIVATE MAX_TICKS_PER_TRACK=${MAX_TICKS_PER_TRACK})