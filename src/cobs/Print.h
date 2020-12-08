#pragma once

#include "packet_interfaces.h"

namespace packetio {

class COBSPrint : public PacketPrint
{
private:
    uint8_t  _buffer[255];   // buffer for storing incomplete chunks
    uint8_t& _counter;       // alias for the first element, the count
    uint8_t  _next_counter;  // counter for the next chunk. Usually 1.
    size_t   _written_upto;  // index of the first byte not written
    Print&   _base;          // underlying stream

    // The current chunk is complete. Send it if possible
    // Returns whether the entire buffer was sent
    bool _send_full_buffer() {
        size_t n_sent = _base.write(_buffer, _counter);
        if(n_sent < _counter) {
            _written_upto = n_sent;
            return false;
        }
        _counter = _next_counter;
        return true;
    }

    // Send any remaining buffer that was not succesfully sent by
    // _send_full_buffer(). Returns whether these leftovers were all sent.
    bool _send_leftover_buffer() {
        // nothing leftover
        if(_written_upto == SIZE_MAX) return true;

        // write whatever is left
        size_t n_left = _counter - _written_upto;
        size_t n_sent = _base.write(_buffer + _written_upto, n_left);
        if(n_sent < n_left) {
            // didn't manage to clear it out
            _written_upto += n_sent;
            return false;
        }

        // mark leftovers cleared out
        _written_upto = SIZE_MAX;
        _counter = _next_counter;
        return true;
    }

    // Returns true if we should continue and send the next character
    // Whatever happens, the current character is viewed as sent successfully
    bool _write(uint8_t val) {
        if (val == 0)
        {
            // send what we have so far, add this byte to the next chunk
            _next_counter = 1;
            bool ok = _send_full_buffer();
            if(!ok) return false;
        }
        else
        {
            // consume and add
            _buffer[_counter++] = val;

            if (_counter == 0xFF)
            {
                // send a full chunk, and insert a dummy byte
                _next_counter = 1;
                bool ok = _send_full_buffer();
                if(!ok) return false;
            }
        }
        return true;
    }

public:
    COBSPrint(Print& p) : _counter(_buffer[0]), _written_upto(SIZE_MAX), _base(p)
    {
        // start with nothing sent
        _counter = 1;
    }

    virtual size_t write(uint8_t val) override
    {
        // Finish any pending writes that failed last time
        bool ok = _send_leftover_buffer();
        if(!ok) return 0;

        // don't care whether we can send another, this always succeeds
        _write(val);
        return 1;
    }

    virtual size_t write(const uint8_t *buffer, size_t size) override
    {
        // Finish any pending writes that failed last time
        bool ok = _send_leftover_buffer();
        if(!ok) return 0;

        // write as many as possible
        size_t i = 0;
        while(i < size && _write(buffer[i++]));

        return i;
    }

    using Print::write;

    virtual bool end() override
    {
        // Finish any pending writes that failed last time
        bool ok = _send_leftover_buffer();
        if(!ok) return false;

        // Send the last buffer chunk. Do not start the next chunk.
        _next_counter = 0;
        ok = _send_full_buffer();
        if(!ok) return false;

        // send the null terminator
        ok = _base.write((uint8_t) 0);
        if(!ok) return false;

        // begin the next packet
        _counter = 1;
        return true;
    }

    virtual void abort() override {
        // mid-chunk - sending a 0 is illegal
        if(_written_upto != SIZE_MAX) {
            _base.write((uint8_t) 0);
        }

        // new-chunk - send the shortest illegal chunk
        else {
            _base.write((uint8_t) 2);
            _base.write((uint8_t) 0);
        }

        // reset leftover buffer
        _written_upto = SIZE_MAX;
        // begin the next packet
        _counter = 1;
    }
};

}
