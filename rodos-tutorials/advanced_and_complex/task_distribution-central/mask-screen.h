

 /**** Generated with support_programms/maskgen ***/


const char* screen[] = {
"*** See support_programs/maskgen ****",
"",
"",
"Node Nr [  ]        T0   T1   T2   T3   T4   T5   T6   T7   T8   T9   ",
"                   -------------------------------------------------",
"Distribution Talbe [  ] [  ] [  ] [  ] [  ] [  ] [  ] [  ] [  ] [  ] ",
"                   -------------------------------------------------",
"",
"TaskId  TaksNr : Context",
"-------------------------",
"",
"negCnt   [  ]  : [       ]",
"oddcnt   [  ]  : [       ]",
"evenCnt  [  ]  : [       ]",
"PrimCnt  [  ]  : [       ]",
"-------  [  ]  : [       ]",
"-------  [  ]  : [       ]",
"-------  [  ]  : [       ]",
"",
"Sendig report:",
"      node [  ] sane [ ] cpudLoad [ ]  timeNow [         ]",
"",
"",
 0 };


#define CLEAR_MASK "\x1B[2J\x1B[1;1H"
#define INIT_MASK() PRINTF("%s", CLEAR_MASK); for(int i = 0; screen[i] != 0; i++) PRINTF("%s\n", screen[i]);

