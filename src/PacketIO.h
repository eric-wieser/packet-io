#pragma once

#include "Print.h"

class PacketPrint : public Print {
public:
    // complete the current packet
    virtual bool end() = 0;

    // abort and flush the current packet, preferably invalidating it if possible
    virtual void abort() = 0;
};
