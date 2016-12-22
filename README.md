PacketIO
========

A PlatformIO library for framing packets sent or received over an arduino `Stream`, such as `Serial`.

The framing methods are exposed through the `PacketPrint` and `PacketStream` interfaces.

 * `PacketPrint` - extends `Print`:
   * `bool end()` - end a packet, returning whether it was sucessfull
   * `void abort()` - try as best as possible to start a new packet
 * `PacketStream` - extends `Stream`:
   * `read()`, `peek()` - return `PacketStream::EOP` (-2) if the packet is complete (without touching the underlying stream), otherwise behaves as normal
   * `next()` - stop reading from this packet, and move on

Framing Protocols
-----------------

Implemented so far

* [x] COBS
* [ ] SLIPS
* [ ] COBS-R
* [ ] Length-prefixed

Examples
--------

Writing
~~~~~~~
```
// pick a protocol here - everything below doesn't need to change
COBSPrint cobs_out(Serial);

PacketPrint& out = cobs_out;

out.print("Hello ");
out.print("World");
out.end(); // mark the end of a packet

out.print("Goodbye ");
if(something_went_horribly_wrong) {
	out.abort(); // terminate this packet early and try to mark what was sent as bad
}
else {
	out.print("World");
	out.end();
}
```

Reading
~~~~~~~
```
// pick a protocol here - everything below doesn't need to change
COBSStream cobs_in(Serial);

PacketStream& in = cobs_in;

while(true) {
	char message[10];
	size_t n = 0;

	while(true) {
		// read until we get something
		int c = in.read();
		if(c == in::EOF) continue;

		// detect End Of Packet
		if(c == in::EOP) break;

		// save anything else
		message[n++] = c;
	}

	Serial.print("Got: ");
	Serial.write(message, n);
	Serial.println();

	// start reading the next message
	in.next();
}

```

Future work
-----------

Provide an interface to [`nanopb`](https://github.com/nanopb/nanopb), that converts `Print` into `pb_ostream_s` and `Stream` into `pb_istream_s`, to allow protobufs to be framed.
