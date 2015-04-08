// export LIB="-lm -L/opt/X11R5/lib  -lXt -lXmu -lXext -lX11"
// export XINC="-I/opt/X11R5/include -I/usr/include -I/usr/include/X11"
// gcc ${XINC} $* ${LIB}

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <pthread.h>
#include <unistd.h>

#include "rodosgui-server.h"
#include "udp_common.h"
//#include "rodosgui.h"

using namespace RODOS;
#define RGB_COLOR(r, g, b) ( ((r)<<16)  + ((g)<<8) + (b)) 

/*Linux users will need to add -ldl to the Makefile to compile 
 *this example.
 */
Display *dis;
Window win;
XEvent report;
GC grafic_context;
Colormap colormap;

extern void *controller(void *param);

int main() {

	pthread_t controllerThread;
	long dummyParam = 0;

	pthread_create(&controllerThread, NULL, controller, &dummyParam);

	UDPOutLx udpOut(6022, (char*) "localhost");

	printf("see http://tronche.com/gui/x/xlib/graphics/ \n");
	dis = XOpenDisplay(NULL);
	win = XCreateSimpleWindow(dis, RootWindow(dis, 0) , 100, 100, 500, 400, 0, 0, RGB_COLOR(128, 128, 128));
	XMapWindow(dis, win);
	colormap = DefaultColormap(dis, 0) ;
	grafic_context = XCreateGC(dis, win, 0, 0);

	//XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask | PointerMotionMask);
	XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask);

	sleep(1);
	XFlush(dis);

	while (1) {
		XNextEvent(dis, &report);
		//printf("Event gekommen %d\n", report.type);
		GuiEvent* event = new GuiEvent();
		switch (report.type) {
		case Expose:
			fprintf(stdout, "I have been exposed.\n");
			//XDrawRectangle(dis, win, grafic_context, 50, 50, 398, 398);
			//XSetForeground(dis, grafic_context, 255);
			//XFillRectangle(dis, win, grafic_context, 100, 100, 20, 30);
			//XSetForeground(dis, grafic_context, 255 << 16);
			//XDrawString(   dis, win, grafic_context, 200, 200, "hello", strlen("hello"));
			//XFlush(dis);
			break;
		case KeyPress:
			/*Close the program if q is pressed.*/
			if (XLookupKeysym(&report.xkey, 0) == XK_q) {
				exit(0);
			}
			event->pressedKey = XLookupKeysym(&report.xkey, 0);
			event->x = report.xkey.x;
			event->y = report.xkey.y;
			//printf("pressed Key = %d, x = %d, y = %d\n", XLookupKeysym(&report.xkey, 0), event -> x, event -> y);
			//printf("pressed Key = %u\n", report.xkey.keycode);
			break;
		case KeyRelease:
			event->releasedKey = XLookupKeysym(&report.xkey, 0);
			event->x = report.xkey.x;
			event->y = report.xkey.y;
			//printf("released Key = %d, x = %d, y = %d\n", XLookupKeysym(&report.xkey, 0), event -> x, event -> y);
			break;
		case ButtonPress:
			event->pressedKey = report.xbutton.button;
			event->x = report.xbutton.x;
			event->y = report.xbutton.y;
			printf("pressed Button = %u, x = %d, y = %d\n", report.xbutton.button, event->x, event->y);
			break;
		case ButtonRelease:
			event->releasedKey = report.xbutton.button;
			event->x = report.xbutton.x;
			event->y = report.xbutton.y;
			printf("released Button = %u, x = %d, y = %d\n", report.xbutton.button, event->x, event->y);
			break;
		case MotionNotify:
			printf("Maus bewegt\n");
			event->x = report.xmotion.x;
			event->y = report.xmotion.y;
			break;
		default:
			printf("anderes Event\n");
		}

		udpOut.send(event, sizeof(GuiEvent));

	}

	return 0;
}

/*************************************************/

UDPInLx udpIn(6021);
//*
void drawPolygon(GuiCommand gCmd) {
	int pointCount = gCmd.param[4];
	XPoint points[gCmd.param[4]];
	points[0].x = gCmd.param[5];
	points[0].y = gCmd.param[6];
	points[1].x = gCmd.param[7];
	points[1].y = gCmd.param[8];
	points[2].x = gCmd.param[9];
	points[2].y = gCmd.param[10];
	for (int i = 0; i < gCmd.param[4] - 3; i++) {
		points[i + 3].x = gCmd.param[i * 2 + 11];
		points[i + 3].y = gCmd.param[i * 2 + 12];
	}
	XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[0], gCmd.param[1], gCmd.param[2]));
	for (int i = 0; i < pointCount; i++) {
		XDrawLine(dis, win, grafic_context, points[i].x, points[i].y, points[(i + 1) % pointCount].x, points[(i + 1) % pointCount].y);
	}
}
void fillPolygon(GuiCommand gCmd) {
	XPoint points[gCmd.param[4]];
	points[0].x = gCmd.param[5];
	points[0].y = gCmd.param[6];
	points[1].x = gCmd.param[7];
	points[1].y = gCmd.param[8];
	points[2].x = gCmd.param[9];
	points[2].y = gCmd.param[10];
	for (int i = 0; i < gCmd.param[4] - 3; i++) {
		points[i + 3].x = gCmd.param[i * 2 + 11];
		points[i + 3].y = gCmd.param[i * 2 + 12];
	}
	XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[0], gCmd.param[1], gCmd.param[2]));
	XFillPolygon(dis, win, grafic_context, points, gCmd.param[4], Complex, CoordModeOrigin);
}
void *controller(void *param) {

	printf("Gui server 3.5 \n");
	GuiCommand gCmd;

	long waiting = 0;
	while (1) {

		bool ok = udpIn.get(&gCmd);
		printf("command angekommen %d\n", gCmd.commandNr);
		char* text;

		switch (gCmd.commandNr) {

		case GUICMD_ACTIVATE_MOUSEMOVE_EVENTS:
			XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask | (gCmd.param[0] * PointerMotionMask));
			break;

		case GUICMD_POS_AND_SIZE:
			XMoveResizeWindow(dis, win, gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);
			break;

		case GUICMD_CLEAR:
			XClearWindow(dis, win);
			break;

		case GUICMD_BACKGROUND:
			printf("aendere background %d, %d, %d\n", gCmd.param[0], gCmd.param[1], gCmd.param[2]);
			XSetBackground(dis, grafic_context, RGB_COLOR(gCmd.param[0], gCmd.param[1], gCmd.param[2]));
			break;

		case GUICMD_DRAW_TEXT:
			text = (char*) &gCmd.param[5];
			XTextItem textItem;
			textItem.chars = text;
			textItem.nchars = strlen(text);
			XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[2], gCmd.param[3], gCmd.param[4]));
			XDrawText(dis, win, grafic_context, gCmd.param[0], gCmd.param[1], &textItem, 1);
			break;

		case GUICMD_DRAW_LINE:
			printf("drawline %d,%d -> %d,%d\n", gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);
			XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[4], gCmd.param[5], gCmd.param[6]));
			XDrawLine(dis, win, grafic_context, gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);
			break;

		case GUICMD_DRAW_BOX:
			XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[4], gCmd.param[5], gCmd.param[6]));
			XDrawRectangle(dis, win, grafic_context, gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);
			break;

		case GUICMD_DRAW_CIRCLE:
			XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[3], gCmd.param[4], gCmd.param[5]));
			//	Window params	     upper left corner = mid - radius
			XDrawArc(dis, win, grafic_context, gCmd.param[0] - gCmd.param[2], gCmd.param[1] - gCmd.param[2],
			// with and hight= radius * 2       Angle of Start and End
					gCmd.param[2] * 2, gCmd.param[2] * 2, 0, 360 * 64);
			break;

		case GUICMD_FILL_BOX:
			XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[4], gCmd.param[5], gCmd.param[6]));
			XFillRectangle(dis, win, grafic_context, gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);
			break;

		case GUICMD_FILL_CIRCLE:
			XSetForeground(dis, grafic_context, RGB_COLOR(gCmd.param[3], gCmd.param[4], gCmd.param[5]));
			//	Window params	     upper left corner = mid - radius
			XFillArc(dis, win, grafic_context, gCmd.param[0] - gCmd.param[2], gCmd.param[1] - gCmd.param[2],
			// with and hight= radius * 2       Angle of Start and End
					gCmd.param[2] * 2, gCmd.param[2] * 2, 0, 360 * 64);
			break;

		case GUICMD_DRAW_POLYGON:
			drawPolygon(gCmd);
			break;

		case GUICMD_FILL_POLYGON:
			fillPolygon(gCmd);
			break;

		default:
			printf("unbekannter Befehl\n");

		}
		XFlush(dis);
	}
}

