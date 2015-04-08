#include "rodos.h"


class printftester : public Thread {
public:

  void run() {
    float d1 = -3.1415926;
    double f1 = 2.72737475;
    long long cnt = 1;

    for(int i = 0; i < 132; i++) {
      //PRINTF("i = %d = '%4d' = '%04d', %09b\n", i, i, i, i);
      //PRINTF(" power or 2 = %lld\n", cnt);
      cnt = cnt << 1;

      PRINTF(" floats = %f  = %5.6f\n", d1, f1);
      d1 += 0.1;
      f1 += 0.2;
    }
  }
} printtester;












