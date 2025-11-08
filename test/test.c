#include "../test/tTrackData.c"
#include "../test/tPseudoHeapManagerSmall.c"
#include "../test/tPseudoHeapManagerLong.c"
#include "../third-party/test/unity/src/unity.c"


void setUp(void) {}

void tearDown(void) {}

int main() {
    printf("BEGIN TEST TrackData.c\n");
    UNITY_BEGIN();

    run_all_tests_TrackData();

    int testTrackDataResult = UNITY_END();

    printf("\nBEGIN TEST PseudoHeapManagerShort.c\n");
    UNITY_BEGIN();

    run_all_tests_PseudoHeapManagerShort();

    int testPseudoHeapManagerShortResult = UNITY_END();

    printf("\nBEGIN TEST PseudoHeapManagerLong.c\n");
    UNITY_BEGIN();

    run_all_tests_PseudoHeapManagerLong();

    int testPseudoHeapManagerLongResult = UNITY_END();

    return testTrackDataResult + testPseudoHeapManagerShortResult + testPseudoHeapManagerLongResult;
}