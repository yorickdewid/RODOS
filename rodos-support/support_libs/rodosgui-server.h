#pragma once


/** GUI Interface for Rodos
  * may by implemented in differten technoloiges and platforms
  * this is not a core component of rodos, just an extra component
  **/

// WARNING!!!!
// If you modify here, you have to update the server_java_freedrowing by hand

#include "stdlib.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

struct GuiEvent {
    static const int MAX_CHARS = 100;
    GuiEvent() { windowNr = 1; x = y =  pressedKey = releasedKey = userEvent = 0; text[0] = 0; }
    GuiEvent(int px, int py) { windowNr = 1; x = px; y = py; pressedKey = releasedKey = userEvent = 0; text[0] = 0; }

    int32_t windowNr;    
    int32_t x, y;        // last known position from mouse
    int32_t pressedKey;  // last kexboard and mosue keys. 0 if key was relased, double click: bot set
    int32_t releasedKey; // last kexboard and mosue keys. 0 if key was presed,  double click: bot set
    int32_t userEvent;   // user defined ovents, please write 0 for pres and mouse events
    char    text[MAX_CHARS];   // in case of text windows, optinal parameter
    void printMe();
};

struct GuiCommand {
    static const int32_t MAX_NUM_OF_PARAMS = 64; // so big due to text strings
    GuiCommand() { }
    GuiCommand(int32_t wn, int32_t cmd) { windowNr = wn; commandNr = cmd; }

    int32_t windowNr;
    int32_t commandNr;
    union {
        int32_t param[MAX_NUM_OF_PARAMS];
        float   paraf[MAX_NUM_OF_PARAMS];
    };

    void printMe();
};


enum guiCommandNames {
    GUICMD_NOP = 0,
    GUICMD_ACTIVATE_MOUSEMOVE_EVENTS,
    GUICMD_POS_AND_SIZE,
    GUICMD_CLEAR,
    GUICMD_BACKGROUND,
    GUICMD_DRAW_TEXT,
    GUICMD_DRAW_LINE,
    GUICMD_DRAW_BOX,
    GUICMD_DRAW_CIRCLE,
    GUICMD_FILL_BOX,
    GUICMD_FILL_CIRCLE,
    GUICMD_DRAW_POLYGON,
    GUICMD_FILL_POLYGON,

    GUICMD_3D_COMMANDS   = 16000,
    GUICMD_LOAD_POLY,
    GUICMD_ROTATE_TRANSLATE_POLY,

    GUICMD_USER_COMMANDS = 0x010000000,
    GUICMD_USER_COMMAND1,
    GUICMD_USER_COMMAND2,
    GUICMD_USER_COMMAND3,
    GUICMD_USER_COMMAND4  // etc.... 

};


#ifndef NO_RODOS_NAMESPACE
}
#endif

