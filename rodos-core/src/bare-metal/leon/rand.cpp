/* -*- Mode: C++ -*- */

/**
* @file
* @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
*
* Copyright 2008-2010 DLR
*
* @brief Pseudo random number generator.
*
*/

#include "rodos.h"
#include "scheduler.h"
#include "hw_stdlib.h"
#include "hw_core.h"
#include "hw_trap.h"
#include "hw_specific.h"

RandomState RandomState::globalState;

void RandomState::setSeed (unsigned xx)
{
  x= xx;
  y= 362436069;
  z= 521288629;
  w= 88675123;
}

RandomState::RandomState ()
{
  setSeed (123456789);
}

RandomState::RandomState (unsigned seed)
{
  setSeed (seed);
}

int RandomState::getRandom ()
{
  // Xor-Shift random generator.  According to Wikipedia, passes diehard tests.

  unsigned old= hwEnterTrapMode(); // might use global data, so protect against interrupts

    unsigned t= x ^ (x << 11);
    x= y;
    y= z;
    z= w;
    w= w ^ (w >> 19) ^ t ^ (t >> 8);
    unsigned result= w;

  hwLeaveTrapMode(old);

  return result;
}

void srand (unsigned int seed)
{
  RandomState::getGlobalState()->setSeed (seed);
}

int rand(void)
{
  return RandomState::getGlobalState()->getRandom();
}

int rand_r(unsigned *seed)
{
  // Maximal period (2^32-1) Galois LFSR.
  //
  // We use one whose selector constant can be loaded with a single sethi(), i.e., a
  // ll tags are >= 12; there are so many to choose from that this optimisation is
  // easy.  We use a function with 12 tags, i.e., quite many feedback bits.
  //
  // This generator is worse than getRandom(), of course, but probably one of
  // the best generators you get with only a 32-bit state.  Plus: it's very simple.

  unsigned x= (*seed);
  x+= !x;              // 0 is illegal, due to its period of 0, so map 0 to 1.
  x= (x >> 1) ^ ((-(x & 1)) & 0xb88d3c00);
  (*seed)= x;
  return x;
}
