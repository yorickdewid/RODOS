// -*- Mode: C++ -*-

///
// @file
// @author Christian Schulte, Henrik Theiling (AbsInt Angewandte Informatik GmbH)
//
// Tests that the system time never runs backwards.

#include <rodos.h>
#include <hw_core.h>

static Application module01("Systime Test");

class TTest: public Thread {
public:
  TTest(const char *name):
    Thread(name)
  {}

  void run()
  {
    char const *n= getName();

    unsigned int cycles= 0;
    TTime then= NOW();
    while (true) {
      TTime now= NOW();
      if (then > now)
        hwUserPanic ("Timewarp", then - now);
      then= now;

      cycles++;
      if (cycles >= 200000) {
        cycles= 0;
        xprintf ("%s",n);
      }
    }
  }
};

static TTest t1("1");
static TTest t2("2");

