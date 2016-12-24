#ifdef UNIT_TEST

#include "test_helpers.h"
#include "mock_print.h"
#include "COBSStream.h"

static MockABuffer<20> short_buf;

static void test_basic_read() {
    // test reading two sequential packets
    MockPrint mp(short_buf);
    MockStream ms(mp);
    const char first[] = "1234";
    const char second[] = "5678";
    mp.write((uint8_t) 5);
    mp.write(first);
    mp.write((uint8_t) 0);
    mp.write((uint8_t) 5);
    mp.write(second);
    mp.write((uint8_t) 0);

    COBSStream s(ms);

    for(int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL(first[i], s.read());
    }
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    s.next();
    for(int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL(second[i], s.read());
    }
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    s.next();
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
}

static void test_piecewise_read() {
    // test reading a packet that is coming in piece by piece
    MockPrint mp(short_buf);
    MockStream ms(mp);
    COBSStream s(ms);

    mp.write((uint8_t) 5);
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write("12");
    TEST_ASSERT_EQUAL('1', s.read());
    TEST_ASSERT_EQUAL('2', s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write("34");
    TEST_ASSERT_EQUAL('3', s.read());
    TEST_ASSERT_EQUAL('4', s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write((uint8_t) 0);
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
}

static void test_piecewise_read0() {
    // test reading a packet that is coming in piece by piece
    MockPrint mp(short_buf);
    MockStream ms(mp);
    COBSStream s(ms);

    mp.write((uint8_t) 3);
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write("12");
    TEST_ASSERT_EQUAL('1', s.read());
    TEST_ASSERT_EQUAL('2', s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write((uint8_t) 3);
    TEST_ASSERT_EQUAL(0, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write("34");
    TEST_ASSERT_EQUAL('3', s.read());
    TEST_ASSERT_EQUAL('4', s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    mp.write((uint8_t) 0);
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
}

static void test_basic_read_peek() {
    // test peeking a packet as it is read
    MockPrint mp(short_buf);
    MockStream ms(short_buf);
    mp.write((uint8_t) 5);
    mp.write("1234");
    mp.write((uint8_t) 0);

    COBSStream s(ms);

    TEST_ASSERT_EQUAL('1', s.peek());
    TEST_ASSERT_EQUAL('1', s.read());
    TEST_ASSERT_EQUAL('2', s.peek());
    TEST_ASSERT_EQUAL('2', s.read());
    TEST_ASSERT_EQUAL('3', s.peek());
    TEST_ASSERT_EQUAL('3', s.read());
    TEST_ASSERT_EQUAL('4', s.peek());
    TEST_ASSERT_EQUAL('4', s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.peek());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.peek());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.peek());
    TEST_ASSERT_EQUAL(COBSStream::EOP, s.read());
}

static void test_confused_reader() {
    // test giving up on a packet before it is done
    MockPrint mp(short_buf);
    MockStream ms(mp);
    COBSStream s(ms);

    // send first packet
    mp.write((uint8_t) 5);
    mp.write("1234");
    mp.write((uint8_t) 0);

    // read only 3/4 of the packet
    TEST_ASSERT_EQUAL('1', s.read());
    TEST_ASSERT_EQUAL('2', s.read());
    TEST_ASSERT_EQUAL('3', s.read());
    TEST_ASSERT_EQUAL(1, s.available());
    s.next();
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
    TEST_ASSERT_EQUAL(0, s.available());

    // send another
    mp.write((uint8_t) 5);
    mp.write("5678");
    mp.write((uint8_t) 0);

    // read only 1/4
    TEST_ASSERT_EQUAL(4, s.available());
    TEST_ASSERT_EQUAL('5', s.read());
    s.next();
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());

    // send another
    mp.write((uint8_t) 5);
    mp.write("5678");
    mp.write((uint8_t) 0);

    // read none!
    s.next();
    TEST_ASSERT_EQUAL(COBSStream::EOF, s.read());
}

void run_all_cobs_decode() {
    RUN_TEST(test_basic_read);
    RUN_TEST(test_basic_read_peek);
    RUN_TEST(test_piecewise_read);
    RUN_TEST(test_piecewise_read0);
    RUN_TEST(test_confused_reader);
}

#endif
