Consistent-Overhead Byte Stuffing
=================================

This is an implementation of `Consistent-Overhead Byte Stuffing`_.

This uses a null byte as an end of packet marker, and uses a clever technique
to encode null bytes within the packet with minimal overhead. See the link
above for more information.

.. _Consistent-Overhead Byte Stuffing: http://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
