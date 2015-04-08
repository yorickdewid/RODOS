
#include "interfaceToMyGui.h"
#include <stdio.h>

#include "rodosgui-server.h"
#include "udp_common.h"

using namespace RODOS;

static InterfaceToMyGUI* guiP = 0;

InterfaceToMyGUI::InterfaceToMyGUI(QMainWindow *parent) : QMainWindow(parent){
    setupUi(this);

    /** Init your gui elements
     **  take as ref generated-GUI-drawer.h aprox from lin 35 ***/

    textOutput->setText("here comes the time");
    red1->setChecked(false);
    yellow1->setChecked(false);
    green1->setChecked(false);
    green2->setChecked(false);
    yellow2->setChecked(false);
    red2->setChecked(false);

    red1->setStyleSheet( "border: 2px solid red" );
    yellow1->setStyleSheet( "border: 2px solid yellow" );
    green1->setStyleSheet( "border: 2px solid green" );
    red2->setStyleSheet( "border: 2px solid red" );
    yellow2->setStyleSheet( "border: 2px solid yellow" );
    green2->setStyleSheet( "border: 2px solid green" );


    /** Connect your input buttons to the event servers ***/
    connect(pedastrian1,      SIGNAL(clicked()), this, SLOT(pedastrian1Event()));
    connect(pedastrian2,      SIGNAL(clicked()), this, SLOT(pedastrian2Event()));
    connect(sendtext,         SIGNAL(clicked()), this, SLOT(sendTextEvent()));
    connect(lineEdit,         SIGNAL(clicked()), this, SLOT(lineEditEvent()));
    //connect(dial,             SIGNAL(valueChanged(int)), this, SLOT(dialEvent(int)));
    connect(horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(horizontalSliderEvent(int)));
    //connect(&lineEdit,        SIGNAL(returnPressed()), this, SLOT(sendText(lineEdit.text())));

    /** This pointer will need the cammand executer ***/

    guiP = this;

}
 
InterfaceToMyGUI::~InterfaceToMyGUI() { guiP = 0; }


/********* add here your upcalls from QT Events ******/

UDPOutLx udpOut(6022, (char*)"localhost");


void InterfaceToMyGUI::pedastrian1Event() {
    printf("pedastrian1\n");
    GuiEvent guiEvent(0, 0);
    guiEvent.userEvent = 1;
    udpOut.send(&guiEvent, sizeof(guiEvent));
}
void InterfaceToMyGUI::pedastrian2Event() {
    printf("pedastrian2\n");
    GuiEvent guiEvent(0, 0);
    guiEvent.userEvent = 2;
    udpOut.send(&guiEvent, sizeof(guiEvent));
}
void InterfaceToMyGUI::lineEditEvent() {
    printf("lineedit\n");
    GuiEvent guiEvent(0, 0);
    guiEvent.userEvent = 3;
    udpOut.send(&guiEvent, sizeof(guiEvent));
}
void InterfaceToMyGUI::dialEvent(int val) {
    printf("dial\n");
    GuiEvent guiEvent(0, val);
    guiEvent.userEvent = 4;
    udpOut.send(&guiEvent, sizeof(guiEvent));
}
void InterfaceToMyGUI::horizontalSliderEvent(int val) {
    printf("horontalslider\n");
    GuiEvent guiEvent(val, 0);
    guiEvent.userEvent = 5;
    udpOut.send(&guiEvent, sizeof(guiEvent));
}
void InterfaceToMyGUI::sendTextEvent() {
    printf("sendText\n");
    char* s = (char*)guiP->lineEdit->text().toStdString().c_str();
    // guiP->textOutput->setText(s);
    GuiEvent guiEvent(0, 0);
    guiEvent.userEvent = 7;
    strncpy(guiEvent.text, s, guiEvent.MAX_CHARS);
    udpOut.send(&guiEvent, sizeof(guiEvent));
}


/********* Get and executed commands ***************/

void executeGuiCommand(GuiCommand& gCmd) {
    long lamps = gCmd.commandNr & 0x0ff;

    guiP->red1->setChecked(lamps & 0x01);
    guiP->yellow1->setChecked(lamps & 0x02);
    guiP->green1->setChecked(lamps & 0x04);
    guiP->red2->setChecked(lamps & 0x08);
    guiP->yellow2->setChecked(lamps & 0x10);
    guiP->green2->setChecked(lamps & 0x20);

    char* text = (char*)&(gCmd.param[1]);
    guiP->textOutput->setText(text);

    guiP->dial->setValue(gCmd.param[0]);
    // guiP->horizontalSlider->setValue(gCmd.param[0]);
}


