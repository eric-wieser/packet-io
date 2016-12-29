#ifdef UNIT_TEST

#include "test_helpers.h"
#include "mock_print.h"
#include "cobs/Print.h"

using namespace packetio;

static MockABuffer<20> short_buf;

static void test_cobs_basic(void) {
    MockPrint mp(short_buf);
    COBSPrint p(mp);

    size_t n = 4;
    TEST_ASSERT_EQUAL(n, p.write("1234"));
    TEST_ASSERT(p.end());

    TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
}

static void test_cobs_basic0(void) {
    MockPrint mp(short_buf);
    COBSPrint p(mp);

    TEST_ASSERT_EQUAL(5, write(p, "12\x00""34"));
    TEST_ASSERT(p.end());

    TEST_ASSERT_EQUAL_STREAM("\x03""12\x03""34\x00", mp);
}

static void test_cobs_piecewise(void) {
    MockPrint mp(short_buf);
    COBSPrint p(mp);

    p.write("123");
    p.write("4");
    p.end();

    TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
}

static void test_cobs_piecewise0(void) {
    MockPrint mp(short_buf);
    COBSPrint p(mp);

    TEST_ASSERT_EQUAL(3, write(p, "12\x00"));
    TEST_ASSERT_EQUAL(2, write(p, "34"));
    TEST_ASSERT(p.end());

    TEST_ASSERT_EQUAL_STREAM("\x03""12\x03""34\x00", mp);
}

static void test_cobs_interrupted() {
    const size_t fail_on[] = {0, 3, 6, 7};
    MockPrint mp(short_buf);
    FailingPrint fmp(mp, fail_on);
    COBSPrint p(fmp);

    write_retry(p, "1234");
    while(!p.end());

    TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
}

static void test_cobs_interrupted0() {
    const size_t fail_on[] = {0, 3, 6, 7};
    MockPrint mp(short_buf);
    FailingPrint fmp(mp, fail_on);
    COBSPrint p(fmp);

    write_retry(p, "12\x00""34");
    while(!p.end());

    TEST_ASSERT_EQUAL_STREAM("\x03""12\x03""34\x00", mp);
}

static void test_cobs_interrupted_many() {
    for(size_t i = 0; i < 6; i++)
    for(size_t j = i; j < 6; j++)
    for(size_t k = j; k < 6; k++)
    {
        const size_t fail_on[] = {i, j, k};
        MockPrint mp(short_buf);
        FailingPrint fmp(mp, fail_on);
        COBSPrint p(fmp);

        write_retry(p, "1234");
        while(!p.end());

        TEST_ASSERT_EQUAL_STREAM("\x05""1234\x00", mp);
    }
}

static void test_cobs_long() {
    MockABuffer<512> long_buf;
    MockABuffer<512> long_buf2;

    const MockSBuffer content = "ABCDE";
    size_t total = 300;

    MockPrint expected(long_buf2);
    {
        expected.write((uint8_t) 255);
        size_t at = 0;
        for(; at < 254; at++)
            expected.write(content[at % content.size()]);
        expected.write((uint8_t) (total - 254 + 1));
        for(; at < total; at++)
            expected.write(content[at % content.size()]);
        expected.write((uint8_t) 0);
    }
    MockPrint mp(long_buf);
    COBSPrint p(mp);

    size_t at = 0;
    for(; at < total; at++)
        p.write(content[at % content.size()]);
    p.end();

    TEST_ASSERT_EQUAL_STREAM(expected, mp);

}

void run_all_cobs_encode() {
    RUN_TEST(test_cobs_basic);
    RUN_TEST(test_cobs_basic0);
    RUN_TEST(test_cobs_piecewise);
    RUN_TEST(test_cobs_piecewise0);
    RUN_TEST(test_cobs_interrupted);
    RUN_TEST(test_cobs_interrupted0);
    RUN_TEST(test_cobs_interrupted_many);
    RUN_TEST(test_cobs_long);
}

#endif
