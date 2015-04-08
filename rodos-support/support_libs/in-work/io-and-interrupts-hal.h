

/*** WARNING: all functions to be implemented in c++ (do not use c due to link names) ***/

/****************** RODOS Hardware Interrupt Interface (HAL-Int) *********************/

void HW_setInterruptHandler(unsigned char index, void (*interruptHandler)() );

void HW_enableInterrupts();
void HW_disableInterrupts();

void HW_maskInterrupts(unsigned int interruptId);   /// not recomended very HW dependent
void HW_unmaskInterrupts(unsigned int interruptId); /// not recomended very HW dependent

unsigned long  HW_getUnhandledInterruptsCounter();


/****************************************************************************/
/******************** RODOS Hardware IO Interface  (HAL-IO) *****************/


/************** Generic for all drivers *****************/

class GenericIOInterface {
    void* context; 	///  HAL dependent, reserved for low level developers
public:

    GenericIOInterface() { } // create all drivers static but do not int there, init in config/reset

    /*** Impelemnted by each interface ****/

    virtual void config() { } /// Virtual, but do not call using a pointer, each config differs

    virtual void reset() { }

    virtual bool readyToSend()  {return true; }
    virtual bool dataReady()    {return false; }

    virtual int write(const char* sendBuf, int len) {return 0;}   // when returns is hw dependent
    virtual int read(char* recBuf, int maxlen) {return 0;}       // when returns is hw dependent
    virtual int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen) {return 0;} 

    /******* may be implemented by the user, called by corrsponding driver  *****/
    virtual void upCallSendReady() { } /// last transmit order is concluded
    virtual void upCallDataReady() { } /// data has arrived
};

/************ Simple Interfaces ************/

class GPIO : public GenericIOInterface { // one object per pin (grounp)
public:
    GPIO();
    void config(int pinIndex = 0, int numOfPins = 1, bool isOutput = true);


    /** From GenericIOInterface ***/
    void reset();

    bool readyToSend()  { return true; }
    bool dataReady()    { return true; }

    int write(const char* sendBuf, int len) 				{ return 0; }
    int read(char* recBuf, int maxlen) 					{ return 0; }
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen) 	{ return 0; }

    /*** Extra only for GPIO **/
    void setPins(unsigned int val);    // the lest significant bits will be used
    unsigned int readPins(); // the least significant bits will be set
};


class PWM : public GenericIOInterface { // one object per pwm  (+ one GPIO to set direction)
public:
    PWM();
    void config(int hwPwmGeneratorIndex = 0, int frequency = 1000, int numOfSteps = 256);

    /** From GenericIOInterface ***/
    void reset();

    bool readyToSend()  { return true; }
    bool dataReady()    { return true; }

    int write(const char* sendBuf, int len) 				{ return 0; }
    int read(char* recBuf, int maxlen) 					{ return 0; }
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen) 	{ return 0; }

    /*** Extra only for PWM **/
    void setDutyCycle(unsigned int c);    // 0 .. +numOfSteps
};


class DigitalToAnalog : public GenericIOInterface { // one object per analong pin output
public:
    DigitalToAnalog();
    void config(int daIndex = 0);

    /** From GenericIOInterface ***/
    void reset();

    bool readyToSend()  { return true; }
    bool dataReady()    { return true; }

    int write(const char* sendBuf, int len) 				{ return 0; }
    int read(char* recBuf, int maxlen) 					{ return 0; }
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen) 	{ return 0; }

    /*** Extra only for DA **/
    int getNumOfSteps(); // = 2^numOfBits, hardware dependent
    void setAnalogVal(int analogVal);    // -numOFSteps .. +numOfSteps
};


class AnalogToDigital : public GenericIOInterface { // one object per analong pin intput (+  GPIO-group multiplexer)
public:
    AnalogToDigital();
    void config(int adIndex = 0);
    void setAssociatedGPIOMultilexer(GPIO* muxControl); // in case we have an external analog mux

    /** From GenericIOInterface ***/
    void reset();

    bool readyToSend()  { return true; }
    bool dataReady();   // depending on analog multiplexer

    int write(const char* sendBuf, int len) 				{ return 0; }
    int read(char* recBuf, int maxlen) 					{ return 0; }
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen) 	{ return 0; }

    /*** Extra only for AD **/
    void selectChannel(unsigned int chanIndex); // eventaully using the external multiplex and GIOP muxControl
    bool readAnalogValue();
};


/***********************************************************
 * Message orinted connections
 * typical interfaces:
 *      UART (many complex devs are attached to UARTS)
 *	I2C
 *	SPI
 *	Ethernet
 *	CAN
 *	oneWire
 *	SpaceWire?
 *
 * Bus strucutres with 1xmaster multiple slave
 * (uart 845,I2C, onwWire, CAN) usually send first a command
 * (wirte) and then toggle to read -> we need a writeRead call
 * for such configurationes never use read(...)
 *
 *********************************************************/


class UART : public GenericIOInterface {
public:
    UART();

    /** From GenericIOInterface ***/
    void config(int uartIndex = 0, int baurate = 115200);
    void reset();

    bool readyToSend(); /// at least one character
    bool dataReady();   /// at least one character

    int write(const char* sendBuf, int len);  // when returns is hw dependent
    int read(char* recBuf, int maxlen);      // when returns is hw dependent
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen);

    /*** Extra only for UARTS **/
    bool putcharNoWait(char c);   // no waiting, test with readtoToSend()
    bool getcharNoWait(char &c); // no waiting, test with dataReady()

};

class I2C : public GenericIOInterface {
public:
    I2C();

    /** From GenericIOInterface ***/
#warning: how to config
    void config(int i2cIndex = 0, int speed = 400000);
    void reset();

    bool readyToSend(); /// at least one character
    bool dataReady();   /// at least one character

    int write(const char* sendBuf, int len);
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen);
};

class OneWire : public GenericIOInterface {
public:
    OneWire();

    /** From GenericIOInterface ***/
#warning: how to config
    void config(int onewIndex = 0, int speed = 115200);
    void reset();

    bool readyToSend(); /// at least one character
    bool dataReady();   /// at least one character

    int write(const char* sendBuf, int len);
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen);
};

class SPI : public GenericIOInterface {
public:
    SPI();

    /** From GenericIOInterface ***/
#warning: how to config
    void config(int spiIndex = 0);                   /// in case we have more than one SPI ports
    void setAssociatedGPIOMSelect(GPIO* muxControl); /// To select slaves
    void selectSleave(int slaveIndex = 0);           /// using the associated GPIO
    void reset();

    bool readyToSend(); /// at least one character
    bool dataReady();   /// at least one character

    int write(const char* sendBuf, int len);  // when returns is hw dependent
    int read(char* recBuf, int maxlen);      // when returns is hw dependent, usefull only if we are slave
    int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen);

};

class Ethernet : public GenericIOInterface {
public:
    Ethernet();

    /** From GenericIOInterface ***/
#warning: how to config
    void config(int ethIndex = 0);
    void reset();

    bool readyToSend(); /// at least one character
    bool dataReady();   /// at least one character

    int write(const char* sendBuf, int len);  // when returns is hw dependent
    int read(char* recBuf, int maxlen);      // when returns is hw dependent

};

class CAN : public GenericIOInterface {
public:
    CAN();

    /** From GenericIOInterface ***/
#warning: how to config
    void config(int canIndex = 0, int speed = 1000000);
    void reset();

    bool readyToSend(); /// at least one character
    bool dataReady();   /// at least one character

    int write(unsigned long objectID, const char* sendBuf, int len);  // when returns is hw dependent
    int read(unsigned long* objectID, char* recBuf, int maxlen);      // when returns is hw dependent

};



#include "stdio.h"

main() {
    printf("alles ok\n");
}
