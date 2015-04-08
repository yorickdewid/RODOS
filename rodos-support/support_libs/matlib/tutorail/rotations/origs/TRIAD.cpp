#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {

	Vector3D v1 (1 ,1 ,1);
	Vector3D v2 (2 ,2 ,2);
	CoordinateFrame3D A (v1 ,v2 ); // Aufbau des Frames A
	Matrix3D V(A.x,A.y,A.z); // die matrix (v1 ,v2 ,v3)
	Vector3D w1 (0 ,1 ,4);
	Vector3D w1 ( -3 ,2 ,0);
	CoordinateFrame3D B (w1 ,w2 ); // Aufbau des Frames B
	Matrix3D W(B.x,B.y,B.z); // die Matrix (w1 ,w2 ,w3)
	Matrix3D R = W. mMult (V. transpose ()); // Matrix R
	Quaternion q = R. toQuaternion (); // Quaternion


    system("Pause");
    return 1;
}