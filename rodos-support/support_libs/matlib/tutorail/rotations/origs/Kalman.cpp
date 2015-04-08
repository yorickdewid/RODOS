#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {


	//Funktion fuer einen Stoervektor
	Vector3D frand() {
			 double r = 2*((rand()/(double)RAND_MAX)) - 0.5;
			 Vector3D d(900*r,130*r,300*r);
		return d;
	}
	//___________________________________________

	//Funktion um die korrekten Werte zu berechnen
	Vector3D getReal(double t){
	  double z = 6*t+2;
	  double y = 3*t*t+2*t;
	  double x = t*t*t +t*t ;
	  Vector3D real(x,y,z);
	  return real;
	  
	}
	//Initialisierung
		Vector3D x_est_last (0,0,0);
	//Initialmatrix P, Nullmatrix
		Matrix3D P_last(x_est_last,x_est_last,x_est_last);

	//Stoermatrix Q
		Vector3D q(0.022,0.022,0.022);
		Matrix3D Q(q);
		
	 //Stoermatrix R   
		Vector3D r (4.617,4.617,4.617);
		Matrix3D R(r);
		
	//Kalmangain , wird im ersten Schritt berechnet    
		Matrix3D K;
	// Matrix P , wird im ersten Schritt berechnet
		Matrix3D P;
	//Matrix H, die Identitaet
		Matrix3D H;
	//Systemmatrix A, wie oben angegeben
		double arr[]={1,1,0.5,0,1,1,0,0,1};
		Matrix3D A (arr);
	// wird in den apriori Gleichungen errechnet
		Matrix3D P_temp;
	// wird in den apriori Gleichungen errechnet
		Vector3D x_temp_est;
	//wird in den aposteriori Gleichungen errechnet	
		Vector3D x_est;
	// Zusammenfassung des Vektors 6*u + w
		Vector3D w(1.022,3.022,6.022);
	// eigentlicher Messwert , wird von frand() errechnet
		Vector3D x_measured;
	// Initialisiert als Messwert	
		x_est_last = getReal(0.0).vecAdd(frand());
	//_________________________________________

	// 30 Schritte
	for(int i = 0;i<30;i++){
	//Prediction , a priori Gleichungen
	  x_temp_est =  x_est_last.matVecMult(A).vecAdd(w);
	  P_temp = A.mMult(P_last.mMult(A.transpose())).mAdd(Q);
					 
	//Kalmangain
	  K = P_temp.mMult(P_temp.mAdd(R).invert());
	//measure
	  x_measured = getReal(i).vecAdd(frand());
					 
	//correct, a posteriori
	  Vector3D temp;
	  temp = (x_measured.vecSub(x_temp_est).matVecMult(K));
	  x_est = x_temp_est.vecAdd(temp);
	  P = H.mSub(K).mMult(P_temp);

	  
	//Update System
	  P_last = P;
	  x_est_last = x_est;
				
	}

    system("Pause");
    return 1;
}