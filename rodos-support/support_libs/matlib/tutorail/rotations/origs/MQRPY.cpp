#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {

Quaternion q(1 ,0 ,0 ,0) // Identity
Matrix3D Id =q. toRotationMatrix ();// Identity
YPR y = q. toYPR (); // 0 ,0 ,0

Matrix3D Id ;// Identity
Quaternion q = Id. toQuaternion ();//[1 ,0 ,0 ,0]
YPR y = Id. toYPR (); // 0 ,0 ,0

YPR y; // 0,0 ,0
Matrix3D Id = y. toMatrix3D ();
Quaternion q = y. toQuaternion ();
    system("Pause");
    return 1;
}