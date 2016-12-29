#pragma once

#include "_compat/Print.h"
#include "_compat/Stream.h"
#undef EOF

namespace packetio {

class PacketPrint : public Print {
public:
    // complete the current packet
    virtual bool end() = 0;

    // abort and flush the current packet, preferably invalidating it if possible
    virtual void abort() = 0;
};

class PacketStream : public Stream {
public:
    // move onto the next packet. read() will return eof until then
    virtual bool next() = 0;

    static const int EOF = -1;
    static const int EOP = -2;
};

}
