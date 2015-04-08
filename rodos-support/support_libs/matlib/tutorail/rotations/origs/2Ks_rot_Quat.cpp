#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
	Vector3D v1 (1 ,0 ,0);
	Vector3D v2 (0 ,1 ,0);
	Vector3D v3 (0 ,0 ,1);
	Vector3D w1 (1 ,2 ,3);
	Vector3D w2 (0 ,0 ,4);
	Vector3D Ov (0 ,0 ,0);
	Vector3D Ow (1 ,1 ,1);
	CoordinateFrame3D V(v1 ,v2 ,v3 ,Ov ); // Erstelle V
	CoordinateFrame3D W(w1 ,w2 ,Ow ); // Erstelle W
	Matrix4D M = V. mapTo (W); //M enthält p und R

	Matrix3D R = M. getRotation ();
	Vector3D p = M. getTranslation ();

	YPR Y(R);
	Quaternion q=R. toQuaternion ();

    return 1;
}
