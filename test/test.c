#include "../test/tTrackData.c"
#include "../third-party/test/unity/src/unity.c"


void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    run_all_tests_TrackData();

    return UNITY_END();
}