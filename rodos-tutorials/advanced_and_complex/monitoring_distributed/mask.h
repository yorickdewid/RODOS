

 /**** Generated with support_programms/maskgen ***/


const char* screen[] = {
"*** See support_programs/maskgen ****",
"",
"  Neg counter time [                ]",
"-------------------------------------------------------------------",
"  odd cnt [   ]  [       ]  even cnt [       ]   ven pre [        ]",
"",
"-------------------------------------------------------------------",
"Prim counter",
"           msg type  [          ]",
"           part 1    [          ]",
"           part 2    [          ]",
"",
"-----------------------------------------------------------------",
"           Odd / even cnt [     ] :  [      ] [  ]",
"",
"--------------------------------------------------------------",
"          unknown meesage counter [    ] Type [    ] ",
"",
"",
 0 };


#define CLEAR_MASK "\x1B[2J\x1B[1;1H"
#define INIT_MASK() PRINTF("%s", CLEAR_MASK); for(int i = 0; screen[i] != 0; i++) PRINTF("%s\n", screen[i]);

#define NEGT              "\x1B[3;21H"
#define OST  "\x1B[5;12H"
#define OCNT     "\x1B[5;19H"
#define ECNT     "\x1B[5;39H"
#define EPRE      "\x1B[5;59H"
#define PRIMTYPE    "\x1B[9;23H"
#define PRIM1       "\x1B[10;23H"
#define PRIM2       "\x1B[11;23H"
#define CNT    "\x1B[14;28H"
#define NAME    "\x1B[14;39H"
#define LB  "\x1B[14;48H"
#define ERR   "\x1B[17;36H"
#define ERRT  "\x1B[17;48H"
