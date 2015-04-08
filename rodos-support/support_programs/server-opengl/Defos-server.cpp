/**
 * @file Defos.cpp
 * @date 2012/06/25
 * @author Julius Peinelt
 *
 * Copyright Julius Peinelt
 *
 * A DefosServer is Subclass of Thread, it handles Communication with
 * clients and takes care of the computation of requests from clients
 *
 */

#include <GL/freeglut.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for  inet_addr
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>


#include "./Helper/jMath.h"
#include "./Visualisation/Visualisation.h"
#include "../rodosgui-server.h"
#include "./Helper/fifo.h"


using namespace RODOS;

static Fifo<GuiCommand, 100> guiCmd; // to pass commands from udp interface to the OpenGl-window
static int sock; // udp-server socket
static char inputBuf[1400];// as long as a UDP packet can be
sockaddr_in inputAddr; // address for udp-socket

/**
 * udp reader function - called by interrupt
 */
void udpReader(int a) {
    GuiCommand *guiCommand;
    while (read(sock, inputBuf, 1400) > 0) {
        guiCommand = (GuiCommand *) inputBuf;
        guiCmd.put(*guiCommand);
    }
}

void keyPressed(unsigned char key, int x, int y) {
    usleep(100); /* sleep to avoid thrashing this procedure */
    printf("key %c at %d %d\n", key, x, y);
}


void mouseEvent(int button, int state, int x, int y) {
    printf("mouse vent but %d, state %d at (%d,%d)\n", button, state, x, y);
}

void mouseMotionEvent(int x, int y) {
    printf("mouse move (%d,%d)\n",  x, y);
}

/**
 * @class DefosServer
 * @brief %DefosServer main opengl-server class.
 */
class DefosServer {

private:
    GuiCommand guiCommand;
    Visualisation* visualisations[1];
    vector<long> polyederIDs;

    /**
     * Construktor.
     * Calls the parent class construktor and sets the
     * name of the Thread to "DefosServer" and the priority to 250.
     */
public:
    DefosServer() {
    }

    /**
     * Destructor.
     * Closes udp-server socket.
     */
public:
    ~DefosServer() {
        close(sock);
    }
    /**
     * Initialisation of the DefosServer.
     * Creates default arguments for calling initializeGraphics() sets up freeglut.
     * @overrides Thread::init()
     */
public:
    void init() {
        udpInit(6021);
        char* myargv[1];
        int myargc = 1;
        myargv[0] = (char*) "Defos";
        initializeGraphics(myargc, myargv);
    }

    /**
     * Sets up the freeglut library.
     * Initialize the freeglut library and sets the initial display mode to
     * double buffered and rgb color model.
     * @params argc a default value which is not used
     * @params argv[] a default pointer which is not used
     */
private:
    void initializeGraphics(int argc, char* argv[]) {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // enable dobble buffer & use RGB
    }

    /**
     * Induces the displaying of a window.
     * Calls  Visualisation::display() which initiate the drawing of objects.
     */
private:
    void display() {
        visualisations[0]->display();
    }

    /**
     * Gets called by updateVisualisation() and causes a Visualisation to
     * create a new Polyhedron from the 3D model file specified by the parameter
     * @params filename 3D model file (a wavefront .obj file)
     */
private:
    size_t createPolyhedron(string& filename) {
        return visualisations[0]->createPolyhedron(filename);
    }

    /**
     * Run-method - never terminates. It creates a Visualisation and asks
     * the FIFO buffers for new requests from the clients. After computation of these
     * requests, the display is refreshed.
     */
public:
    void run() {
        visualisations[0] = new Visualisation(string("Defos 0"), 100, 100, 800,
                                              600); // POSITION & SIZE
        display();
        glutKeyboardFunc(keyPressed);
        glutMouseFunc(mouseEvent);
        glutPassiveMotionFunc(mouseMotionEvent);


        bool ok = false;
        while (1) {
            // get command from fifo
            ok = guiCmd.get(guiCommand);
            if (ok) {
                // react on different commands
                switch (guiCommand.commandNr) {
                case GUICMD_ROTATE_TRANSLATE_POLY: {
                    size_t updatePolyId = 0;
                    // todo? use id, instead of name
                    //updatePolyId = updateBuffer.at(i).polyId;
                    if (updatePolyId == 0) {
                        for (size_t j = 0; j < polyederIDs.size(); j++) {
                            if (polyederIDs.at(j) == guiCommand.param[0]) {
                                updatePolyId = j + 1;
                                break;
                            }
                        }
                    }
                    visualisations[0]->updatePolyhedron(updatePolyId,
                                                         guiCommand.paraf[1],
                                                         guiCommand.paraf[2],
                                                         guiCommand.paraf[3],
                                                         guiCommand.paraf[4],
                                                         guiCommand.paraf[5],
                                                         guiCommand.paraf[6]);
                }
                break;
                case GUICMD_LOAD_POLY: {
                    string filename = (char*) &guiCommand.param[1];
                    createPolyhedron(filename);
                    polyederIDs.push_back(guiCommand.param[0]); // The user determines the ID, not me!
                }
                break;
                }
            }

            //glutMainLoop only needed for callbacks and displaying the window
            glutMainLoopEvent(); //better than glutMainLoop() because it returns

            if (!ok) {
                display();
            }
        }
    }

    /**
     * Initialize udp-socket.
     * @params myPort listening-port
     */
private:
    void udpInit(int myPort) {

        ///// Open and bind socket /////
        sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock == -1) {
            printf("!! UDP-in: cannot open socket\n");
            return;
        }

        memset(&inputAddr, 0, sizeof(inputAddr));
        inputAddr.sin_family = AF_INET; // IPv4
        inputAddr.sin_addr.s_addr = INADDR_ANY; // Use no fixed IP-address for data source
        inputAddr.sin_port = htons(myPort); // Save portaddresss in networkorder
        int retval = bind(sock, (const struct sockaddr*) &inputAddr,
                          sizeof(inputAddr));
        if (retval != 0) {
            printf("!! UDP-in: cannot bind socket\n");
            return;
        }

        ///// enable interrupt for asyn reception /////
        signal(SIGIO, udpReader);

        fcntl(sock, F_SETFL, O_NONBLOCK);
        fcntl(sock, F_SETOWN, getpid());
        /* turn on async mode -- this is the important part which enables signal delivery */
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_ASYNC);

    }

};

/**
 * Main-Function -> calls init and run of DefosServer.
 */
DefosServer myServer;
int main(int argc, const char* argv[]) {
    myServer.init();
    myServer.run();
}

