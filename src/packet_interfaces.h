#pragma once

#include "_compat/Print.h"
#include "_compat/Stream.h"
#undef EOF

namespace packetio {

/*!
    @brief Augments arduino's Print to add knowledge of packets

    Acts like a new Print for each packet. To stop printing to the current
    packet, and send new prints to the next one, call :func:end

    This only concerns itself with the read parts of stream, and should not be
    used for writing.
*/
class PacketPrint : public Print {
public:
    /*!
        @brief  End the current packet
        @return true if successful
    */
    virtual bool end() = 0;

    /*!
        @brief Abort the current packet, preferably invalidating it if possible
    */
    virtual void abort() = 0;

    /*!
        @overload virtual size_t write(uint8_t val)
        @brief  Write a single byte to the current packet.
        @param  val the byte to encode
        @return The number of un-encoded bytes written - 1 on success, 0 on
                failure
    */
    // virtual size_t write(uint8_t val) override = 0;

    /*!
        @overload virtual size_t PacketPrint::write(const uint8_t* buffer, size_t size)
        @brief    Write a collection of bytes to the current packet. If the return
                  value is not equal to `size`, then the write can be safely
                  continued with `write(buffer + ret, size-ret)`
        @param    buffer  the bytes to encode
        @param    size    the number of bytes
        @return   The number of un-encoded bytes written
    */

};


/*!
    @brief Augments arduino's Stream to add knowledge of packets

    Acts like a new stream for each packet, but instead of returning EOF when
    the packet is over, it returns EOP. EOF is returned when there is nothing
    to read, but the packet is not complete.

    This only concerns itself with the read parts of stream, and should not be
    used for writing.
*/
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
