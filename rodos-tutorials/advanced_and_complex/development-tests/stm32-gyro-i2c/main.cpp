

#include "pre-rodos.h"


class Prime {
public:
      int x;
      Prime( int xp) {
		xprintf("\n\nDas ist vor main\n\n");
	        x = xp;
	}
};

static Prime prime1(123);
static Prime prime2(234);
static Prime prime3(345);


/*
 *	main
 */
int main(void) {

	hw_init();

	xprintf("\n\n VERSION (4.0) Init constructus %d %d %d  sm 1.0 \n", prime1.x, prime2.x, prime3.x);
	//xprintf("\n\nlong logn %lld %lld %lld  sm 1.0 \n", cntll1, cntll2, cntll3);
	//printf("\n\n fp  %f, %f\n", 3.1415, 2.78);
	xprintf("\n\n\tGyro V1.93s\n");

	// printf("Hello worln\n");
	xprintf("\nDas war der normale printf\n");

	long long llcnt = 0;
	long long timeNow = 0;
	for(llcnt = 0; llcnt < 20; llcnt++) {
		xprintf(" llcnt = %ld  100, 200, 300 = %d %d %d\n", llcnt, 100, 200, 300);
		timeNow = getNanoseconds();
		xprintf(" time = %lld \n", timeNow);
	}

	while(1) {
		nsSleep(1000000000);
		timeNow = getNanoseconds();
		xprintf(" time = %lld \n", timeNow);
	}

}
