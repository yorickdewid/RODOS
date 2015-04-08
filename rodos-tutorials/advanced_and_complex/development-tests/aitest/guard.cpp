// -*- Mode: C++ -*-

///
// @file
// @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
// Small test program to test __cxa_guard* functions.

#include <rodos.h>

struct GuardedData {
  int data;

  GuardedData():
    data (rand())  // some non-trivial initialisation so that gcc does not optimise initing away
  {}

public:
  static GuardedData *getInstance()
  {
      static GuardedData it;
      return &it;
  }
};


struct ThreadTest: public Thread {
  ThreadTest():
    Thread ("Test", 100)
  {}

  void init() {}

  void run() {
    while(1) {
      suspendCallerUntil(NOW() + 1000*MILLISECONDS);
      GuardedData *d= GuardedData::getInstance();
      xprintf ("%d\n", d->data);
    }
  }
};

static Application   application("Test", 4711);
static ThreadTest test1;
static ThreadTest test2;

