#include "../test/tTrackData.c"
#include "../test/tPseudoHeapManager.c"
#include "../third-party/test/unity/src/unity.c"


void setUp(void) {}

void tearDown(void) {}

int main() {
    // printf("BEGIN TEST TrackData.c\n");
    // UNITY_BEGIN();

    // run_all_tests_TrackData();

    // int testTrackDataResult = UNITY_END();

    printf("\nBEGIN TEST PseudoHeapManager.c with small sizes\n");
    UNITY_BEGIN();

    run_all_tests_PseudoHeapManager_small();

    int testPseudoHeapManagerResult = UNITY_END();

    // return testTrackDataResult + testPseudoHeapManagerResult;
    return testPseudoHeapManagerResult;
    // return testTrackDataResult;
}