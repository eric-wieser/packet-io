#pragma once

#include "escaped/Print.h"
#include "escaped/codes.h"

namespace packetio {
	typedef EscapedPrint<SLIPEscapeCodes> SLIPPrint;
}
