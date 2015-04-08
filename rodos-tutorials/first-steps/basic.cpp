#include "rodos.h"

static Application appHW("HelloWorld");

class HelloWorld : public Thread {

public:

  HelloWorld() : Thread("HelloWorld") { }

  void init() { 
    PRINTF("Printing Hello World"); 
  }

  void run(){
    PRINTF("Hello World!\n");
  }
};

HelloWorld helloworld;

