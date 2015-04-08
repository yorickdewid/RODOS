
#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**
*  @class ShiftList
*/
template < typename Type, int32_t len > class ShiftList  {

public:
    Type buffer[len];

    ShiftList()  { memset(buffer, 0, sizeof(buffer)); }
    void clear() { memset(buffer, 0, sizeof(buffer)); }

    /**  returns true == ok, false == fifo full */
    void put(const Type val) {
        for (int i = len - 1; i > 0; i--) buffer[i] = buffer[i-1];
        buffer[0] = val;
    }

    int32_t getLen()  { return len; }
    int32_t getSize() { return len*sizeof(Type); }
};


#ifndef NO_RODOS_NAMESPACE
}
#endif
