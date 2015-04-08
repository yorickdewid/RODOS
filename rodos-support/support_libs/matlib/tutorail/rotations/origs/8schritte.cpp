#include "matlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "matlib.h"

int main(void) {
	double rx = -37 e6;
	double ry = 45 e6;
	double rz = 38.5 e6;
	double mu = 3.986 e14 ;
	double Re = 6378137;
	double vx = 3150;
	double vy = -4830;
	double vz = -2860;
	Vector3D r0(rx ,ry ,rz );
	Vector3D v0(vx ,vy ,vz );
	Vector3D exE (1 ,0 ,0);
	Vector3D eyE (0 ,1 ,0);
	Vector3D ezE (0 ,0 ,1);
	Vector3D h_vec = r0. cross (v0 );
	double h = h_vec . getLength ();
	Vector3D q_vec ;
	q_vec = v0. cross ( h_vec ). vecSub (r0. normalize (). scale (mu ));
	double q = q_vec . getLength ();
	double p = h*h/mu;
	double e = q/mu;
	Vector3D ez = h_vec . normalize ();
	Vector3D ex = q_vec . normalize ();
	Vector3D ey =ez. cross (ex );
	Vector3D n;
	n = ezE. cross (ez ). scale (1/( ezE . cross (ez ). getLength ()));
	Vector3D er = r0. normalize ();
	double i = acos (ez.dot( ezE ));
	double Omega = atan2 (n. dot ( eyE ),(n. dot ( exE )));
	double omega = acos (n.dot(ex ));
	double theta0 = atan2 (er. dot (ey),er. dot (ex ));
	// e >1 Fall
	double F = 2* atanh ( sqrt ((e -1)/( e +1))* tan (0.5* theta0 ));
	double tau = e* sinh (F)-F;
	double a = p/(e*e -1);
	double t = -tau /( sqrt (mu /(a*a*a )));
	// Berechne Perigaeumsabstand
	double rp =a*(e -1);
	// Berechne wahre Anomalie des Aufpralls
	double theta_knall = -acos (1/ e*(p/Re -1));
	// Berechne verbleibende Zeit
	double F_knall ;
	F_knall = 2* atanh ( sqrt ((e -1)/( e +1))* tan (0.5* theta_knall ));
	double tau_knall = e* sinh ( F_knall )- F_knall ;
	double t_knall = -tau_knall /( sqrt (mu /(a*a*a )));
	// Berechne Aufprallort in ECI - Koordinaten
	double ot = omega + theta_knall ;
	double rx_knall ;
	double ry_knall ;
	double rz_knall ;
	rx_knall = cos (ot )* cos ( Omega )- sin (ot )* sin ( Omega )* cos (i);
	ry_knall = cos (ot )* sin ( Omega )+ sin (ot )* cos ( Omega )* cos (i);
	rz_knall = sin (ot )* sin (i);
	Vector3D ECI_knall ( rx_knall , ry_knall , rz_knall );
	ECI_knall = ECI_knall . scale (p /(1+ e*cos( theta_knall )));
	// ECI nach ECF nach geodetic am 13.3.7 um 0:00:00
	double y2k = daysSinceY2k (2007 ,3 ,13 ,0 ,0 ,0);
	Matrix3D toECF = eciToECF ( y2k );
	Vector3D ecf_knall = ECI_knall . matVecMult ( toECF );
	Vector3D geodetic = ecfToGeodetic ( ecf_knall );



    system("Pause");
    return 1;
}