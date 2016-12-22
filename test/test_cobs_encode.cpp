#ifdef UNIT_TEST

#include "test_helpers.h"
#include "mock_print.h"
#include "COBSPrint.h"

void test_cobs_basic(void) {
    MockPrint<> mp;
    COBSPrint p(mp);

    size_t n = 4;
    TEST_ASSERT_EQUAL(n, p.write("1234"));
    TEST_ASSERT(p.end());

    TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
}

void test_cobs_basic0(void) {
    MockPrint<> mp;
    COBSPrint p(mp);

    size_t n = 5;
    TEST_ASSERT_EQUAL(n, p.write("12\x00""34", n));
    TEST_ASSERT(p.end());

    TEST_ASSERT_EQUAL_STREAM("\x03""12\x03""34\x00", mp);
}

void test_cobs_piecewise(void) {
    MockPrint<> mp;
    COBSPrint p(mp);

    p.write("123");
    p.write("4");
    p.end();

    TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
}

void test_cobs_piecewise0(void) {
    MockPrint<> mp;
    COBSPrint p(mp);

    TEST_ASSERT_EQUAL(3, p.write("12\x00", 3));
    TEST_ASSERT_EQUAL(2, p.write("34", 2));
    TEST_ASSERT(p.end());

    TEST_ASSERT_EQUAL_STREAM("\x03""12\x03""34\x00", mp);
}

void test_cobs_interrupted() {
    const size_t fail_on[] = {0, 3, 6, 7};
    FailingMockPrint<15> mp(fail_on);
    COBSPrint p(mp);

    write_retry(p, "1234", 4);
    while(!p.end());

    TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
}

void test_cobs_interrupted0() {
    const size_t fail_on[] = {0, 3, 6, 7};
    FailingMockPrint<15> mp(fail_on);
    COBSPrint p(mp);

    write_retry(p, "12\x00""34", 5);
    while(!p.end());

    TEST_ASSERT_EQUAL_STREAM("\x03""12\x03""34\x00", mp);
}

void test_cobs_interrupted_many() {
    for(size_t i = 0; i < 6; i++)
    for(size_t j = i; j < 6; j++)
    for(size_t k = j; k < 6; k++)
    {
        const size_t fail_on[] = {i, j, k};
        FailingMockPrint<15> mp(fail_on);
        COBSPrint p(mp);

        write_retry(p, "1234", 4);
        while(!p.end());

        TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
    }
}

void run_all_cobs_encode() {
    RUN_TEST(test_cobs_basic);
    RUN_TEST(test_cobs_basic0);
    RUN_TEST(test_cobs_piecewise);
    RUN_TEST(test_cobs_piecewise0);
    RUN_TEST(test_cobs_interrupted);
    RUN_TEST(test_cobs_interrupted0);
    RUN_TEST(test_cobs_interrupted_many);
}

#endif
