#pragma once

#include "EscapedStream.h"
#include "escaped_defs.h"

namespace packetio {
	typedef EscapedStream<SLIPEscapeCodes> SLIPStream;
}
