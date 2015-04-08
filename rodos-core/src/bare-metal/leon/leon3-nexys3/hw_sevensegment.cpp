/**
 * Author: Murat Goeksu <Murat.Goeksu@dlr.de>
 *
 * 7 segment display driver interface for Nexys3/Leon3.
 */

#include "hw_sevensegment.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

    enum SegmentBit
    {
        A = (1 << 0),
        B = (1 << 1),
        C = (1 << 2),
        D = (1 << 3),
        E = (1 << 4),
        F = (1 << 5),
        G = (1 << 6),
        H = (1 << 7)
    };

    static const uint8_t charTable[10+27] =
    {
            A | B | C | D | E | F,      // 0
                B | C,                  // 1
            A | B |     D | E |     G,  // 2
            A | B | C | D |         G,  // 3
                B | C |         F | G,  // 4
            A |     C | D |     F | G,  // 5
            A |     C | D | E | F | G,  // 6
            A | B | C,                  // 7
            A | B | C | D | E | F | G,  // 8
            A | B | C | D |     F | G,  // 9

            A | B | C |     E | F | G,  // A
                    C | D | E | F | G,  // B
                        D | E |     G,  // C
                B | C | D | E |     G,  // D
            A |         D | E | F | G,  // E
            A |             E | F | G,  // F
            A |     C | D | E | F,      // G
                B | C |     E | F | G,  // H
                            E | F,      // I
                B | C | D | E,          // J
                B | C |     E | F | G,  // K
                        D | E | F,      // L
            A |     C |     E,          // M
                    C |     E |     G,  // N
                    C | D | E |     G,  // O
            A | B |         E | F | G,  // P
            A | B | C |         F | G,  // Q
                            E |     G,  // R
            A |     C | D |     F | G,  // S
                        D | E | F | G,  // T
                B | C | D | E | F,      // U
                    C | D | E,          // V
                B |     D |     F,      // W
                B | C |     E | F | G,  // X
                B | C | D |     F | G,  // Y
            A | B |     D | E |     G,  // Z
                                    G,  // -
    };

    /**
     * Converts a character into the corresponding value of the
     * 7 segment display
     */
    uint8_t HW_SEVENSEGMENT::convCharToVal(char c)
    {
        uint8_t newVal = 0;
        if( (c >= '0') && (c <= '9') )
        {
            newVal = charTable[c-'0'];
        }
        else if( (c >= 'A') && (c <= 'Z') )
        {
            newVal = charTable[c-'7'];
        }
        else if( (c >= 'a') && (c <= 'z') )
        {
            newVal = charTable[c-'W'];
        }
        else if(c == '-')
        {
            newVal = charTable[26];
        }

        return newVal;
    }

    /**
     * Writes raw data into the given index of the 7 segment display
     */
    void HW_SEVENSEGMENT::writeRaw(char c, uint8_t index)
    {
        if( (index < 4) )
        {
            // Read current port value and set byte to be written to zero
            uint32_t oldVal = *HW_OCR(HW_OCR_7SEG_DISPLAY) & ~(0xFF << (index*8));
            // Write new value to port
            *HW_OCR(HW_OCR_7SEG_DISPLAY) = oldVal | (c << (index*8));
        }
    }

    /**
     * Writes raw data into the 7 segment display
     */
    void HW_SEVENSEGMENT::writeRaw(uint32_t value)
    {
        // Write value to port
        *HW_OCR(HW_OCR_7SEG_DISPLAY) = value;
    }

    /**
     * Writes 4 characters into the 7 segment display
     */
    void HW_SEVENSEGMENT::writeChars(char c1, char c2, char c3, char c4)
    {
        writeRaw( (convCharToVal(c1) << 24) | (convCharToVal(c2) << 16) |
                  (convCharToVal(c3) <<  8) | (convCharToVal(c4)) );
    }

    /**
     * Writes a string into the 7 segment display
     */
    void HW_SEVENSEGMENT::writeText(const char* s)
    {
        switch (strlen(s))
        {
            case 0:
                writeChars(' ', ' ', ' ', s[0]);
                break;
            case 1:
                writeChars(' ', ' ', ' ', s[0]);
                break;
            case 2:
                writeChars(' ', ' ', s[0], s[1]);
                break;
            case 3:
                writeChars(' ', s[0], s[1], s[2]);
                break;
            case 4:
                writeChars(s[0], s[1], s[2], s[3]);
                break;
            default:
                writeChars('-', '-', '-', '-');
                break;
        }
    }

    /**
     * Writes a hex value into the 7 segment display
     */
    void HW_SEVENSEGMENT::writeHex(uint16_t value)
    {
        uint32_t segments = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            segments |= ( charTable[value & 0x000f] << (i*8) );
            value >>= 4;
        }
        writeRaw(segments);
    }

    /**
     * Writes a dec value into the 7 segment display (can be slow!).
     */
    void HW_SEVENSEGMENT::writeDec(uint16_t value)
    {
        if(value <= 9999)
        {
            uint32_t segments = 0;
            for(uint8_t i=0; i<4; i++)
            {
                // Get the nth digit
                uint16_t tmp = value;
                while(i--)
                {
                    tmp /= 10;
                }
                segments |= ( charTable[tmp % 10] << (i*8) );
            }
            writeRaw(segments);
        }
        else
        {
            // Value too big
            writeChars('-', '-', '-', '-');
        }
    }

#ifndef NO_RODOS_NAMESPACE
}
#endif
