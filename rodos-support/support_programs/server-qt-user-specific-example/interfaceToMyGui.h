#ifndef RODOS_GUI_H
#define RODOS_GUI_H
 
#include "ui_myGui-designer.h"
 
class InterfaceToMyGUI : public QMainWindow, public Ui::MainWindow{
    Q_OBJECT
 
public:
    InterfaceToMyGUI (QMainWindow *parent = 0);
    ~InterfaceToMyGUI();

private slots:

/******* add here your upcalls, from qt events: **
 ******  take as ref generated-GUI-drawer.h aprox from lin 35 ***/

    void pedastrian1Event();
    void pedastrian2Event();
    void sendTextEvent();
    void lineEditEvent();
    void dialEvent(int value);
    void horizontalSliderEvent(int value);

};

#endif
