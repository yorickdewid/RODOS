#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
	int y = 2003;
	int m= 1;
	int d = 7;
	int h = 12;
	int min = 44;
	double sec =15.369216;
	double dy2k = daysSinceY2k (y,m,d,h,min , sec );
	// Tage seit J2000
	Vector3D posECI (10000 ,20000 ,30000); // in Metern
	Vector3D posECF = posECI . mRotate ( eciToECF ( dy2k ));

	Vector3D posGeod = ecfToGeodetic ( posECF );
	posECF = geodeticToECF ( posGeod );
    system("Pause");
    return 1;
}