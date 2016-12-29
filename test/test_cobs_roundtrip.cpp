#ifdef UNIT_TEST

#include "test_helpers.h"
#include "mock_print.h"
#include "COBSStream.h"
#include "COBSPrint.h"

static MockABuffer<1024> long_buf;
static MockABuffer<1024> long_buf2;

static void test_one(const MockBuffer& content) {
    // output
    MockPrint mp(long_buf);
    COBSPrint output(mp);

    //input
    MockStream ms(mp);
    COBSStream input(ms);

    // send a packet
    size_t n = output.write(content.get_buffer(), content.size());
    TEST_ASSERT_EQUAL(n, content.size());
    TEST_ASSERT(output.end());

    // read it back
    MockPrint result(long_buf2);
    int c;
    while(c = input.read(), c >= 0) {
        result.write(c);
    }

    // verify that we spotted the packet ended, and the message was preserved
    TEST_ASSERT_EQUAL(COBSStream::EOP, c);
    TEST_ASSERT_EQUAL_STREAM(result, content);
}
template<int N>
void test_one(const char (&s)[N]) {
    test_one(MockSBuffer::of(s));
}

static void test_basic() { test_one("Hello world"); }
static void test_nulls() { test_one("Hello\0World, this is a long\0\0\0message containing nulls"); }
static void test_long() { test_one("Hello\0World\0" REP(2,5,5,"A")); }
static void test_long0() { test_one("Hello\0World\0" REP(2,5,5,"A") "\0"); }

void run_all_cobs_roundtrip() {
    RUN_TEST(test_basic);
    RUN_TEST(test_nulls);
    RUN_TEST(test_long);
    RUN_TEST(test_long0);
}

#endif
