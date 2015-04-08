//********** You can test the generated code with this code semgnet *****

#include "stdio.h"
#include "unistd.h"

#include "mask.h"

#define PRINTF printf

main() {
   INIT_MASK();

   for(int i = 0; i < 200; i++) {
        printf(STATE "%d", i); // ****!!! be aware of this c++ speciality: "aaa" "bbb" 
        for(int j = 100; j < 130; j++) {
                printf(TIME "%d",  j);
                printf(EXAMP "%d", j*100);
                fflush(stdout);
                usleep(50000);
        }
    }
}

