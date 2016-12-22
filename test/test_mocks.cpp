#ifdef UNIT_TEST

#include <unity.h>
#include "mock_print.h"
#include "test_helpers.h"

void test_mock_write() {
    MockPrint<> mp;
    mp.write("01234");
    mp.write("56789");

    TEST_ASSERT_EQUAL_STREAM("0123456789", mp);
}

void test_failing_mock_write() {
    const size_t fail_on[] = {0, 3, 6, 7};
    FailingMockPrint<15> mp(fail_on);

    TEST_ASSERT_EQUAL(0, mp.write("01234"));
    TEST_ASSERT_EQUAL(0, mp.get_len());

    TEST_ASSERT_EQUAL(2, mp.write("1234"));
    TEST_ASSERT_EQUAL(1, mp.write("4"));
    TEST_ASSERT_EQUAL(1, mp.write("56789"));
    TEST_ASSERT_EQUAL(0, mp.write("789"));
    TEST_ASSERT_EQUAL(2, mp.write("89"));

    TEST_ASSERT_EQUAL_STREAM("124589", mp);
}

void test_failing_mock_write_fixed() {
    const size_t fail_on[] = {0, 3, 6, 7};
    FailingMockPrint<15> mp(fail_on);

    write_retry(mp, "0123456789", 10);
    TEST_ASSERT_EQUAL_STREAM("0123456789", mp);
}

void run_all_mocks() {
    RUN_TEST(test_mock_write);
    RUN_TEST(test_failing_mock_write);
    RUN_TEST(test_failing_mock_write_fixed);
}

#endif
