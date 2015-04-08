
/**
* @file bytesey.h
* @author Sergio MOntenegro
*/

#ifndef __BYTESEX_H_
#define __BYTESEX_H_


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


/**  bigendian is standard net potocoll. 
 * Warning: Will be set in main.
 * never use beore main (in static constructurs)
 **/
extern bool isHostBigEndian; 

short swapShort(short sw_);
long swapLong(long lw_);
long long swapLonglong(long long llw_);

// bigendian / littleendian dependent funcions

short shortConvertHost2Net(short sw_); 
long  longConvertHost2Net(long sw_);
long long longlongConvertHost2Net(long long sw_); 


#ifndef NO_RODOS_NAMESPACE
}
#endif



#endif /* __MISC_H__ */
