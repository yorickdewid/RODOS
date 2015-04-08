

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


/**
 * @file gatewayGeneric.cc
 * @date 2012/05/01 7:07
 * @author Sergio Montenegro, Uni Würzburg
 *
 * Copyright 2009 DLR
 *
 * @brief gateway for middleware
 *
 */

#include "gateway/gateway.h"
#include "rodos.h"



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

extern long myNodeNr; // noremaly use getNodeNumber(), but gateway is intrinsic function
extern int64_t numberOfReceivedMsgsFromNetwork;

static Application application("Gateway", APID_GATEWAY_GENERIC);

/**************** Transmitter part of the gateway   ******************/

int32_t globalMsgSequenceCounter = 1;

Gateway::Gateway(Linkinterface* linkinterface_, bool forwardall_, bool enable_) :
    Subscriber(defaultGatewayTopic, nopPutter, "Gateway", true),
    Thread("gateway", NETWORKREADER_PRIORITY) {
    this->linkinterface = linkinterface_;
    this->forwardAll = forwardall_;
    this->linkIdentifier = linkinterface_->getLinkdentifier();
    enable(enable_);

    getTopicsToForwardFromOutside=true;
    externalsubscribers.init();
}

int32_t Gateway::numberOfNodes = 0;
SeenNode Gateway::seenNodes[MAX_NUMBER_OF_NODES];
Semaphore Gateway::seenNodesProtector;

int compare_SeenNode (const void *a, const void *b) {
    int32_t temp =  ((SeenNode*)a)->nodeID - ((SeenNode*)b)->nodeID;
    if(temp > 0)      return 1;
    else if(temp < 0) return -1;
    else              return 0;
}

bool Gateway::messageSeen(NetworkMessage& msg) {
    //SeenNode key;
    SeenNode* node;



    seenNodesProtector.enter();
    /*===========qsort and binsearch version, disabled for now ========================
    key.nodeID=msg.senderNode;

    node= (SeenNode*)bsearch(&key,seenNodes,numberOfNodes,sizeof(SeenNode),compare_SeenNode);

    if(!node) {
        if(numberOfNodes+1 > MAX_NUMBER_OF_NODES) {
            ERROR("There are more nodes than MAX_NUMBER_OF_NODES");
            return false;
        }
        key.lastMsgTime=msg.sentTime-1;
        seenNodes[numberOfNodes]=key;
        numberOfNodes++;

        qsort(seenNodes,numberOfNodes,sizeof(SeenNode),compare_SeenNode);
        node= (SeenNode*)bsearch(&key,seenNodes,numberOfNodes,sizeof(SeenNode),compare_SeenNode);
    }

    */

    //=============== simple search =======================================
       node=0;
       for(int i=0;i< numberOfNodes;i++){

           if(seenNodes[i].nodeID == msg.get_senderNode()){
               node = &seenNodes[i];
               break;
           }
       }

       if(!node){
           if(numberOfNodes >= MAX_NUMBER_OF_NODES) {
               ERROR("There are more nodes than MAX_NUMBER_OF_NODES");
               return false;
           }
           node=&seenNodes[numberOfNodes];
           numberOfNodes++;

           node->nodeID = msg.get_senderNode();
           node->lastMsgTime = msg.get_sentTime()-1;
       }

       // ==================================================


    int64_t msgSentTime = msg.get_sentTime();
    if(node->lastMsgTime >= msgSentTime) {
        if(node->lastMsgTime-msgSentTime > 5*SECONDS) {
            node->lastMsgTime= msgSentTime;
            seenNodesProtector.leave();
            return false;
        }
        seenNodesProtector.leave();
        return true;
    } else {
        node->lastMsgTime= msgSentTime;
        seenNodesProtector.leave();
        return false;
    }

}

/** Forward the message to the interface **/

long Gateway::put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
    if(!isEnabled) return 0;

    if(!forwardAll) {
        if(topicId !=0 && !externalsubscribers.find(topicId)) { return 0; }
    }

    networkOutProtector.enter();
    {
        prepareNetworkMessage(networkOutMessage,topicId,data,len);
        sendNetworkMessage(networkOutMessage);
    }
    networkOutProtector.leave();
    return 1;
}



void Gateway::sendNetworkMessage(NetworkMessage& msg) {
    //When implemented, buffering of the outgoing Messages should be done here.

    /*if(!forwardAll){
        if(msg.topicId !=0 && !externalsubscribers.find(msg.topicId)){
            return;
        }
    }*/

    networkOutProtector.enter(); // Also lock here if this function gets called from outside
    linkinterface->sendNetworkMsg(msg);
    networkOutProtector.leave();

}


/**************** Receiver part of the gateway   ********************/


void Gateway::setTopicsToForward(void* topicList) {
    getTopicsToForwardFromOutside=false;
    externalsubscribers = *(TopicListReport*)topicList;
}

void Gateway::addTopicsToForward(void* topicsWanted_) {
    getTopicsToForwardFromOutside=false;
    TopicListReport *topicsWanted = (TopicListReport*)topicsWanted_;
    for(int i = 0; i < topicsWanted->numberOfTopics; i++) {
        externalsubscribers.add(topicsWanted->topicList[i]);
    }
}

void Gateway::addTopicsToForward(TopicInterface* topicId1, TopicInterface* topicId2, TopicInterface* topicId3, TopicInterface* topicId4) {
    getTopicsToForwardFromOutside=false;

    externalsubscribers.add(topicId1->topicId);
    if(topicId2) { externalsubscribers.add(topicId2->topicId); }
    if(topicId3) { externalsubscribers.add(topicId3->topicId); }
    if(topicId4) { externalsubscribers.add(topicId4->topicId); }
}

void Gateway::resetTopicsToForward() {
    getTopicsToForwardFromOutside=false;
    externalsubscribers.init();
}



/*************************************************************************/

void Gateway::AnalyseAndDistributeMessagesFromNetwork() {

    if(networkInMessage.get_senderNode() == myNodeNr) {
        return;
    } // ***** discard messages from self

    if(!networkInMessage.isCheckSumOk()) {
        // PRINTF("Message dropped: Wrong Checksum %x != %x\n", networkInMessage.get_checksum(), networkInMessage.calculateCheckSum());
        return;
    }
    if(messageSeen(networkInMessage)) return;

    networkInMessage.dec_maxStepsToForward();
    numberOfReceivedMsgsFromNetwork++;

    long topicId           = networkInMessage.get_topicId();

    if(topicId == 0 && getTopicsToForwardFromOutside) { // This is a topic report of expected topics in network.
        if(linkinterface->isBroadcastLink) {
            addTopicsToForward(networkInMessage.userDataC); // for broadcast networks
        } else {
            setTopicsToForward(networkInMessage.userDataC); // for inteellignet networks-Swtiches
        }
        getTopicsToForwardFromOutside=true;

    } else if(topicId > 0) {
        /** now distribute locally (if not from self and not topicreports) **/


        NetMsgInfo msgInfo;
        msgInfo.linkId         = linkIdentifier;
        msgInfo.sentTime       = networkInMessage.get_sentTime();
        msgInfo.senderNode     = networkInMessage.get_senderNode();
        msgInfo.senderThreadId = networkInMessage.get_senderThreadId();

        ITERATE_LIST(TopicInterface, TopicInterface::topicList) {
            if(iter->topicId == topicId) {
                iter->publish(networkInMessage.userDataC, false, &msgInfo);
            }
        } // search all local topics

        //Publish for Routers to forward
        ((TopicInterface*)&defaultRouterTopic)->publish(&networkInMessage,false,&msgInfo);
    }
}

/*************************************************************************/
/**
 * one trhead to distribute internaly the incommig messages
 */



void Gateway::run() {
    bool     didSomething;
    int32_t  realMsgSize;


    linkinterface->init();
    linkinterface->setThreadToResume(this);

    while(1) {
        suspendCallerUntil(NOW()+ 1 * SECONDS);

        didSomething=true;
        while(didSomething) {
            didSomething=false;

            if(linkinterface->getNetworkMsg(networkInMessage, realMsgSize)) {

                if(realMsgSize < 0) {// The physical layer does not provide a "real" msg size by its own, but relies on the message header
                    AnalyseAndDistributeMessagesFromNetwork();
                } else {
                    if(realMsgSize >= ((int32_t)sizeof(NetworkMessage)-MAX_NETWORK_MESSAGE_LENGTH)) {
                        if(realMsgSize == (int32_t)networkInMessage.numberOfBytesToSend()) {
                            AnalyseAndDistributeMessagesFromNetwork();
                        } else {
                            PRINTF("Message dropped because expected size(%d) != real size(%d)\n",(int)networkInMessage.numberOfBytesToSend(),(int)realMsgSize);
                        }
                    } else {
                        PRINTF("Message dropped because received is even smaller than the header\n");
                    }
                }

                didSomething=true;
            }

        }
    }
} // run




void prepareNetworkMessage(NetworkMessage& netMsg, const long topicId,const void* data, int len) {
    netMsg.put_senderNode(myNodeNr); // Set node ID of sending node
    netMsg.put_topicId(topicId);     // ID of calling topic
    netMsg.put_sentTime(NOW());      // Timestamp
    netMsg.put_maxStepsToForward(10);
    netMsg.put_senderThreadId((uint32_t)Thread::getCurrentThread());
    netMsg.setUserData(data, len);
    netMsg.setCheckSum();
}

//Counter for automaticaly assigning linkidentifiers
//This is here because Linkinterface has no own source file
int Linkinterface::linkidentifierCounter=10000;

#ifndef NO_RODOS_NAMESPACE
}
#endif

