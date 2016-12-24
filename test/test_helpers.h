#pragma once

#include <unity.h>
#include "mock_print.h"

inline void DumpStream(const uint8_t* buf, size_t n) {
    for(size_t i = 0; i < n; i++) {
        UnityPrintNumberHex(buf[i], 2);
        if(i != n-1) UnityPrint(" ");
    }
}

inline void DumpStream(const MockBuffer& s) {
    DumpStream(s.get_buffer(), s.size());
}


inline void AssertEqualStream(const MockBuffer& expected,
                              const MockBuffer& actual,
                              const UNITY_LINE_TYPE lineNumber)
{
    size_t         le = expected.size();
    const uint8_t* be = expected.get_buffer();
    size_t         la = actual.size();
    const uint8_t* ba = actual.get_buffer();

    bool match = false;
    if(le == la) {
        match = true;
        for(size_t i = 0; i < le; i++)
            match &= (ba[i] == be[i]);
    }
    if(!match) {
        UNITY_OUTPUT_CHAR('\n');
        UnityPrint("  Expected: ");
        DumpStream(expected);
        UNITY_OUTPUT_CHAR('\n');
        UnityPrint("  Actual  : ");
        DumpStream(actual);
        UNITY_OUTPUT_CHAR('\n');
        UNITY_TEST_FAIL(lineNumber, "Streams do not match");
    }
}

template<int N>
void DumpStream(const char (&s)[N]) {
    DumpStream(MockSBuffer::of(s));
}

template<size_t N>
void AssertEqualStream(const char (&expected)[N],
                       const MockBuffer& actual,
                       const UNITY_LINE_TYPE lineNumber) {
    return AssertEqualStream(
        MockSBuffer::of(expected),
        actual,
        lineNumber
    );
}

#define TEST_ASSERT_EQUAL_STREAM(expected, actual) AssertEqualStream((expected), (actual), (__LINE__))

template<size_t N>
inline size_t write(Print &p, const char (&data)[N]) {
    return p.write(data, N-1);
}

template<size_t N>
inline void write_retry(Print &p, const char (&data)[N]) {
    size_t n = N - 1;
    const char* datap = data;
    while(n) {
        size_t amt = p.write(datap, n);
        n -= amt;
        datap += amt;
    }
}
