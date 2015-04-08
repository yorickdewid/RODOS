#include "rodos.h"

class HelloWorld : public Thread {
  void run(){ 
    PRINTF("Hello "); 
    PRINTF("World!\n"); 
  }
};

HelloWorld helloWorld[10];

