#pragma once

#include "_compat/Print.h"
#include "_compat/Stream.h"
#undef EOF

namespace packetio {

class PacketPrint : public Print {
public:
    /*!
        @brief End the current packet
        @return true if successful
    */
    virtual bool end() = 0;

    /*!
        @brief Abort the current packet, preferably invalidating it if possible
    */
    virtual void abort() = 0;
};

class PacketStream : public Stream {
public:

    //! End Of File. This is already the value used by arduino
    static const int EOF = -1;
    //! End Of Packet.
    static const int EOP = -2;

    /*!
        @brief Read a single byte from the current packet.
        @return The byte if available
                EOF if we have reached the end of the stream
                EOP if the current packet is complete
    */
    virtual int read() override = 0;

    /*!
        @brief Peek the next byte from the current packet.
        @return The byte if available
                EOF if we have reached the end of the stream
                EOP if the current packet is complete
    */
    virtual int peek() override = 0;

    /*!
        @return A lower bound on the number of bytes available. This count
                includes the EOP return value.
    */
    virtual int available() = 0;

    /*!
        @brief Advance to the next packet. If the current packet is not
               complete, then read() will return EOF until it is.
    */
    virtual void next() = 0;
};

}
