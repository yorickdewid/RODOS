 /**** Generated with support_programs/maskgenerator/generator.cpp ***/

const char* screen[] = {
" *** Minimum console size: 49x11 ****",
"",
" ┌─────────────────────────────────────────────┐",
" │ Logging framework example with MaskAppender │",
" ├─────────────────────────────────────────────┤",
" │ Time since boot:                [      ] s  │",
" │ Battery voltage:                [      ] mV │",
" │ Battery current:                [      ] mA │",
" └─────────────────────────────────────────────┘",
"",
 0 };

#define CLEAR_MASK "\x1B[2J\x1B[1;1H"
#define INIT_MASK() PRINTF("%s", CLEAR_MASK); for(int i = 0; screen[i] != 0; i++) PRINTF("%s\n", screen[i]);
#define  START  "\x1B[1;1H"

#define  TIME   "\x1B[6;37H"
#define  BATV   "\x1B[7;37H"
#define  BATC   "\x1B[8;37H"
