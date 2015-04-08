#include "rodos.h"
#include "matlib.h"
#include "math.h"

void MAIN(void) {

	Vector3D v(1 ,2 ,3);
	Vector3D w(0 , -1 , -4);
	double alpha ;
	Vector3D u;
	alpha = v. getAngle (w); // liefert winkel in rad
	u = v. cross (w). normalize ();
	Quaternion q(cos(alpha/2), u.scale(sin(alpha/2)));

	YPR Y(q);
        PRINTF(" Quaternion: " );
        q.print();


}
