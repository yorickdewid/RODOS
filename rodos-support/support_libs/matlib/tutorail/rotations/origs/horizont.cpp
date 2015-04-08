#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {

Vector3D origin (1000 ,2000 ,3000); // origin ECEF
Matrix3D Id; // Einheitsmatrix
double angle = M_PI /2;
Matrix3D R_z = Id. rotateZ ( angle );
Vector3D rotated_or = origin . mRotate ( R_z );

Vector3D x(1 ,0 ,0);
Vector3D y(0 , -1 ,0);
Vector3D z(0 ,0 , -1);
CoordinateFrame3D body (x,y,z);
CoordinateFrame3D rotated ;
rotated = body . rotate ( R_z );
    system("Pause");
    return 1;
}