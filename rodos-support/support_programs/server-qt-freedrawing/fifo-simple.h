

/**
* @file fifo.h
* @date 2008/05/11 16:13
* @author Sergio Montenegro, Lutz Dittrich
*
* Copyright 2008 DLR
*
* @brief FIFO (first in first out) communication (header)
*
*/

#ifndef __FIFOSIMPLE_H__
#define __FIFOSIMPLE_H__


template <typename Type, int len > class Fifo {

private:
  /** internal buffer */
  Type buffer[len];

  /** write index */
  volatile int writeX;

  /** read index */
  volatile int readX;

  /** advance index to next position
  * with overflow to 0 to implement a ring
  */
  int advanceIndex(int index) {
    return ((index < (len-1)) ? (index+1) : 0);
  }


public:
  /** constructor */
  Fifo() {
    readX = 0;
    writeX = 0;
  }

  /**  returns true == ok, false == fifo full */
  bool put(const Type& val) {
    int index =  advanceIndex(writeX);
    if(index == readX) {
       return false; /* full! */
    }

    buffer[writeX] = val;
    writeX = index;
    return true;
  }

  /** return true == ok, false = fifo empty, val not modified */
  bool get(Type& val) {
    if(readX == writeX) {
      return false;
    }
    val = buffer[readX];
    readX = advanceIndex(readX);
    return true;
  }

  ///
  int getLen() { return len; }
};


#endif /* __FIFO_H__ */
