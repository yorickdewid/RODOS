#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
	Matrix3D Id ,R_x ,R_y ,R_z;
	double alpha ,beta , gamma ;
	R_x = Id. rotateX ( gamma );
	R_y = Id. rotateY ( beta );
	R_z = Id. rotateZ ( alpha );

	Vector3D s(1 ,2 ,3);
	Vector3D rotated ;
	double alpha ,beta , gamma ;
	rotated = rotateX (s, gamma ); // fuer Rotation um x
	rotated = rotateY (s, beta ); // fuer Rotation um y
	rotated = rotateZ (s, alpha ); // fuer Rotation um z

	double yaw = M_PI /4;
	double pitch = M_PI /6;
	double roll = M_PI /12;
	YPR y(yaw ,pitch , roll )
	Matrix3D YPR = y. toMatrix3D ();

    system("Pause");
    return 1;
}