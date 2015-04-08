/**
* @file networkMessages.cpp
* @date 2014/09/30
* @author Sergio Montenegro
*
* @brief Messages to be exchanged between nodes and network
*
*/

#include "rodos.h"
#include "macros_bits_bytes.h"

int32_t NetworkMessage::setUserData(const void* data, uint32_t len) {
    len =  MIN(len, MAX_NETWORK_MESSAGE_LENGTH);
    memcpy(userDataC, data, len);
    put_len(len);
    return len;
}

/** Copies data from message received to a user provided buffer.
 *
 * @param destination pointer to output data stream received
 * @param length of message buffer, supposed to be <= MAX_NETWORK_MESSAGE_LENGTH
 * @return length of message copied
 */
uint32_t NetworkMessage::getUserData(void* destination, uint32_t maxLen) {
    maxLen = MIN(maxLen, get_len());
    memcpy(destination, userDataC, maxLen);
    return maxLen;
}
