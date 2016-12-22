#ifdef UNIT_TEST

#include <unity.h>

extern void run_all_mocks();
extern void run_all_cobs_encode();
extern void run_all_cobs_decode();

void run_all_tests() {
    UNITY_BEGIN();

    run_all_mocks();
    run_all_cobs_encode();
    run_all_cobs_decode();

    UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>
void setup() { run_all_tests(); }
void loop() {}
#else
int main(int argc, char **argv) {
    run_all_tests();
    return 0;
}
#endif

#endif
