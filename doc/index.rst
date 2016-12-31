PacketIO
========

A C++ library distributed with PlatformIO_, for framing packets sent or received over an Arduino `Stream`, such as `Serial`.

The key feature of this library over other framing implementations it that it operates on streams. This means that if your application layer is able to produce or consume streams, you can push these streams right the way through your program. Put simply, this means you can send arbitrarily large packets, without having to worry about allocating buffers.

.. toctree::
   :maxdepth: 2

   basics
   protocols
   listening
   internals

.. default-domain:: cpp

.. _PlatformIO: https://platformio.org/