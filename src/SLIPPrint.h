#pragma once

#include "EscapedPrint.h"
#include "escaped_defs.h"

namespace packetio {
	typedef EscapedPrint<SLIPEscapeCodes> SLIPPrint;
}
