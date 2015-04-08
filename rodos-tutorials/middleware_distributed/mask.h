

/**** Generated with support_programms/maskgen ***/


#define CLEAR_MASK "\x1B[2J\x1B[1;1H"
#define INIT_MASK() PRINTF("%s", CLEAR_MASK); for(int i = 0; screen[i] != 0; i++) PRINTF("%s\n", screen[i]);

#define S11     "\x1B[9;12H"
#define R11     "\x1B[9;28H"
#define S12     "\x1B[10;12H"
#define R12     "\x1B[10;28H"
#define S13     "\x1B[11;12H"
#define R13     "\x1B[11;28H"
#define S14     "\x1B[12;12H"
#define R14     "\x1B[12;28H"
#define S15     "\x1B[13;12H"
#define R15     "\x1B[13;28H"
#define S16     "\x1B[14;12H"
#define R16     "\x1B[14;28H"
#define R16S      "\x1B[14;37H"
#define MSGCNT  "\x1B[18;36H"
