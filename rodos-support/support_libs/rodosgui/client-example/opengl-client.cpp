#include "rodos.h"
#include "rodosgui-client.h"

/************** Test GUI output ********************/

#define AIRCRAFT 100
#define HORIZONT 200
#define PIRAMID  300

class OpenGlTester: public Thread {
public:
    void run() {

        rodosGui.loadPoly3d("aircraft.obj", AIRCRAFT);
        rodosGui.loadPoly3d("artificalHorizont.obj", HORIZONT);
        rodosGui.loadPoly3d("pyramid.obj", PIRAMID);

        float rotX = 0.0f;
        float updateX = 1.0f;
        float rotator = 0;
        float translat = -10.0;

        TIME_LOOP	(2*SECONDS, 50*MILLISECONDS) {
            if (rotX < -90.0) {
                updateX = 0.1;
            } else if (rotX > 90.0) {
                updateX = -0.1;
            }
            rotX = rotX + (updateX * 6.0);
            rotator +=  0.3;
            translat -= 0.02;
            rodosGui.rotateTranslatePoly3d(AIRCRAFT,-1.0,-2.0,-20.0, rotX, rotX, rotX);
            rodosGui.rotateTranslatePoly3d(HORIZONT, 2.0, 2.0,-15.0, -rotX, rotX, rotX);
            rodosGui.rotateTranslatePoly3d(PIRAMID,  3.0, 2.0, translat, rotator, rotator, rotator);
        }
    }

} glTester;

