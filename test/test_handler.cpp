#ifdef UNIT_TEST

#include "test_helpers.h"
#include "mock_print.h"
#include "cobs/Stream.h"
#include "cobs/Print.h"
#include "PacketListener.h"

using namespace packetio;

static MockABuffer<512> long_buf;

static void test_handler_basic() {
    // output
    MockPrint mp(long_buf);
    COBSPrint output(mp);

    //input
    MockStream ms(mp);
    COBSStream input(ms);

    PacketListener handler(input);

    size_t received = 0;
    size_t sent = 0;
    const size_t N = 2;

    const MockSBuffer messages[N] = {
        "Hello world",
        "Go\0dbye world"
    };

    auto onMessage = [&](uint8_t* data, size_t len) {
        const MockSBuffer expected = messages[received++];
        TEST_ASSERT_EQUAL_STREAM(expected, MockSBuffer(data, len));
    };
    handler.onMessage(&onMessage);
    handler.update();
    TEST_ASSERT_EQUAL_MESSAGE(sent, received, "Nothing ran");

    for(size_t i = 0; i < N; i++) {
        output.write(messages[i].get_buffer(), messages[i].size());
        handler.update();

        TEST_ASSERT_EQUAL_MESSAGE(sent, received, "Sent too soon");

        output.end();
        sent++;
        handler.update();
        TEST_ASSERT_EQUAL(sent, received);
    }

    (void) sent;
}

void run_all_handler() {
    RUN_TEST(test_handler_basic);
}

#endif
