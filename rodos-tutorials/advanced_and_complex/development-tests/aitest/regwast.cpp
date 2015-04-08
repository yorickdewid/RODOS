// -*- Mode: C++ -*-

///
// @file
// @author Henrik Theiling (AbsInt Angewandte Informatik GmbH)
//
// Test program to constantly set and check registers in order
// to test the task switching: no register value must ever
// change, obviously.
//
// This uses a lot of assembler to do the job and thus only
// works on SPARC.  It is a very low-level stress test.
//
// To be done: test floating point flags.

#include <math.h>
#include <rodos.h>
#include "hw_core.h"
#include "hw_trap.h"

#define REG_CNT   66
#define REC_DEPTH 40

#define REG_ICC   64 // one register to test that the flags don't change
#define REG_Y     65

#define STORE_AND_CLEAR(_reg) \
    "\n\tld [ %0 + 4 * " #_reg " ], %%r" #_reg \
    "\n\tst %%g0, [ %0 + 4 * " #_reg " ]"

#define LOAD_BACK(_reg) \
    "\n\tst %%r" #_reg ", [ %0 + 4 * " #_reg "]"

#define STORE_AND_CLEAR_F(_reg) \
    "\n\tld [ %0 + 4 * (32 + " #_reg ")], %%f" #_reg \
    "\n\tst %%g0, [ %0 + 4 * (32 + " #_reg ")]" \

#define LOAD_BACK_F(_reg) \
    "\n\tst %%f" #_reg ", [ %0 + 4 * (32 + " #_reg " )]"


#define NOP0   "nop"
#define NOP1   NOP0  "; "   NOP0
#define NOP2   NOP1  "; "   NOP1
#define NOP3   NOP2  "; "   NOP2
#define NOP4   NOP3  "; "   NOP3
#define NOP5   NOP4  "\n\t" NOP4
#define NOP6   NOP5  "\n\t" NOP5
#define NOP7   NOP6  "\n\t" NOP6
#define NOP8   NOP7  "\n\t" NOP7
#define NOP9   NOP8  "\n\t" NOP8
#define NOP10  NOP9  "\n\t" NOP9

#define FLAG_C  1
#define FLAG_V  2
#define FLAG_Z  4
#define FLAG_N  8

#define STRINGIFY_AUX2(X) #X
#define STRINGIFY_AUX1(X) STRINGIFY_AUX2(X)
#define STRINGIFY(X)      STRINGIFY_AUX1(X)

// Array of two number that, when added with addcc, produce the icc value of
// the first index of the array at that position.
// This is because we cannot directly set the ICC in user mode, so we need
// to do it indirectly.
// Not all combinations are possible with an addcc.
#define OP_NONE 0
#define OP_TADD 1
#define OP_TSUB 2

struct ComputeForICC {
  unsigned op;
  unsigned arg1;
  unsigned arg2;
};
static ComputeForICC computeForICC[16]= {

  /* 0x0 ---- */ { OP_TADD, 0x00000000, 0x20000000 },
  /* 0x1 ---C */ { OP_TADD, 0x40000000, 0xe0000000 },
  /* 0x2 --V- */ { OP_TADD, 0x00000000, 0x00000001 },
  /* 0x3 --VC */ { OP_TADD, 0x20000000, 0xe0000001 },
  /* 0x4 -Z-- */ { OP_TADD, 0x00000000, 0x00000000 },
  /* 0x5 -Z-C */ { OP_TADD, 0x20000000, 0xe0000000 },
  /* 0x6 -ZV- */ { OP_TSUB, 0x1fffffff, 0x1fffffff },
  /* 0x7 -ZVC */ { OP_TADD, 0x80000000, 0x80000000 },
  /* 0x8 N--- */ { OP_TADD, 0x00000000, 0x80000000 },
  /* 0x9 N--C */ { OP_TADD, 0xa0000000, 0xe0000000 },
  /* 0xa N-V- */ { OP_TADD, 0x00000000, 0x80000001 },
  /* 0xb N-VC */ { OP_TADD, 0xa0000000, 0xe0000001 },
  /* 0xc NZ-- */ { OP_NONE, 0x00000000, 0x00000000 },
  /* 0xd NZ-C */ { OP_NONE, 0x00000000, 0x00000000 },
  /* 0xe NZV- */ { OP_NONE, 0x00000000, 0x00000000 },
  /* 0xf NZVC */ { OP_NONE, 0x00000000, 0x00000000 },

  // NOTE:
  // N=Z=1 is an impossible combination, and user space cannot produce this.

};

// Simple version of the test:
extern inline void storeCallLoad (int cnt, unsigned reg[REG_CNT])
{
  register unsigned _arg1 __asm__("l1")= computeForICC[reg[REG_ICC]].arg1;
  register unsigned _arg2 __asm__("l2")= computeForICC[reg[REG_ICC]].arg2;
  register unsigned _op   __asm__("l3")= computeForICC[reg[REG_ICC]].op;
  register int      _cnt  __asm__("o0")= cnt;
  register void *   _reg  __asm__("l0")= reg;

  __asm__ __volatile__ (
    // STEP 0: set PSR.ICC indirectly by using a compute:
    "\n\tsubcc  %4, "STRINGIFY(OP_TADD)", %%g0"
    "\n\tbe,a   1f"
    "\n\ttaddcc %2, %3, %%g0"  // then

    "\n\ttsubcc %2, %3, %%g0"  // else

 "\n1:"
    "\n\tst     %%g0, [ %0 + 4 * "STRINGIFY(REG_ICC)" ]" // clear entry in table

    // STEP 1: First store all registers we are allowed to use:

    // y
    "\n\tld     [ %0 + 4 * "STRINGIFY(REG_Y)" ], %%g1"
    "\n\tmov    %%g1, %%y"
    "\n\tst     %%g0, [ %0 + 4 * "STRINGIFY(REG_Y)" ]"

    // r0..r7 = g0..g7
    STORE_AND_CLEAR( 0)    // Always 0!  We'll test it anyway.
    STORE_AND_CLEAR( 1)
    STORE_AND_CLEAR( 2)
    STORE_AND_CLEAR( 3)
    STORE_AND_CLEAR( 4)
    STORE_AND_CLEAR( 5)
    STORE_AND_CLEAR( 6)
    STORE_AND_CLEAR( 7)

    // r8..r15 = o0..o7
//    STORE_AND_CLEAR( 8)  // param 0
    STORE_AND_CLEAR( 9)
    STORE_AND_CLEAR(10)
    STORE_AND_CLEAR(11)
    STORE_AND_CLEAR(12)
    STORE_AND_CLEAR(13)
//     STORE_AND_CLEAR(14)  // sp
    STORE_AND_CLEAR(15)  // return address (maked as clobbered, so we can check it)

    // r16..r23 = l0..l7
//     STORE_AND_CLEAR(16) // param 1
    STORE_AND_CLEAR(17)
    STORE_AND_CLEAR(18)
    STORE_AND_CLEAR(19)
    STORE_AND_CLEAR(20)
    STORE_AND_CLEAR(21)
    STORE_AND_CLEAR(22)
    STORE_AND_CLEAR(23)

    // r24..r31 = i0..i7
    STORE_AND_CLEAR(24)
    STORE_AND_CLEAR(25)
    STORE_AND_CLEAR(26)
    STORE_AND_CLEAR(27)
    STORE_AND_CLEAR(28)
    STORE_AND_CLEAR(29)
//    STORE_AND_CLEAR(30)  // fp
//    STORE_AND_CLEAR(31)  // our return address

    // f0..f31
    STORE_AND_CLEAR_F( 0)
    STORE_AND_CLEAR_F( 1)
    STORE_AND_CLEAR_F( 2)
    STORE_AND_CLEAR_F( 3)
    STORE_AND_CLEAR_F( 4)
    STORE_AND_CLEAR_F( 5)
    STORE_AND_CLEAR_F( 6)
    STORE_AND_CLEAR_F( 7)
    STORE_AND_CLEAR_F( 8)
    STORE_AND_CLEAR_F( 9)
    STORE_AND_CLEAR_F(10)
    STORE_AND_CLEAR_F(11)
    STORE_AND_CLEAR_F(12)
    STORE_AND_CLEAR_F(13)
    STORE_AND_CLEAR_F(14)
    STORE_AND_CLEAR_F(15)
    STORE_AND_CLEAR_F(16)
    STORE_AND_CLEAR_F(17)
    STORE_AND_CLEAR_F(18)
    STORE_AND_CLEAR_F(19)
    STORE_AND_CLEAR_F(20)
    STORE_AND_CLEAR_F(21)
    STORE_AND_CLEAR_F(22)
    STORE_AND_CLEAR_F(23)
    STORE_AND_CLEAR_F(24)
    STORE_AND_CLEAR_F(25)
    STORE_AND_CLEAR_F(26)
    STORE_AND_CLEAR_F(27)
    STORE_AND_CLEAR_F(28)
    STORE_AND_CLEAR_F(29)
    STORE_AND_CLEAR_F(30)
    STORE_AND_CLEAR_F(31)

    // STEP 2: do a couple of nops to give the system a chance to switch task
    "\n\t" NOP10

    // STEP 3: load back all registers that might be righteously clobbered by the user trap
    // o0..o7:
    LOAD_BACK ( 8)
    LOAD_BACK ( 9)
    LOAD_BACK (10)
    LOAD_BACK (11)
    LOAD_BACK (12)
    LOAD_BACK (13)
    LOAD_BACK (14)
    LOAD_BACK (15)

    // g0..g7:
    LOAD_BACK ( 0)
    LOAD_BACK ( 1)
    LOAD_BACK ( 2)
    LOAD_BACK ( 3)
    LOAD_BACK ( 4)
    LOAD_BACK ( 5)
    LOAD_BACK ( 6)
    LOAD_BACK ( 7)

    // f0..31:
    LOAD_BACK_F( 0)
    LOAD_BACK_F( 1)
    LOAD_BACK_F( 2)
    LOAD_BACK_F( 3)
    LOAD_BACK_F( 4)
    LOAD_BACK_F( 5)
    LOAD_BACK_F( 6)
    LOAD_BACK_F( 7)
    LOAD_BACK_F( 8)
    LOAD_BACK_F( 9)
    LOAD_BACK_F(10)
    LOAD_BACK_F(11)
    LOAD_BACK_F(12)
    LOAD_BACK_F(13)
    LOAD_BACK_F(14)
    LOAD_BACK_F(15)
    LOAD_BACK_F(16)
    LOAD_BACK_F(17)
    LOAD_BACK_F(18)
    LOAD_BACK_F(19)
    LOAD_BACK_F(20)
    LOAD_BACK_F(21)
    LOAD_BACK_F(22)
    LOAD_BACK_F(23)
    LOAD_BACK_F(24)
    LOAD_BACK_F(25)
    LOAD_BACK_F(26)
    LOAD_BACK_F(27)
    LOAD_BACK_F(28)
    LOAD_BACK_F(29)
    LOAD_BACK_F(30)
    LOAD_BACK_F(31)

    // ICC:
    "\n\tmov     %%g0, %%g1"

    "\n\tbcs,a   . + 8"
    "\n\tor      %%g1, "STRINGIFY(FLAG_C) ", %%g1"

    "\n\tbvs,a   . + 8"
    "\n\tor      %%g1, "STRINGIFY(FLAG_V) ", %%g1"

    "\n\tbz,a    . + 8"
    "\n\tor      %%g1, "STRINGIFY(FLAG_Z) ", %%g1"

    "\n\tbneg,a  . + 8"
    "\n\tor      %%g1, "STRINGIFY(FLAG_N) ", %%g1"

    "\n\tst      %%g1, [ %0 + 4 * "STRINGIFY(REG_ICC)" ]"

    // Y
    "\n\tmov    %%y, %%g1"
    "\n\tst     %%g1, [ %0 + 4 * "STRINGIFY(REG_Y)" ]"


    // STEP 4: maybe call

    "\n\tcall regWasteRecurse ; nop"   // param cnt is already in place

    // STEP 5: load back all registers that may not clobbered by the call

    // g0..g7:
    LOAD_BACK (16)
    LOAD_BACK (17)
    LOAD_BACK (18)
    LOAD_BACK (19)
    LOAD_BACK (20)
    LOAD_BACK (21)
    LOAD_BACK (22)
    LOAD_BACK (23)

    // o0..o7:
    LOAD_BACK (24)
    LOAD_BACK (25)
    LOAD_BACK (26)
    LOAD_BACK (27)
    LOAD_BACK (28)
    LOAD_BACK (29)
    LOAD_BACK (30)
    LOAD_BACK (31)

    : /* no output regs */
    : "r" (_reg), "r" (_cnt), "r" (_arg1), "r" (_arg2), "r" (_op)
    : /* long clobber list */
            "g1", "g2", "g3", "g4", "g5", "g6", "g7",
            "o1", "o2", "o3", "o4", "o5",       "o7",
                              "l4", "l5", "l6", "l7",
      "i0", "i1", "i2", "i3", "i4", "i5"
  );
}

extern "C" {

  static __attribute__((__noinline__))
  void regWasteRecurse (int cnt)
  {
    if (cnt > 0) {
      unsigned src[REG_CNT];
      for (int i=0; i < REG_CNT; i++)
        src[i]= rand();

      // find a valid value for ICC:
      for(;;) {
        src[REG_ICC]= rand() & 0xf;
        if (computeForICC[src[REG_ICC]].op != OP_NONE)
          break;
      }

      unsigned dst[REG_CNT];
      memcpy (dst, src, sizeof(src));

      storeCallLoad (cnt-1, dst);

      src[0]= 0; // r0 must always read as 0
      for (int i=0; i < REG_CNT; i++) {
        if (! (i ==  8 ||  // param 0
               i == 14 ||  // sp
               i == 16 ||  // param 1
               i == 30 ||  // fp
               i == 31))   // return address
        {
          if (src[i] != dst[i])
            hwUserPanic("register value changed",
                i |
                ((src[i] & 0xfff) <<  8) |
                ((dst[i] & 0xfff) << 20));
        }
      }
    }
  }

}

struct ThreadRecurse: public Thread {
  char const *id;

  ThreadRecurse(char const *_id):
    Thread (
      "Recursion Test",
      100,
      (
        REC_DEPTH *
        (
          2 * (REG_CNT * 4) +  // 2 times the space for the registers (src and dst arrays)
          32 +                 // some more local variables
          96                   // standard minimum frame size
        )
      ) +
      10240                  // some more space for trap handlers
    ),
    id (_id)
  {}

  void init() {}

  void run(){
    int i= 0;
    while(1) {
      if (i == 0)
          puts(id);
      i= (i+1) & 0x3f;
      regWasteRecurse(1 + (((unsigned)rand()) % REC_DEPTH));
    }
  }

};

static Application   application("UART Send Test", 4711);
static ThreadRecurse test1 ("1");
static ThreadRecurse test2 ("2");
static ThreadRecurse test3 ("3");
static ThreadRecurse test4 ("4");
