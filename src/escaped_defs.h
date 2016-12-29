#pragma once

namespace packetio {

struct SLIPEscapeCodes {
    static const uint8_t END     = 0xC0; // Frame End
    static const uint8_t ESC     = 0xDB; // Frame Escape
    static const uint8_t ESC_END = 0xDC; // Transposed Frame End
    static const uint8_t ESC_ESC = 0xDD; // Transposed Frame Escape
};

struct BackslashEscapeCodes {
    static const uint8_t END     = '\n'; // Frame End
    static const uint8_t ESC     = '\\'; // Frame Escape
    static const uint8_t ESC_END =  'n'; // Transposed Frame End
    static const uint8_t ESC_ESC = '\\'; // Transposed Frame Escape
};

}
