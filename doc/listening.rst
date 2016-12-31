Listening for packets
=====================

.. doxygenclass:: packetio::PacketListener_
    :members:
    :undoc-members:

Example usage:

.. code-block:: cpp

    #include <packet_interface.h>
    #include <PacketListener.h>
    #include <cobs/Stream.h>
    using namespace packetio;

    void setup () {
        COBSStream cobs_serial_in(Serial);
        PacketListener handler(cobs_serial_in);

        int message_count = 0;
        auto message_handler = [&](const uint8_t* buffer, size_t len) {
            message_count++;
        };
        handler.onMessage(&message_handler);

        while(true) {
            handler.update();
            Serial.print("Message recieved: ");
            Serial.print(message_count);
            Serial.println();
        }
    }
