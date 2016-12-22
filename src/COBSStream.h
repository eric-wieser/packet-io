#pragma once

#include "PacketIO.h"

class COBSStream : public PacketStream
{
private:
    Stream& _base;

    uint8_t _counter;       // the count of the current chunk
    uint8_t _chunk_index;

    bool _packet_done;
    bool _packet_valid;
    bool _skip_to_end;

    // true if the chunk starting is not due to a zero
    bool _next_marker_is_dummy;

    bool _packet_can_end_here() {
        return !_next_marker_is_dummy && _chunk_index == _counter;
    }

    void _reset() {
        _skip_to_end = false;
        _packet_done = false;
        _packet_valid = false;
        _next_marker_is_dummy = true;

        _counter = _chunk_index = 0;
    }

    int _read() {
        int c = _base.read();
        if(c < 0) {
            return EOF;
        }
        else if(c == 0) {
            _packet_done = true;
            _packet_valid = _packet_can_end_here();
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
        else if(c == 0) {
            _packet_done = true;
            _packet_valid = _packet_can_end_here();
            return EOP;
        }
        else {
            return c;
        }
    }

    int _update_count() {
        while(_chunk_index == _counter) {
            // try and read the new counter
            int c = _read();
            if(c < 0) return c;

            _counter = c;

            // decide whether to start the chunk by emitting a zero
            _chunk_index = _next_marker_is_dummy ? 1 : 0;

            // and whether the next chunk should also emit a zero
            _next_marker_is_dummy = _counter == 0xFF;
        }
        return 0;
    }


public:
    COBSStream(Stream& p) : _base(p) { _reset(); }

    virtual int read() override
    {
        while(_skip_to_end) {
            int c = _base.read();
            if(c < 0) return -1;

            if(c == 0) _reset();
        }

        if(_packet_done) return EOP;


        // update the counter field if we don't have one
        int stat = _update_count();
        if(stat < 0) return stat;

        // if we're looking at the counter, we should emit zero
        if(_chunk_index == 0) {
            _chunk_index++;
            return 0;
        }
        // otherwise, try to read the next value from the stream
        int val = _read();
        if(val < 0) return val;

        _chunk_index++;
        return val;
    }

    virtual int peek() override {
        if(_packet_done) return EOP;

        // update the counter field if we don't have one
        int stat = _update_count();
        if(stat < 0) return stat;

        // if we're looking at the counter, we should emit zero
        if(_chunk_index == 0) {
            return 0;
        }
        // otherwise, try to read the next value from the stream
        return _peek();
    }

    virtual void flush() override {
        _base.flush();
    }

    virtual int available() override {
        if(_packet_done) return 0;

        int stat = _update_count();
        if(stat < 0) return 0;

        return (_counter - _chunk_index);
    }

    virtual bool next() {
        if(_packet_done)
            _reset();
        else
            _skip_to_end = true;
        return true;
    }
    virtual bool done() { return _packet_done; }
private:
    size_t write(uint8_t) { return 0; }
    size_t write(const uint8_t*, size_t) { return 0; }
};
