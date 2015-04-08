#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"

void MAIN(void) {

	Vector3D v(1 ,2 ,3);
	Vector3D w(0 , -1 , -4);
	double alpha ;
	Vector3D u;
	alpha = v. getAngle (w); // liefert winkel in rad
	u = v. cross (w). normalize ();
	AngleAxis u_phi(u,alpha);
	Quaternion q(u_phi);

	YPR Y(q);
        Y.print();
}
