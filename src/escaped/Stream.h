#pragma once

#include "packet_interfaces.h"

namespace packetio {

template<typename EscapeCodes>
class EscapedStream : public PacketStream
{
private:
    Stream& _base;

    bool _skip_to_end;
    bool _escape_pending;
    bool _packet_done;
    bool _packet_valid;

    void _reset() {
        _skip_to_end = false;
        _packet_done = false;
        _packet_valid = false;
        _escape_pending = false;
    }

    int _read() {
        int c = _base.read();
        if(c < 0) {
            return EOF;
        }
        else if(c == EscapeCodes::END) {
            _packet_done = true;
            _packet_valid = !_escape_pending;
            return EOP;
        }
        else {
            return c;
        }
    }

    int _peek() {
        int c = _base.peek();
        if(c < 0) {
            return EOF;
        }
        else if(c == EscapeCodes::END) {
            return EOP;
        }
        else {
            return c;
        }
    }

public:
    EscapedStream(Stream& p) : _base(p) { _reset(); }

    virtual int read() override
    {
        while(_skip_to_end) {
            int c = _base.read();
            if(c < 0) return -1;

            if(c == EscapeCodes::END) _reset();
        }

        if(_packet_done) return EOP;

        while(true) {
            int val = _read();
            if(val < 0) return val;

            if(_escape_pending) {
                if(val == EscapeCodes::ESC_ESC) {
                    val = EscapeCodes::ESC;
                }
                else if(val == EscapeCodes::ESC_END) {
                    val = EscapeCodes::END;
                }
                else {
                    // something went wrong, but we'll ignore the previous escape character
                }
                _escape_pending = false;
                return val;
            }
            else if(val == EscapeCodes::ESC) {
                _escape_pending = true;
            }
            else {
                return val;
            }
        }
    }

    virtual int peek() override {
        if(_packet_done) return EOP;

        while(true) {
            int val = _peek();
            if(val < 0) return val;

            if(_escape_pending) {
                if(val == EscapeCodes::ESC_ESC) {
                    val = EscapeCodes::ESC;
                }
                else if(val == EscapeCodes::ESC_END) {
                    val = EscapeCodes::END;
                }
                else {
                    // something went wrong, but we'll ignore the previous escape character
                }
                return val;
            }
            if(val == EscapeCodes::ESC) {
                _read(); // consume the escape character we peeked
                _escape_pending = true;
            }
        }
    }

    virtual void flush() override {
        _base.flush();
    }

    virtual int available() override {
        if(_packet_done) return 1;  // TODO: test

        // consume the next character if it is an escape
        peek();

        // lower bound on how many bytes could be available
        // the first byte cannot be an escape
        int n = _base.available();
        return (n + 1) / 2;
    }

    virtual bool next() {
        if(_packet_done)
            _reset();
        else
            _skip_to_end = true;
        return true;
    }
private:
    // this isn't readable and writable!
    size_t write(uint8_t) override { return 0; }
    size_t write(const uint8_t*, size_t) override { return 0; }
};

}