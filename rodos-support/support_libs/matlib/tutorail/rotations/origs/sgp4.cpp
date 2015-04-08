#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {

	string * strptr ;
	string iss_2Line [] =
	{"1 25544 U 98067 A 03007.53073344 .00041100
	00000 -0 34010 -3 0 9019" ,
	"2 25544 51.6346 107.2569 0004536
	6.6073 353.5141 15.58656572 35940"};
	strptr = iss_2Line ;
	TwoLineElement tle( strptr );
	ComputationModel calc ( tle );
	int delta = 360;
	// 360 minutes = 6 hours between predictions
	double elapsedTime = 0;
	double pos_x ,pos_y ,pos_z ,v_x ,v_y ,v_z;
	for ( int i = 0; i < 5; i ++) {
	pos_x = calc . sgp4 ( elapsedTime , tle )[0];
	pos_y = calc . sgp4 ( elapsedTime , tle )[1];
	pos_z = calc . sgp4 ( elapsedTime , tle )[2];
	v_x = calc . sgp4 ( elapsedTime , tle )[3];
	v_y = calc . sgp4 ( elapsedTime , tle )[4];
	v_z = calc . sgp4 ( elapsedTime , tle )[5];
	elapsedTime += delta ;
	}

	int y = 2003;
	int m= 1;
	int d = 7;
	int h = 12;
	int min = 44;
	double sec =15.369216;
	double dy2k = daysSinceY2k (y,m,d,h,min , sec );

	Vector3D posECI (pos_x ,pos_y , pos_z );
	Vector3D posECF = posECI . mRotate ( eciToECF ( dy2k ));

	Vector3D posGeod = ecfToGeodetic ( posECF );




    system("Pause");
    return 1;
}