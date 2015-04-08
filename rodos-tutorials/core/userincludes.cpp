#include "rodos.h"

void MAIN() {
    xprintf("to change this set the shell variable USERCONFIGDIR \n");
    xprintf(" (do not forget, a file called userconfig.h hast to be there... may be empty \n");
    xprintf("example:\n");
    xprintf("   export USERCONFIGDIR=-I/home/mylogin/tmp\n\n\n");
    xprintf("using userincludes %s \n", USER_CONFIG_TEXT);
    xprintf("using userincludes version %d \n", USER_CONFIG_VERSION);
}



