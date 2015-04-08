/*******Rodos qui implemented for QT **************/

#include "rodos.h"
#include "rodosgui-client.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/****************************************************************
 ****************************************************************/

Topic<GuiEvent> guiEvent(-1, "GUIEvents");
Topic<GuiCommand> guiCommand(-1, "GUICommands");

Topic<GenericMsgRef> guiUdpMessages(-1, "uguUdpMessages");

static Application guiOnQt("GuiCliet", 30);
static UDPOut guiOut(6021);
static UDPIn guiIn(6022, &guiUdpMessages);

/****************** Handle incomming events from the GUI  *******/

class GuiUdpreceiver: public Subscriber, public Thread {
	CommBuffer<GenericMsgRef> inputMsgBuffer;
public:
	GuiUdpreceiver() :
			Subscriber(guiUdpMessages, "GuiUdpReceiver") {
	}

	void putFromInterrupt(const long topicId, const void* data, int len) {
		inputMsgBuffer.put(*(GenericMsgRef*) data);
		this->resume(); // not to publish from interrupt, call a thread to do it
	}
	void run() {
		GenericMsgRef inputMsg;
		while (1) {
			if (inputMsgBuffer.getOnlyIfNewData(inputMsg)) {
				rodosGui.lastEvent = *(GuiEvent*) inputMsg.msgPtr; // copy fast before the next interrupt comes
				guiEvent.publish(rodosGui.lastEvent);
			} else {
				suspendCallerUntil(NOW()+ 1*SECONDS); // will be resumed from putFromInterrupt
			}
		} // loop
	} // run
} guiUdpReceiver;

				/*******************************************************/

void GuiEvent::printMe() {
	xprintf("Guievent (%d,%d) key v %d ^ %d Usr: %d '%s'\n", x, y, pressedKey, releasedKey, userEvent, text);
}

void GuiCommand::printMe() {
	xprintf("Guicommand win %d, cmd %d, first params: %d %d %d %d", windowNr, commandNr, param[0], param[1], param[2], param[3]);
}

/***************Marschald and forward commands to the GUI *******/

RodosGui::RodosGui() {
}

bool RodosGui::isGuiReady() {
	return true;
}

void RodosGui::userCommand(GuiCommand& guiCommand) {
	guiOut.send(&guiCommand, sizeof(guiCommand));
}

void RodosGui::setPosAndSize(int x, int y, int width, int height) {
	GuiCommand gCmd(1, GUICMD_POS_AND_SIZE);
	gCmd.param[0] = x;
	gCmd.param[1] = y;
	gCmd.param[2] = width;
	gCmd.param[3] = height;
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::setBackgroundColor(int R, int G, int B, int alpha_channel) {
	GuiCommand gCmd(1, GUICMD_BACKGROUND);
	gCmd.param[0] = R;
	gCmd.param[1] = G;
	gCmd.param[2] = B;
	gCmd.param[3] = alpha_channel;
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::clearScreen() {
	GuiCommand gCmd(1, GUICMD_CLEAR);
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::enableMousereports(bool enabled) {
	GuiCommand gCmd(1, GUICMD_ACTIVATE_MOUSEMOVE_EVENTS);
	gCmd.param[0] = enabled;
	guiOut.send(&gCmd, sizeof(gCmd));
}

void RodosGui::drawText(int x, int y, const char* strMsg, int R, int G, int B) {
	GuiCommand gCmd(1, GUICMD_DRAW_TEXT);
	gCmd.param[0] = x;
	gCmd.param[1] = y;
	gCmd.param[2] = R;
	gCmd.param[3] = G;
	gCmd.param[4] = B;
	long maxStrLen = gCmd.MAX_NUM_OF_PARAMS * sizeof(int32_t) - 5 * sizeof(int32_t); // the 5 params x,y,r,g,b
	strncpy((char*) (gCmd.param + 5), strMsg, maxStrLen);
	guiOut.send(&gCmd, sizeof(gCmd));
}

void RodosGui::drawLine(int x1, int y1, int x2, int y2, int R, int G, int B) {
	GuiCommand gCmd(1, GUICMD_DRAW_LINE);
	gCmd.param[0] = x1;
	gCmd.param[1] = y1;
	gCmd.param[2] = x2;
	gCmd.param[3] = y2;
	gCmd.param[4] = R;
	gCmd.param[5] = G;
	gCmd.param[6] = B;
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::drawRectangle(int x, int y, int width, int height, int R, int G, int B, int alpha_channel) {
	GuiCommand gCmd(1, GUICMD_DRAW_BOX);
	gCmd.param[0] = x;
	gCmd.param[1] = y;
	gCmd.param[2] = width;
	gCmd.param[3] = height;
	gCmd.param[4] = R;
	gCmd.param[5] = G;
	gCmd.param[6] = B;
	gCmd.param[7] = alpha_channel;
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::drawCircle(int center_x, int center_y, int radius, int R, int G, int B, int alpha_channel) {
	GuiCommand gCmd(1, GUICMD_DRAW_CIRCLE);
	gCmd.param[0] = center_x;
	gCmd.param[1] = center_y;
	gCmd.param[2] = radius;
	gCmd.param[3] = R;
	gCmd.param[4] = G;
	gCmd.param[5] = B;
	gCmd.param[6] = alpha_channel;
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::fillRectangle(int x, int y, int width, int height, int R, int G, int B, int alpha_channel) {
	GuiCommand gCmd(1, GUICMD_FILL_BOX);
	gCmd.param[0] = x;
	gCmd.param[1] = y;
	gCmd.param[2] = width;
	gCmd.param[3] = height;
	gCmd.param[4] = R;
	gCmd.param[5] = G;
	gCmd.param[6] = B;
	gCmd.param[7] = alpha_channel;
	guiOut.send(&gCmd, sizeof(gCmd));
}
void RodosGui::fillCircle(int center_x, int center_y, int radius, int R, int G, int B, int alpha_channel) {
	GuiCommand gCmd(1, GUICMD_FILL_CIRCLE);
	gCmd.param[0] = center_x;
	gCmd.param[1] = center_y;
	gCmd.param[2] = radius;
	gCmd.param[3] = R;
	gCmd.param[4] = G;
	gCmd.param[5] = B;
	gCmd.param[6] = alpha_channel;
	guiOut.send(&gCmd, sizeof(gCmd));
}

void RodosGui::drawPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1, int x2, int y2, int x3, int y3, ...) {
	GuiCommand gCmd(1, GUICMD_DRAW_POLYGON);
	if ((pointCount * 2) + 5 > gCmd.MAX_NUM_OF_PARAMS) {
		pointCount = (gCmd.MAX_NUM_OF_PARAMS - 5) / 2;
	}
	gCmd.param[0] = R;
	gCmd.param[1] = G;
	gCmd.param[2] = B;
	gCmd.param[3] = alpha_channel;
	gCmd.param[4] = pointCount;
	gCmd.param[5] = x1;
	gCmd.param[6] = y1;
	gCmd.param[7] = x2;
	gCmd.param[8] = y2;
	gCmd.param[9] = x3;
	gCmd.param[10] = y3;
	va_list ap;
	va_start(ap, y3);
	for (int j = 11; j < (pointCount * 2) + 11; j++) {
		gCmd.param[j] = va_arg(ap, int);
	}
	va_end(ap);
	guiOut.send(&gCmd, sizeof(gCmd));
}

void RodosGui::fillPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1, int x2, int y2, int x3, int y3, ...) {
	GuiCommand gCmd(1, GUICMD_FILL_POLYGON);
	if ((pointCount * 2) + 5 > gCmd.MAX_NUM_OF_PARAMS) {
		pointCount = (gCmd.MAX_NUM_OF_PARAMS - 5) / 2;
	}
	gCmd.param[0] = R;
	gCmd.param[1] = G;
	gCmd.param[2] = B;
	gCmd.param[3] = alpha_channel;
	gCmd.param[4] = pointCount;
	gCmd.param[5] = x1;
	gCmd.param[6] = y1;
	gCmd.param[7] = x2;
	gCmd.param[8] = y2;
	gCmd.param[9] = x3;
	gCmd.param[10] = y3;
	va_list ap;
	va_start(ap, y3);
	for (int j = 11; j < (pointCount * 2) + 11; j++) {
		gCmd.param[j] = va_arg(ap, int);
	}
	va_end(ap);
	guiOut.send(&gCmd, sizeof(gCmd));
}

void RodosGui::loadPoly3d(const char* strFile, const int myIdForIt) {
	GuiCommand gCmd(1, GUICMD_LOAD_POLY);
	gCmd.param[0] = myIdForIt;
	long maxStrLen = gCmd.MAX_NUM_OF_PARAMS * sizeof(int32_t);
	strncpy((char*) (&gCmd.param[1]), strFile, maxStrLen);
	guiOut.send(&gCmd, sizeof(gCmd));
}

void RodosGui::rotateTranslatePoly3d(const int myIdForIt, float x_trans, float y_trans, float z_trans, float x_rot, float y_rot, float z_rot) {
	GuiCommand gCmd(1, GUICMD_ROTATE_TRANSLATE_POLY);
	gCmd.param[0] = myIdForIt;
	gCmd.paraf[1] = x_trans;
	gCmd.paraf[2] = y_trans;
	gCmd.paraf[3] = z_trans;
	gCmd.paraf[4] = x_rot;
	gCmd.paraf[5] = y_rot;
	gCmd.paraf[6] = z_rot;
	guiOut.send(&gCmd, sizeof(gCmd));
}

/********************************** Singletons *******************************/

RodosGui rodosGui;

class GuiCommandForwarder: public Subscriber {
public:
	GuiCommandForwarder() :
			Subscriber(guiCommand, "guiCommandForwarde") {
	}

    long put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
		guiOut.send(data, sizeof(GuiCommand));
		return 1;
	}
} guiCommandForwarder;

#ifndef NO_RODOS_NAMESPACE
}
#endif
