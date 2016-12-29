#pragma once

#include "escaped/Stream.h"
#include "escaped/codes.h"

namespace packetio {
	typedef EscapedStream<SLIPEscapeCodes> SLIPStream;
}
