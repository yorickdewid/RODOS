/**
 * @file timepoints.cc
 * @date 2008/06/12 10:10
 * @author Lutz Dittrich
 *
 * Copyright 2008 DLR
 *
 * @brief test for class TimePoints
*/
#include "rodos.h"

static Application module01("TestTimePoints");

class TestThread : public Thread {

private:
  TimePoints tp;

public:
  TestThread(const char* name = "TestThread") : Thread(name) { } 

  void run() {
    int i=0;
    tp.add("run");
    tp.add("next entry");
    tp.printList();
    tp.printList();
    while(1) {
      if (i>20) {
        tp.printList();
        i=0;
      }
      i++;
      tp.add(" loop entry");
      suspendCallerUntil(NOW() + 100*MILLISECONDS);
    }
  }
};

TestThread t;
