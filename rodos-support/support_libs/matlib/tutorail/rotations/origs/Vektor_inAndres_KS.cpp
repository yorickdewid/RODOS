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
	Vector3D v (1337 ,1 ,1);
	Vector4D v4D =v. to4D (); // 4 dimensional
	Vector4D w4D =v4D. matVecMult (M); // w4d = M* v4D
	Vector3D w = w4D . to3D (); // ergebnis



    system("Pause");
    return 1;
}