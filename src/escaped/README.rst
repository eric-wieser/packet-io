Escaping packet framer
======================

This packet framer uses a special character to indicate end-of-frame, and an
escape character to allow this to appear within a message. In the worst-case,
this causes the packet to be twice the data size.

The choice of these special characters is parameterizable through template
arguments. :file:`encoded/codes.h` contains some example choices of values, including
an implementation of SLIP_. To define your own, you can use code like the
following::

	#include <escaped/Print.h>
	#include <escaped/Stream.h>
	#include <escaped/codes.h>

	using namespace packetio;

	// end, escape, escaped end, escaped escape
	typedef EscapeCodes<'A','/','a','\\'> MyCodes;

	EscapedPrint<MyCodes> printer(Serial);
	EscapedStream<MyCodes> reader(Serial);


In this example, we use ``A`` to end a packet. So the packet ``ABCD/EFGH`` is
encoded to ``/aBCD/\EFGHA``. Here, the first `A` is replaced by the escape
sequence ``/a``, and the ``/`` is replaced with ``/\``. Finally, an ``A`` is
appended to end the packet.

.. _SLIP: https://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol
