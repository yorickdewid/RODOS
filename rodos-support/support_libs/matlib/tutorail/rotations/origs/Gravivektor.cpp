#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {

	Vector3D getOrtsvector ( double gx , double gy , double gz ){
	double alpha = atan2 (gy ,gx );
	alpha = alpha *180/ M_PI ;
	double beta = atan2 (gz ,gy );
	beta = beta *180/ M_PI ;
	double gamma = atan2 (gx ,gz );
	gamma = gamma *180/ M_PI ;
	Vector3D g(alpha ,beta , gamma );
	return g;
	}

	Quaternion ( const YPR & ypr);
	Vector3D P(1 ,2 ,3) // ausgangslage
	Vector3D P_akt ;
	YPR ypr (gamma ,beta , alpha );
	Quaternion q( ypr );
	P_akt = P. qRotate (q);


    system("Pause");
    return 1;
}