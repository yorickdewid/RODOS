#pragma once

#include "rodosgui-server.h"



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern Topic<GuiCommand> guiCommand;
extern Topic<GuiEvent>   guiEvent;


class RodosGui  {
public:
    RodosGui();

    bool isGuiReady(); 

    // Frame properties
    void setPosAndSize(int x, int y, int width, int height);
    void setBackgroundColor(int R, int G, int B, int alpha_channel=255);
    void clearScreen();
    void enableMousereports(bool enabled);

    // OUTPUTS
    void drawText(int x, int y, const char* strMsg, int R=0, int G=0, int B=0);
    void drawLine(int x1, int y1, int x2, int y2, int R=0, int G=0, int B=0);
    void drawRectangle(int x, int y, int width, int height, int R=0, int G=0, int B=0, int alpha_channel=255);
    void fillRectangle(int x, int y, int width, int height, int R=0, int G=0, int B=0, int alpha_channel=255);
    void drawCircle(int center_x, int center_y, int radius, int R=0, int G=0, int B=0, int alpha_channel=255);
    void fillCircle(int center_x, int center_y, int radius, int R=0, int G=0, int B=0, int alpha_channel=255);
    void drawPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1,int x2, int y2,int x3, int y3, ...);
	void fillPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1,int x2, int y2,int x3, int y3, ...);

    // 3D Commands
    void loadPoly3d(const char* strFile, const int myIdForIt);
    void rotateTranslatePoly3d(const int  myIdForIt,float x_trans, float y_trans, float z_trans,float x_rot, float y_rot, float z_rot);

    void userCommand(GuiCommand& guiCommand);
    //
    GuiEvent lastEvent;
};

extern RodosGui rodosGui; // singleton!


#ifndef NO_RODOS_NAMESPACE
}
#endif

