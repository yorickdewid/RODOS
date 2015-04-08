#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
Complex c (1 ,0);
Complex c_arr [4]={c,c,c,c};
FFT (c_arr ,4 ,0); //[4 ,0 ,0 ,0]

IFFT (c_arr ,4 ,0);


    system("Pause");
    return 1;
}