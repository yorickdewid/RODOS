/*******Rodos qui implemented for QT **************/

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMessageBox>

#include <QtGui/QApplication>
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <stdio.h>
#include <cstdlib>

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include "rodosgui-server.h"
#include "udp_common.h"
#include "fifo-simple.h"

using namespace RODOS;

static Fifo<GuiCommand, 100> guiCmd; // to pass commands from udp interface to the QT painter
static bool enableMosueReports = false;

/**********************************************************/
/************* THE interface to QT, from RODOS ************/
/**********************************************************/

class QTRodosGui: public QWidget {
	//Q_OBJECT  //if I use it I get linker errors!
public:
	QPixmap drawPlane;

public:
	QTRodosGui();

	// Frame properties
	void setPositionOnDesktop(int x, int y);
	void setPosAndSize(int x, int y, int width, int height);
	void setBackgroundColor(int R, int G, int B, int alpha_channel);
	void clearScreen();

	// OUTPUTS
	void drawText(int x, int y, const char* strMsg, int R = 0, int G = 0, int B = 0);
	void drawLine(int x1, int y1, int x2, int y2, int R = 0, int G = 0, int B = 0);
	void drawRectangle(int x, int y, int width, int height, int R = 0, int G = 0, int B = 0, int alpha_channel = 0);
	void fillRectangle(int x, int y, int width, int height, int R = 0, int G = 0, int B = 0, int alpha_channel = 0);
	void drawCircle(int center_x, int center_y, int radius, int R = 0, int G = 0, int B = 0, int alpha_channel = 255);
	void fillCircle(int center_x, int center_y, int radius, int R = 0, int G = 0, int B = 0, int alpha_channel = 255);
	void drawPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1, int x2, int y2, int x3, int y3, int morePoints[]);
	void fillPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1, int x2, int y2, int x3, int y3, int morePoints[]);

	// Upcall INPUTS: Events
	void paintEvent(QPaintEvent *event);

	void mouseReleaseEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);
	void timerEvent(QTimerEvent * event);

	void executeCommand(GuiCommand &gCmd);
};

/****************************************************************/
/****************************************************************/
/****************************************************************/

QTRodosGui::QTRodosGui() {
	// this->setUpdatesEnabled(true);
	this->setMouseTracking(true); // to get an upcall for each mosue move
}

void QTRodosGui::setBackgroundColor(int R, int G, int B, int alpha_channel) {
	QColor color(R, G, B, alpha_channel);
	QPalette p;
	p.setColor(QPalette::Background, color);
	this->setPalette(p);
	//this->repaint();
	//this->update(); // infinit loop!
}

void QTRodosGui::setPositionOnDesktop(int x, int y) {
	this->resize(x, y);
}

void QTRodosGui::setPosAndSize(int x, int y, int width, int height) {
	this->setGeometry(x, y, width, height);

	QPixmap pm(width, height);
	pm.fill(QColor(0, 0, 0, 0));
	drawPlane = pm;
	//this->setMinimumSize(width,height);
	//this->update(); // infinit loop!
}

void QTRodosGui::clearScreen() {
	drawPlane.fill(QColor(0, 0, 0, 0));
	//this->update();
}

/************************ Upcalls ********************************/

static GuiCommand cmdToExecute;
// Repaint ......  called from each update()
void QTRodosGui::paintEvent(QPaintEvent *) {
	QPainter painter(this);

	while (guiCmd.get(cmdToExecute)) {
		executeCommand(cmdToExecute);
	}

	painter.drawPixmap(0, 0, drawPlane);
	painter.end();
	// this->update();  infinity loop
}

void QTRodosGui::timerEvent(QTimerEvent * event) {
	printf("timer event\n");
	// update();
}

/************* Events to be reported bak to RODOS ****************/

UDPOutLx udpOut(6022, (char*) "localhost");

int lastX = 0;
int lastY = 0;

void QTRodosGui::mouseReleaseEvent(QMouseEvent* event) {
	printf("release\n");
	lastX = event->x();
	lastY = event->y();
	GuiEvent guiEvent(lastX, lastY);
	guiEvent.releasedKey = event->button();
	udpOut.send(&guiEvent, sizeof(guiEvent));
}

void QTRodosGui::mousePressEvent(QMouseEvent* event) {
	printf("press\n");
	lastX = event->x();
	lastY = event->y();
	GuiEvent guiEvent(lastX, lastY);
	guiEvent.pressedKey = event->button();
	udpOut.send(&guiEvent, sizeof(guiEvent));
}

void QTRodosGui::mouseDoubleClickEvent(QMouseEvent* event) {
	printf("click\n");
	lastX = event->x();
	lastY = event->y();
	GuiEvent guiEvent(lastX, lastY);
	guiEvent.pressedKey = event->button();
	guiEvent.releasedKey = event->button();
	udpOut.send(&guiEvent, sizeof(guiEvent));
}

// warning: to use this, reactivate in constructor setmousetraking(true)
void QTRodosGui::mouseMoveEvent(QMouseEvent* event) {
	lastX = event->x();
	lastY = event->y();
	if (!enableMosueReports)
		return;
	printf("move\n");
	lastX = event->x();
	lastY = event->y();
	GuiEvent guiEvent(lastX, lastY);
	udpOut.send(&guiEvent, sizeof(guiEvent));
}

void QTRodosGui::keyPressEvent(QKeyEvent* event) {
	printf("key pres\n");
	GuiEvent guiEvent(lastX, lastY);
	guiEvent.pressedKey = event->key();
	udpOut.send(&guiEvent, sizeof(guiEvent));
}

void QTRodosGui::keyReleaseEvent(QKeyEvent* event) {
	printf("key relase\n");
	GuiEvent guiEvent(lastX, lastY);
	guiEvent.releasedKey = event->key();
	udpOut.send(&guiEvent, sizeof(guiEvent));
}

void QTRodosGui::enterEvent(QEvent* event) {
}
void QTRodosGui::leaveEvent(QEvent* event) {
}

/*********************Outpus: Drawing  ***************************************/

void QTRodosGui::drawLine(int x1, int y1, int x2, int y2, int R, int G, int B) {
	QColor color(R, G, B, 255);
	QPen pen(color, 2, Qt::SolidLine);
	QPainter painter(&drawPlane);
	painter.setPen(pen);
	painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));
	painter.end();
	//this->update(); // infinity loop
}

void QTRodosGui::drawRectangle(int x, int y, int width, int height, int r, int g, int b, int alpha_channel) {
	QColor color(r, g, b, alpha_channel);
	QPen pen(color, 2, Qt::SolidLine);
	QPainter painter(&drawPlane);
	painter.setPen(pen);
	painter.drawRect(x, y, width, height);
	painter.end();
	//this->update();
}

void QTRodosGui::fillRectangle(int x, int y, int width, int height, int R, int G, int B, int alpha_channel) {
	QPen pen(Qt::black, 2, Qt::SolidLine);
	QPainter painter(&drawPlane);
	painter.setPen(pen);
	QColor color(R, G, B, alpha_channel);
	painter.fillRect(x, y, width, height, color);
	painter.end();
	//this->update();
}

void QTRodosGui::drawText(int x, int y, const char* strMsg, int R, int G, int B) {
	QColor color(R, G, B, 255);
	QPen pen(color, 2, Qt::SolidLine);
	QPainter painter(&drawPlane);
	painter.setPen(pen);
	painter.drawText(x, y, strMsg);
	painter.end();
	//this->update();
}

void QTRodosGui::drawCircle(int center_x, int center_y, int radius, int R, int G, int B, int alpha_channel) {
	QColor color(R, G, B, alpha_channel);
	QPen pen(color, 2, Qt::SolidLine);
	QPainter painter(&drawPlane);
	painter.setPen(pen);
	painter.drawEllipse(center_x - radius, center_y - radius, radius * 2, radius * 2);
	painter.end();
	//this->update();
}

void QTRodosGui::fillCircle(int center_x, int center_y, int radius, int R, int G, int B, int alpha_channel) {
	QColor color(R, G, B, alpha_channel);
	QPen pen(color, 2, Qt::SolidLine);
	QBrush brush(color);
	QPainter painter(&drawPlane);
	painter.setBrush(brush);
	painter.setPen(pen);
	painter.drawEllipse(center_x - radius, center_y - radius, radius * 2, radius * 2);
	painter.end();
	//this->update();
}

void QTRodosGui::drawPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1, int x2, int y2, int x3, int y3, int morePoints[]) {
	QColor color(R, G, B, alpha_channel);
	QPen pen(color, 2, Qt::SolidLine);
	QPainter painter(&drawPlane);
	QPoint points[pointCount];
	points[0] = QPoint(x1, y1);
	points[1] = QPoint(x2, y2);
	points[2] = QPoint(x3, y3);
	for (int j = 3; j < pointCount; j++) {
		points[j] = QPoint(morePoints[(j - 3) * 2], morePoints[(j - 3) * 2 + 1]);
	}
	painter.setPen(pen);
	painter.drawPolygon(points, pointCount);
	painter.end();
}

void QTRodosGui::fillPolygon(int R, int G, int B, int alpha_channel, int pointCount, int x1, int y1, int x2, int y2, int x3, int y3, int morePoints[]) {
	QColor color(R, G, B, alpha_channel);
	QPen pen(color, 0, Qt::NoPen);
	QBrush brush(color);
	QPainter painter(&drawPlane);
	QPoint points[pointCount];
	points[0] = QPoint(x1, y1);
	points[1] = QPoint(x2, y2);
	points[2] = QPoint(x3, y3);
	for (int j = 3; j < pointCount; j++) {
		points[j] = QPoint(morePoints[(j - 3) * 2], morePoints[(j - 3) * 2 + 1]);
	}
	painter.setBrush(brush);
	painter.setPen(pen);
	painter.drawPolygon(points, pointCount);
	painter.end();
}

/***************************************************************/
/***************************************************************/
/***************************************************************/

void QTRodosGui::executeCommand(GuiCommand &gCmd) {

	switch (gCmd.commandNr) {
	case GUICMD_NOP:

		break;
	case GUICMD_ACTIVATE_MOUSEMOVE_EVENTS:
		enableMosueReports = gCmd.param[0];

		break;
	case GUICMD_POS_AND_SIZE:
		setPosAndSize(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);

		break;
	case GUICMD_CLEAR:
		clearScreen();

		break;
	case GUICMD_BACKGROUND:
		setBackgroundColor(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3]);

		break;
	case GUICMD_DRAW_TEXT:
		drawText(gCmd.param[0], gCmd.param[1], (char*) &gCmd.param[5], gCmd.param[2], gCmd.param[3], gCmd.param[4]);

		break;
	case GUICMD_DRAW_LINE:
		drawLine(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6]);

		break;
	case GUICMD_DRAW_BOX:
		drawRectangle(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6], gCmd.param[7]);

		break;
	case GUICMD_DRAW_CIRCLE:
		drawCircle(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6]);

		break;
	case GUICMD_FILL_BOX:
		fillRectangle(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6], gCmd.param[7]);
		break;
	case GUICMD_FILL_CIRCLE:
		fillCircle(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6]);

		break;
	case GUICMD_DRAW_POLYGON:
		drawPolygon(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6], gCmd.param[7], gCmd.param[8], gCmd.param[9], gCmd.param[10],
				&gCmd.param[11]);
		break;
	case GUICMD_FILL_POLYGON:
		fillPolygon(gCmd.param[0], gCmd.param[1], gCmd.param[2], gCmd.param[3], gCmd.param[4], gCmd.param[5], gCmd.param[6], gCmd.param[7], gCmd.param[8], gCmd.param[9], gCmd.param[10],
				&gCmd.param[11]);
		break;
	default:
		printf("undef gui command %d\n", gCmd.commandNr);
	}

	gCmd.commandNr = GUICMD_NOP; // to avoid executing one comamnd more than one time
}

/****************************************************************
 **
 ** creates the qui using QT calls, it hs to be called
 ** from the main POSIX thread (works only with posix).
 ****************************************************************/

// hellothread/hellothread.h
class UdpToQt: public QThread {
	//Q_OBJECT
private:
	void run();
} udpreader;

QTRodosGui* gui1Ptr = 0;
main (int parn, char* parv[]) {
	QApplication theApp(parn, parv);
	QTRodosGui gui1;
	gui1Ptr = &gui1;

	// QTRodosGui gui2; // example if you want more windows

	// Default Windows Settings ------------------------
	gui1.setPositionOnDesktop(300,300);
	gui1.setPosAndSize(100, 100, 500,400);
	gui1.setBackgroundColor(100, 100, 100, 255);
	gui1.show();
	//gui1.startTimer(100);
	gui1.update();

	udpreader.start();

	theApp.exec();
	gui1Ptr = 0;
}

/*******************************************************************************/
/***********         UDP Command Receiver     **********************************/
/*******************************************************************************/

UDPInLx udpIn(6021);

void UdpToQt::run() {
	GuiCommand inCmd;

	// sleep(2); // else crash!
	while (1) {
		//usleep(50000); // if it is too fast, it crashes!
		bool ok = udpIn.get(&inCmd);
		if (ok) {
			guiCmd.put(inCmd);
			gui1Ptr->update(); // will call paintEvent, it calls executeCommand, which reads gCmd
		}

	}
}

