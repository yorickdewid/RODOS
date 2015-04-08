#include <stdarg.h>
#include "rodos.h"
#include "monitorprintf.h"
#include "scheduler.h"

/* semaphore for control printing from different thread */
static Semaphore reportfProtector;

/* initialise monitoringMsg */
MonitoringMsg MonitorPrintf::monitoringMsg(MonitorPrintf::MONITORING_ID);

void MonitorPrintf::printfToMonitorMsg(const char *fmt, va_list ap) {
    unsigned char c;

    while ( (c = *fmt++) != 0 ) {
        unsigned char is_signed = 0;
        unsigned char is_long = 0;
        unsigned char is_longlong = 0;
        char *ptr;

        if ( c != '%' ) {
            continue;
        }
        c = *fmt++;


        if ( c == '0' ) {
            c = *fmt++;
        }
        if ( c >= '0' && c <= '9') {
            c = *fmt++;
        }

        if ( c == 'l' ) {
            is_long = 1;
            c = *fmt++;
        }
        if ( c == 'l' ) {
            is_longlong = 1;
            c = *fmt++;
        }

        switch ( c ) {
        case 'c':
            c = va_arg(ap, int);	// char promoted to int
	    monitoringMsg.serialize((int)c);
            /* no break */

        default:
            continue;

        case 's':
            ptr = va_arg(ap, char *);
	    monitoringMsg.serializeString(ptr);
            continue;

        case 'o':
            break;

        case 'd':
            is_signed = 1;
            /* no break */

        case 'u':
            break;

        case 'x':
            break;

        case 'b':
            break;
        }

        // Number output
        {
            {
                long long s_val = 0;

                if ( is_longlong ) {
                    s_val = va_arg(ap, long long);
                    monitoringMsg.serialize((long long)s_val);
                } else if ( is_long ) {
                    s_val = va_arg(ap, long);
                    monitoringMsg.serialize((long)s_val);
                } else {
                    s_val = va_arg(ap, int);
                    monitoringMsg.serialize((int)s_val);
                }


            }
        } // number outputs
    } // for all chars in format (fmt)
}


void MonitorPrintf::printfFromMonitorMsg(const char *fmt, MonitoringMsg* report) {
    unsigned char c;

    while ( (c = *fmt++) != 0 ) {
        unsigned char is_signed = 0;
        unsigned char is_long = 0;
        unsigned char is_longlong = 0;
        unsigned short base;
        unsigned char *ptr;

        if ( c != '%' ) {
            yputc(c);
            continue;
        }
        c = *fmt++;

        unsigned char width;
        unsigned char fill;

        width = 0;
        fill = ' ';
        if ( c == '0' ) {
            fill = c;
            c = *fmt++;
        }
        if ( c >= '0' && c <= '9') {
            width = c - '0';
            c = *fmt++;
        }

        if ( c == 'l' ) {
            is_long = 1;
            c = *fmt++;
        }
        if ( c == 'l' ) {
            is_longlong = 1;
            c = *fmt++;
        }

        switch ( c ) {
        case 'c':
            c = report->deserializeInt();
            /* no break */

        default:
            yputc(c);
            continue;

        case 's':
            ptr = (unsigned char*) report->deserializeString();
            while ( (c = *ptr++) ) {
                yputc(c);
            }
            continue;

        case 'o':
            base = 8;
            yputc('0');
            break;

        case 'd':
            is_signed = 1;
            /* no break */

        case 'u':
            base = 10;
            break;

        case 'x':
            base = 16;
            break;

        case 'b':
            base = 2;
            break;
        }

        // Number output
        {
            unsigned long long u_val;

            {
                long long s_val = 0;

                if ( is_longlong ) {
                    s_val = report->deserializeLonglong();
                } else if ( is_long ) {
                    s_val = report->deserializeLong();
                } else {
                    s_val = report->deserializeInt();
                }

                if ( is_signed ) {
                    if ( s_val < 0 ) {
                        s_val = -s_val;		// make it positiv
                        yputc('-');
                        if ( width ) {
                            --width;
                        }
                    }
                }
                u_val = (unsigned long long)s_val;
            }

            {
                unsigned char scratch[26];

                ptr = scratch + sizeof(scratch);
                *--ptr = 0;
                do {
                    char ch = (u_val % base) + '0';

                    if (ch > '9') {
                        ch += 'A' - '9' - 1;
                    }

                    *--ptr = ch;
                    u_val /= base;

                    if ( width ) {
                        --width;
                    }
                } while ( u_val );

                // insert padding chars
                while ( width-- ) {
                    *--ptr = fill;
                }

                // output result
                while ( (c = *ptr++) ) {
                    yputc(c);
                }
            }
        } // number outputs
    } // for all chars in format (fmt)
}



void reportf(const char* fmt, ...) {

	va_list ap;
    va_start(ap, fmt);

    if (!Scheduler::isSchedulerRunning()) {
    	MonitorPrintf::monitoringMsg.serialize((long)fmt); // first fild: a pointer fo the format which shall be used by monitor printer
    	MonitorPrintf::printfToMonitorMsg(fmt, ap);
		MonitorPrintf::monitoringMsg.report(MonitorPrintf::PRINTFMSG);
    } else {
        reportfProtector.enter();
        MonitorPrintf::monitoringMsg.serialize((long)fmt); // first fild: a pointer fo the format which shall be used by monitor printer
        MonitorPrintf::printfToMonitorMsg(fmt, ap);
        MonitorPrintf::monitoringMsg.report(MonitorPrintf::PRINTFMSG);
        reportfProtector.leave();
    }

    va_end(ap);
}
