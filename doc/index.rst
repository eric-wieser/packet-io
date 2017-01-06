PacketIO
========

PacketIO is a C++ library for framing packets sent or received over an Arduino Stream_, such as Serial_. It is distributed as a PlatformIO library.

The key feature of this library over other framing implementations, such as PacketSerial_, it that it operates on streams. This means that if your application layer is able to produce or consume streams, you can push these streams right the way through your program. Put simply, this means you can send arbitrarily large packets, without having to worry about allocating buffers.

The example below shows a contrived case when streams are needed - it duplicates whatever packets come in, but it works no matter how large the incoming packet is:

.. include:: ../examples/packetio_streaming/packetio_streaming.ino
   :code: cpp

Contents
--------

.. toctree::
   :maxdepth: 2

   basics
   protocols
   listening
   internals

.. default-domain:: cpp

.. _PlatformIO: https://platformio.org/
.. _Serial: https://www.arduino.cc/en/Reference/Serial
.. _Stream: https://www.arduino.cc/en/Reference/Stream
.. _PacketSerial: https://github.com/bakercp/PacketSerial