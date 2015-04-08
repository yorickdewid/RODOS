/*****************************************/
/** class LCD							**/
/** athor: Michael Ruffer 				**/
/** Copyright (C) 2012 Michael Ruffer	**/
/** version: 1.0 						**/
/** date: 02.12.2012					**/
/*****************************************/


#ifndef _LCD_H_
#define _LCD_H_


enum OnOff {
    OFF = 0,
    ON
};

enum ShDir {
    LEFT = 0,
    RIGHT
};



class LCD_HD44780 {
	unsigned char displayMode;

    void writeInstr(int a);
    void writeData(int a);
    //int readData(); // not supported on RaspiRadio
    //int readBusy(); // not supported on RaspiRadio

public:
    LCD_HD44780(){displayMode =0;}
    void init();
    void switchBacklight(OnOff swBL);
    void switchDisplay(OnOff swDisp);
    void switchCursor(OnOff swCurs);
    void displayShift(ShDir dir);
    void clear();
    void setCursor(int row, int col);

    // line1 DDRAM addr: hex 0x00 ... 0x27, dec 0...39     \ memory for 40 characters per line
    // line2 DDRAM addr: hex 0x40 ... 0x67, dec 64...103   /
    void writeString(char* a, char addr = 0);
    void write(char* a, int len, char addr = 0);
};

#endif
