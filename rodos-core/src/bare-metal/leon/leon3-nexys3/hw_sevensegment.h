/**
 * Author: Murat Goeksu <Murat.Goeksu@dlr.de>
 *
 * 7 segment display driver interface for Nexys3/Leon3.
 */

#include "rodos.h"
#include "string_pico.h"
#include "hw_defs.h"
#include "hw_core.h"

#ifndef RODOS_NEXYS3_HW_SEVEN_SEGMENT_H
#define RODOS_NEXYS3_HW_SEVEN_SEGMENT_H

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class HW_SEVENSEGMENT
{

public:
    void writeRaw(char c, uint8_t index);
    void writeRaw(uint32_t value);
    void writeChars(char c1, char c2, char c3, char c4);
    void writeText(const char* s);
    void writeHex(uint16_t value);
    void writeDec(uint16_t value);

private:
    uint8_t convCharToVal(char c);

};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif // RODOS_NEXYS3_HW_SEVEN_SEGMENT_H
