#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {

	Vector3D v(1 ,1 ,1) // Ausgangsvektor
	Matrix3D R // Einheitsmatrix
	Vector3D rotated = v. mRotate (R); // w = R*v

	Vector3D v(1 ,1 ,1) // Ausgangsvektor
	Quaternion q(1 ,0 ,0 ,0) // Identity
	Vector3D rotated = v. qRotate (q) // w = q*vq

	YPR y( M_PI /2, M_PI /4, M_PI /6);
	Quaternion q = y. toQuat ();
	Matrix3D R = y. toMatrix3D ();


	Vector3D v(1 ,1 ,1);
	Vector3D rotated = mRotate (R);
	Vector3D rotated = qRotate (q);

	Vector3D v(1 ,1 ,1);
	Vector3D u(1 ,0 ,0) // Drehachse
	double angle = M_PI /2 // Drehwinkel
	AngleAxis u_phi (u, angle );
	Vector3D rotated = v. vecRotate ( u_phi );

    system("Pause");
    return 1;
}