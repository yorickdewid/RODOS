#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
	Matrix3D M,Id;
	Vector3D axis ;
	double angle ;
	M = Id. rotateX ( M_PI /4);
	Quaternion q = M. toQuaternion ();
	axis = q. getVec ()
	angle = q. getAngle ();

	Matrix3D Id , R_x ;
	double angle = M_PI /4;
	R_x = Id. rotateX ( angle ); // Beispielmatrix
	Vector3D u = R_x . getVec ();
	double rotAngle = R_x . getAngle ();

    system("Pause");
    return 1;
}