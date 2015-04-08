

/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/



#pragma once

#include "gateway/linkinterface.h"
#include "gateway/gateway.h"
#include "gateway/networkmessage.h"
#include "hal/hal_uart.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @file linkinterfaceuart.h
 * @date 2012/012/03 10:07
 * @author TW
 *
 * Copyright 2009 DLR
 *
 * @brief subscribers as gateway
 *
 */


#define MAX_UART_MESSAGE_LENGTH (MAX_NETWORK_MESSAGE_LENGTH +6)

#define OUTPUT_BUF_SIZE     64
#if OUTPUT_BUF_SIZE < 2
#error "linkinterface uart: output buffer to small"
#endif

class LinkinterfaceUART : public Linkinterface, IOEventReceiver  {

    HAL_UART* uart;
    //unsigned char inputBuffer[MAX_UART_MESSAGE_LENGTH]; // bom, topicid, message eom
    unsigned char* inputBuffer;

    /* used to send messages in a block and not char by char
     * -> important for bluetooth
     * -> if message is longer than outputBuffer it is split into multiple HAL_UART::write calls
     */
    char outputBuffer[OUTPUT_BUF_SIZE];

    enum ReadStates {
        WAITING_FOR_START,
        READING,
        ESCAPE
    } readState;
    int readIndex;

    volatile bool maySend; // controlled by ^s, ^q from reader
    bool enaTXBreak;  // enable transmit interruption by received character

    NetworkMessage* inMsg;
    char currCharFromLastCall;
    char prevChar;
    int inputIndex;
    bool foundUART_BOM;

    volatile bool transmitinProgrss;

    char getcharWaiting();
    void putcharEncoded(const bool mark, const char c);
    int putcharEncodedToBuf(char* buf, const bool mark, const char c);
    bool sendUartBuffer(char* buf, int size);
    bool processByte(char input);
    void myPutChar(char c);

public:

    LinkinterfaceUART(HAL_UART* uart, long int id = -1, bool enaTXBreak = true);
    void init();

    bool isNetworkMsgSent();
    bool sendNetworkMsg(NetworkMessage& outgoingMessage);
    bool getNetworkMsg(NetworkMessage &inMsg, int32_t &numberOfReceivedBytess);

    void onWriteFinished(); // from IOEventReceiver
    void onDataReady();  // from IOEventReceiver
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

