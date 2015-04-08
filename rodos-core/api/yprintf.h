
#ifndef __YPRINTF__
#define __YPRINTF__

#include <stdarg.h>

#undef putchar


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * micro printf (subset)
 *
 * Autor: Unkown (long long time ago)
 * c++, %ll, %b: Extended: Sergio Montenegro, 2011
 * %f : Sergio Montenegro, 2012
 *    Warning: %3.9f means 3 postions, then . then 9 positions
 *
 */

extern "C" int putchar(int c);

class Yprintf {
public:
    va_list ap;
    virtual ~Yprintf() {va_end(ap);}
    virtual void yputc(char c) { putchar(c); } // define yours *****
    void vaprintf(const char *fmt);
};


class Ysprintf : public Yprintf {
public:
    char* destination;
    Ysprintf(char* dest) { destination = dest;}
    virtual void yputc(char c) { *destination = c; destination++; *destination = 0; }
};

/*******************/

extern void xprintf(const char *fmt, ...) __attribute__((__format__(__printf__,1,2)));
extern void xsprintf(char* dest, const char *fmt, ...);


#ifndef NO_RODOS_NAMESPACE
}
#endif



#endif
