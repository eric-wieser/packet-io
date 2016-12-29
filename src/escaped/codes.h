#pragma once

namespace packetio {

template<uint8_t pEND, uint8_t pESC, uint8_t pESC_END, uint8_t pESC_ESC>
struct EscapeCodes {
    static const uint8_t END     = pEND; // Frame End
    static const uint8_t ESC     = pESC; // Frame Escape
    static const uint8_t ESC_END = pESC_END; // Transposed Frame End
    static const uint8_t ESC_ESC = pESC_ESC; // Transposed Frame Escape
};

typedef EscapeCodes<0xC0,0xDB,0xDC,0xDD> SLIPEscapeCodes;
typedef EscapeCodes<'\n','\\','n','\\'> BackslashEscapeCodes;

}
