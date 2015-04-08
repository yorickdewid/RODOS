
 /**** Generated with support_programms/maskgen ***/

const char* screen[] = {
"Node Nr [  ]",
"My Load [  ]",
"                      Rot X     From",
"                      [       ] [  ]",
"                         |",
"                         |",
"                         |",
"                         |",
"                         |                   Rot Y     From",
"                         +-------------------[       ] [  ]",
"                        /",
"                       /",
"                      /",
"              Rot Z  /    From",
"              [       ]  [  ]",
"",
"",
"            0    1    2    3    4    5    6    7    8    9",
"Node Load [  ] [  ] [  ] [  ] [  ] [  ] [  ] [  ] [  ] [  ] ",
"",
"",
 0 };


#define CLEAR_MASK "\x1B[2J\x1B[1;1H"
#define INIT_MASK() PRINTF("%s", CLEAR_MASK); for(int i = 0; screen[i] != 0; i++) PRINTF("%s\n", screen[i]);

