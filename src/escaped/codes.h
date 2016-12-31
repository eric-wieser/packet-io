#pragma once

namespace packetio {
/**
 * @brief      A trait type for indicating which markers to use in
 *             an EscapedStream or EscapedPrint. This type should be used as the type argument
 *
 * @tparam     pEND      The byte indicating end of frame
 * @tparam     pESC      The byte indicating that the following byte is an escape code
 * @tparam     pESC_END  The escape code for encoding an in-data END value
 * @tparam     pESC_ESC  The escape code for encoding an in-data ESC value
 */
template<uint8_t pEND, uint8_t pESC, uint8_t pESC_END, uint8_t pESC_ESC>
struct EscapeCodes {
    static const uint8_t END     = pEND;     //! Frame End
    static const uint8_t ESC     = pESC;     //! Frame Escape
    static const uint8_t ESC_END = pESC_END; //! Transposed Frame End
    static const uint8_t ESC_ESC = pESC_ESC; //! Transposed Frame Escape
};

typedef EscapeCodes<0xC0,0xDB,0xDC,0xDD> SLIPEscapeCodes;
typedef EscapeCodes<'\n','\\','n','\\'> BackslashEscapeCodes;

}
