#pragma once

#include "packet_interfaces.h"

namespace packetio {

template<typename EscapeCodes>
class EscapedPrint : public PacketPrint
{
private:
    Print& _base;          // underlying stream
    int _pending_escape = -1;

    // finishes sending an escaped character, if the stream reached capacity
    // before both SEC and the code were sent
    bool _send_pending_escape() {
        if(_pending_escape == -1) return true;

        bool ok = _base.write(_pending_escape);
        if(!ok) return false;

        _pending_escape = -1;
        return true;
    }

    // Returns true if we should continue and send the next character
    // Whatever happens, the current character is viewed as sent successfully
    bool _write(uint8_t val) {
        if (val == EscapeCodes::END)
        {
            bool ok = _base.write(EscapeCodes::ESC);
            if(!ok) return false;

            _pending_escape = EscapeCodes::ESC_END;
            ok = _send_pending_escape();
            if(!ok) return false;
        }
        else if (val == EscapeCodes::ESC)
        {
            bool ok = _base.write(EscapeCodes::ESC);
            if(!ok) return false;

            _pending_escape = EscapeCodes::ESC_ESC;
            ok = _send_pending_escape();
            if(!ok) return false;
        }
        else
        {
            return _base.write(val);
        }
        return true;
    }

public:
    EscapedPrint(Print& p) : _base(p) { }

    virtual size_t write(uint8_t val) override
    {
        // Finish any pending writes that failed last time
        bool ok = _send_pending_escape();
        if(!ok) return 0;

        return _write(val);
    }

    virtual size_t write(const uint8_t *buffer, size_t size) override
    {
        // Finish any pending writes that failed last time
        bool ok = _send_pending_escape();
        if(!ok) return 0;

        // write as many as possible
        size_t i = 0;
        for(; i < size; i++) {
            if(!_write(buffer[i])) break;
        }

        return i;
    }

    using Print::write;

    virtual bool end() override
    {
        // Finish any pending writes that failed last time
        bool ok = _send_pending_escape();
        if(!ok) return false;

        // send the terminator
        ok = _base.write((uint8_t) EscapeCodes::END);
        if(!ok) return false;

        return true;
    }

    virtual void abort() override {
        if(_pending_escape != -1) {
            // sending ESC END is illegal
            _base.write((uint8_t) EscapeCodes::ESC);
            _base.write((uint8_t) EscapeCodes::END);
        }
        else {
            // we already sent ESC
            _base.write((uint8_t) EscapeCodes::END);
        }
        reset();
    }

    virtual void reset() override {
        _pending_escape = -1;
    }
};

}
