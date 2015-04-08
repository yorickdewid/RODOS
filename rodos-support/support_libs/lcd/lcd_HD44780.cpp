/*****************************************/
/** class LCD 							**/
/** athor: Michael Ruffer 				**/
/** Copyright (C) 2012 Michael Ruffer	**/
/** version: 1.0 						**/
/** date: 02.12.2012					**/
/*****************************************/



// line1 DDRAM Adr: hex 0x00 ... 0x27, dec 0...39     \ memory for 40 characters per line
// line2 DDRAM Adr: hex 0x40 ... 0x67, dec 64...103   /

#include "rodos.h"
#include "lcd_HD44780.h"
#include "hal/hal_gpio.h"



/******************LCD******************/
/*
 * Funktion Set Instruction
 */
#define FKT_SET_INSTR	0x20

#define DATA_LEN_FLAG	0x10
#define NUM_LINES_FLAG	0x08
#define FONT_FLAG		0x04


/*
 * Data Address Instructions
 */
#define CGRAM_ADDR_INSTR   0x40
#define DDRAM_ADDR_INSTR   0x80


/*
 * Display Control Instruction
 */
#define DISP_CTRL_INSTR  0x08

#define DISP_FLAG     0x04
#define CURS_FLAG     0x02
#define BLINK_FLAG    0x01


/*
 * Cursor or Display shift Instruction
 */
#define CURS_DISP_SH_INSTR 	0x10

#define D_C_FLAG			0x08	// set for display shift, clear for cursor move
#define DIR_FLAG   			0x04 	// set for right, clear for left


/*
 * Entry Mode Set Instruction
 */
#define ENT_MODE_INSTR	0x04

#define INC_DEC_FLAG	0x02 	// set for addr increment during write
#define S_FLAG			0x01	// set for display shift during write -> it seems that cursor doesn't move


/*********** hw dependent ************/
HAL_GPIO d4(GPIO_054); // PD6
HAL_GPIO d5(GPIO_055); // PD7
HAL_GPIO d6(GPIO_056); // PD8
HAL_GPIO d7(GPIO_057); // PD9
HAL_GPIO en(GPIO_058); // PD10
HAL_GPIO rw(GPIO_059); // PD11
HAL_GPIO rs(GPIO_067); // PE3
HAL_GPIO bl(GPIO_068); // PE4

//static void LCD_setRW(){rw.setPins(1);}
static void LCD_clrRW(){rw.setPins(0);}

static void LCD_setRS(){rs.setPins(1);}
static void LCD_clrRS(){rs.setPins(0);}

static void LCD_setE(){en.setPins(1);}
static void LCD_clrE(){en.setPins(0);}


static void LCD_setDataPortInput(){
	d4.init(false);
	d5.init(false);
	d6.init(false);
	d7.init(false);
}


static void LCD_setDataPortOutput(){
   d4.init(true);
   d5.init(true);
   d6.init(true);
   d7.init(true);
}


static void LCD_initGPIOs(){
	LCD_setDataPortInput();

	rs.init(true);
	rw.init(true);
	en.init(true);
	bl.init(true);

    LCD_clrRS();
    LCD_clrE();
}


static void LCD_setD(unsigned char data){
	d4.setPins(data & 0x01);
	d5.setPins((data >> 1) & 0x01);
	d6.setPins((data >> 2) & 0x01);
	d7.setPins((data >> 3) & 0x01);
}

//static unsigned char LCD_getD(){}

static void LCD_sleepUs(int us){
	int64_t startTimeInNs = NOW();

	while( (NOW()-startTimeInNs) < us*1000 ){}
}


void LCD_HD44780::switchBacklight(OnOff swBL){
    if (swBL){
    	bl.setPins(1);
    }
    else{
    	bl.setPins(0);
    }
}


/******************* HW independent **********************/
void LCD_HD44780::setCursor(int row, int col){
	char addr = col + row*0x40;
	writeInstr(DDRAM_ADDR_INSTR | addr);
}


void LCD_HD44780::writeString(char* a, char addr){
	int len=0;
	
	while (a[len] != '\0')len++;

	write(a,len,addr);
}	 


void LCD_HD44780::write(char* a, int len, char addr){
	int i;

	writeInstr(DDRAM_ADDR_INSTR | addr);	//set Cursor to adr 0x00

	for (i=0;i<len;i++){
		if (a[i]== 0xC3) {
			switch((uint8_t)a[i+1]){
			case 0xa4: // ä
				//writeData(0xE1);
				writeData('a');
				writeData('e');
				break;
			case 0xb6: // ö
				//writeData(0xEF);
				writeData('o');
				writeData('e');
				break;
			case 0xbc: // ü
				//writeData(0xF5);
				writeData('u');
				writeData('e');
				break;
			case 0x84: // Ä
				writeData('A');
				writeData('e');
				break;
			case 0x96: // Ö
				writeData('O');
				writeData('e');
				break;
			case 0x9c: // Ü
				writeData('U');
				writeData('e');
				break;
			case 0x9f: // ß
				writeData('s');
				writeData('s');
				break;
			}
			i++;
		}
		else {
			writeData(a[i]);
		}
	}
}


void LCD_HD44780::writeInstr(int a){
    LCD_setDataPortOutput();

    LCD_clrRW();
    LCD_clrRS();

    // high nibble
    LCD_setD(a>>4);
    LCD_setE();
    LCD_sleepUs(1);		// > 230 ns
    LCD_clrE();
    LCD_sleepUs(1);

    // low nibble
    LCD_setD(a);
    LCD_setE();
    LCD_sleepUs(1);		// > 230 ns
    LCD_clrE();

    LCD_setDataPortInput();
    LCD_sleepUs(45);    // >37us
}


// only for initialization
void writeInstr4Bit(int a){
    LCD_setDataPortOutput();

    LCD_clrRW();
    LCD_clrRS();

    LCD_setD(a);
    LCD_setE();
    LCD_sleepUs(1);		// > 230 ns
    LCD_clrE();

    LCD_setDataPortInput();
   	LCD_sleepUs(45);
}


void LCD_HD44780::writeData(int a){
    LCD_setDataPortOutput();

    LCD_clrRW();
    LCD_setRS();

    // high nibble
    LCD_setD(a>>4);
    LCD_setE();
    LCD_sleepUs(1);		// > 230 ns
    LCD_clrE();
    LCD_sleepUs(1);

    // low nibble
    LCD_setD(a);
    LCD_setE();
    LCD_sleepUs(1);		// > 230 ns
    LCD_clrE();

    LCD_setDataPortInput();
	LCD_sleepUs(50);    // >43us
}

/*
int LCD_HD44780::readData(){
    int a = 0;

    LCD_setDataPortInput();

    LCD_setRW();
    LCD_setRS();
    LCD_setE();
    LCD_sleepUs(1);                 // > 230 ns
    LCD_clrE();
    LCD_sleepUs(50);    // >43us

    a =  LCD_getD()<<4; // high nibble

    LCD_setE();
    LCD_sleepUs(1);                 // > 230 ns
    LCD_clrE();
    LCD_sleepUs(1);
    a |=  LCD_getD(); // low nibble

	return a;
}


int LCD_HD44780::readBusy() {
    int a = 0;

    LCD_setDataPortInput();

    LCD_setRW();
    LCD_clrRS();
    LCD_setE();
    LCD_sleepUs(1);                 // > 230 ns
    LCD_clrE();
    LCD_sleepUs(1);                 // > 230 ns


    a =  LCD_getD()<<4; // high nibble

    LCD_setE();
    LCD_sleepUs(1);                 // > 230 ns
    LCD_clrE();
    LCD_sleepUs(1);
    a |=  LCD_getD(); // low nibble

	return a;
}
*/

//LCD_HD44780::LCD_HD44780(){}

void LCD_HD44780::init(){
    LCD_initGPIOs();

	/********** Init procedure after power on *******************/
    LCD_sleepUs(20000);    //wait 20ms
    writeInstr4Bit(0x3);
    LCD_sleepUs(4100);
    writeInstr4Bit(0x3);
    LCD_sleepUs(100);
    writeInstr4Bit(0x3);
    writeInstr4Bit(0x2);
    writeInstr(FKT_SET_INSTR | NUM_LINES_FLAG); // 4bit Bus Mode, two-line-mode, 5*8 dots font display mode
	writeInstr(0x08);							// Display off
	clear();									// Display clear
	writeInstr(ENT_MODE_INSTR | INC_DEC_FLAG);	// shift cursor right,display didn't move
	/********** Init procedure end ******************************/

	displayMode = 0; // Display off, Cursor off, Blinking Cursor off
    writeInstr(DISP_CTRL_INSTR | displayMode);

/********define custom characters****/
	writeInstr(CGRAM_ADDR_INSTR | 0x08);
	writeData(0x0A);
	writeData(0x11);
	writeData(0x0A);
	writeData(0x00);
	writeData(0x04);
	writeData(0x11);
	writeData(0x0E);
	writeData(0x00);

	writeData(0x00);
	writeData(0x00);
	writeData(0x0A);
	writeData(0x00);
	writeData(0x04);
	writeData(0x11);
	writeData(0x0E);
	writeData(0x00);
	writeData(0x00);
	writeData(0x0A);
	writeData(0x00);
	writeData(0x04);
	writeData(0x00);
	writeData(0x11);
	writeData(0x0E);
	writeData(0x00);

	writeData(0x00);
	writeData(0x0A);
	writeData(0x00);
	writeData(0x04);
	writeData(0x04);
	writeData(0x11);
	writeData(0x0E);
	writeData(0x00);

	writeInstr(DDRAM_ADDR_INSTR | 0x00);   // set DDRAM address and cursor to 0x00

	switchDisplay(ON);
	switchBacklight(OFF);
}


void LCD_HD44780::switchDisplay(OnOff swDisp){
	if (swDisp) displayMode |= DISP_FLAG;
    else        displayMode &= ~(DISP_FLAG);

	writeInstr(DISP_CTRL_INSTR |displayMode);
}


void LCD_HD44780::switchCursor(OnOff swCurs){
	if (swCurs) displayMode |= CURS_FLAG;
    else        displayMode &= ~(CURS_FLAG);

	writeInstr(DISP_CTRL_INSTR |displayMode);
}


void LCD_HD44780::clear(){
    writeInstr(0x01);
    LCD_sleepUs(1600); // >1.52 ms
}


void LCD_HD44780::displayShift(ShDir dir){
    if (dir)    writeInstr(CURS_DISP_SH_INSTR | D_C_FLAG | DIR_FLAG); // display shift right
    else        writeInstr(CURS_DISP_SH_INSTR | D_C_FLAG); // display shift left
}
