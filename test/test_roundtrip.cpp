#ifdef UNIT_TEST

#include "test_helpers.h"
#include "mock_print.h"
#include "packet_interfaces.h"

static MockABuffer<1024> long_buf;
static MockABuffer<1024> long_buf2;


template<typename PrintT, typename StreamT>
struct round_trip_tests {

static void test_one(const MockBuffer& content) {
    // output
    MockPrint mp(long_buf);
    PrintT output(mp);

    //input
    MockStream ms(mp);
    StreamT input(ms);

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
    TEST_ASSERT_EQUAL(packetio::PacketStream::EOP, c);
    TEST_ASSERT_EQUAL_STREAM(content, result);
}
template<int N>
static void test_one(const char (&s)[N]) {
    test_one(MockSBuffer::of(s));
}

static void test_basic() { test_one("Hello world"); }
static void test_special() { test_one(
    "Hello\0World, this is a long\0\0\0message "
    "containing nulls, newlines, and backslashes \\\\ \\\n \\n"
); }
static void test_long() { test_one("Hello\0World\0" REP(2,5,5,"A")); }
static void test_long0() { test_one("Hello\0World\0" REP(2,5,5,"A") "\0"); }
};

#include "COBSStream.h"
#include "COBSPrint.h"
#include "EscapedStream.h"
#include "EscapedPrint.h"
#include "escaped_defs.h"

using namespace packetio;

// SLIPS is harder to write escape sequences for than BackslashEscapeCodes
typedef round_trip_tests<COBSPrint, COBSStream> roundtrip_COBS;
typedef round_trip_tests<EscapedPrint<BackslashEscapeCodes>, EscapedStream<BackslashEscapeCodes>> roundtrip_Escaped;

void run_all_cobs_roundtrip() {
    RUN_TEST(roundtrip_COBS::test_basic);
    RUN_TEST(roundtrip_COBS::test_special);
    RUN_TEST(roundtrip_COBS::test_long);
    RUN_TEST(roundtrip_COBS::test_long0);
    RUN_TEST(roundtrip_Escaped::test_basic);
    RUN_TEST(roundtrip_Escaped::test_special);
    RUN_TEST(roundtrip_Escaped::test_long);
    RUN_TEST(roundtrip_Escaped::test_long0);
}

#endif
