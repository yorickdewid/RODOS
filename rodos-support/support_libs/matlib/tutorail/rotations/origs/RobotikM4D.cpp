#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
	// Schritt 1
	double t1 [16]={0 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,0 ,1 ,0 ,500 ,0 ,0 ,0 ,1};
	double t2 [16]={1 ,0 ,0 ,0 ,0 ,0 , -1 ,0 ,0 ,1 ,0 ,10 ,0 ,0 ,0 ,1};
	double t3 [16]={0 , -1 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,300 ,0 ,0 ,0 ,1};
	Matrix4D T01(t1 );
	Matrix4D T12(t2 );
	Matrix4D T23(t3 );
	// Schritt 2
	Matrix4D T_toolBase = T01 . mMult ( T12 . mMult ( T23 ));
	// Schritt 3
	Vector3D v(1 ,1 ,1);
	Vector4D p = v. to4D ();
	p_Base = p. matVecMult ( T_toolBase ); //[11 , -1 ,199 ,1]



    system("Pause");
    return 1;
}